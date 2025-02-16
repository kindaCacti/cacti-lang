#pragma once

#include <sstream>
#include <string>
#include <optional>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <stack>
#include <queue>
#include "parse_nodes.hpp"
#include "generator_sup.hpp"

class CodeGenerator{
    std::shared_ptr<ParseNodes::Prog> root;
public:
    CodeGenerator(std::shared_ptr<ParseNodes::Prog> _root):
        root(_root) {}
    
    void generate_operators(TokenTypes type){
        pop("rax");
        pop("rbx");
        if(type == TokenTypes::_addition) out << "   add rax, rbx\n";
        if(type == TokenTypes::_multiplication) out << "   imul rbx\n";
        push("rax");
    }

    void generate_arit(std::shared_ptr<ParseNodes::ExprArit> expression){
        struct AritVisitor{
            AritVisitor(CodeGenerator* _gen) : gen(_gen) {}

            void operator()(const std::shared_ptr<ParseNodes::ExprIdent> ident_expr){
                std::string identifier = ident_expr->identifier.data.value();

                if(gen->variables_loc.find(identifier) == gen->variables_loc.end()){
                    std::cerr<<"Uninitialized identifier!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                int offset = gen->stack_loc - gen->variables_loc[identifier].stack_loc - 1;

                gen->out << "   mov rax, QWORD [rsp + " << offset * 8 <<"]\n";
                gen->push("rax");
            }

            void operator()(const std::shared_ptr<ParseNodes::ExprIntLit> int_lit){
                //this is kind of stupid but maybe I'll change that later
                gen->out << "   mov rax, " << int_lit->int_lit.data.value() <<"\n";
                gen->push("rax");
            }
            
            void operator()(std::shared_ptr<ParseNodes::ExprSgn> sign){
                RPNNode tmp = RPNNode{.type=sign->oper.type, .value = sign->oper.data.value()};

                do{
                    if(gen->signStack.size() == 0) break;
                    RPNNode prev = *gen->signStack.top();
                    int cval = operator_weights[tmp.type];
                    int lval = operator_weights[prev.type];

                    if(lval > cval) break;

                    gen->generate_operators(prev.type);
                    gen->signStack.pop();
                }while(true);
                gen->signStack.push(std::shared_ptr<RPNNode>(new RPNNode(tmp)));
            }
        private:
            CodeGenerator* gen;
        };

        AritVisitor visitor(this);
        std::visit(visitor, expression->var);
    }
    
    void generate_expression(const std::shared_ptr<ParseNodes::Expr> expression){
        struct ExprVisitor{
            ExprVisitor(CodeGenerator* _gen) : gen(_gen) {}

            void operator()(const std::shared_ptr<ParseNodes::ExprIntLit> int_lit){
                //this is kind of stupid but maybe I'll change that later
                gen->out << "   mov rax, " << int_lit->int_lit.data.value() <<"\n";
                gen->push("rax");
            }

            void operator()(const std::shared_ptr<ParseNodes::ExprIdent> ident_expr){
                std::string identifier = ident_expr->identifier.data.value();

                if(gen->variables_loc.find(identifier) == gen->variables_loc.end()){
                    std::cerr<<"Uninitialized identifier!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                int offset = gen->stack_loc - gen->variables_loc[identifier].stack_loc - 1;

                gen->out << "   mov rax, QWORD [rsp + " << offset * 8 <<"]\n";
                gen->push("rax");
            }

            // Using reverse polish notation to include operator hierarchy
            void operator()(const std::shared_ptr<ParseNodes::ExprOper> operator_expr){
                for(auto _arit : operator_expr->arit){
                    gen->generate_arit(_arit);
                }

                while(gen->signStack.size()){
                    gen->generate_operators(gen->signStack.top()->type);
                    gen->signStack.pop();
                }
            }
        private:
            CodeGenerator* gen;
        };

        ExprVisitor visitor(this);
        std::visit(visitor, expression->var);
    }

    void generate_statement(const std::shared_ptr<ParseNodes::Stmt> statement){
        std::stringstream out;

        struct StmtVisitor{
            StmtVisitor(CodeGenerator* _gen) : gen(_gen) {}

            void operator()(const std::shared_ptr<ParseNodes::StmtExit> exit_statement){
                gen->generate_expression(exit_statement->expression);

                gen->out << "   mov rax, 60\n";
                gen->pop("rdi");
                gen->out << "   syscall\n";
            }

            void operator()(const std::shared_ptr<ParseNodes::StmtLet> let_statement){
                std::string identifier = let_statement->identifier.data.value();
                if(gen->variables_loc.find(identifier) != gen->variables_loc.end()){
                    std::cerr<<"such variable already exists!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                gen->variables_loc[identifier] = Var{.stack_loc = gen->stack_loc};
                gen->generate_expression(let_statement->expression);
            }
        private:
            CodeGenerator* gen;
        };

        StmtVisitor visitor(this);
        std::visit(visitor, statement->var);
    }

    std::string generate_program(){
        out << "global _start\n";
        out << "_start:\n";

        for(const std::shared_ptr<ParseNodes::Stmt> statement : root->statements){
            generate_statement(statement);
        }

        out << "   mov rax, 60\n";
        out << "   mov rdi, 0\n";
        out << "   syscall\n";

        return out.str();
    }

    struct Var{
        size_t stack_loc;
    };

    void push(const std::string reg){
        out << "   push "<< reg <<"\n";
        stack_loc++;
    }

    void pop(const std::string reg){
        if(stack_loc <= 0){
            std::cerr << "value not ton stack!" << std::endl;
            exit(EXIT_FAILURE);
        }

        out << "   pop " << reg << "\n";
        stack_loc--;
    }

    std::unordered_map<std::string, Var> variables_loc;
    std::stringstream out;
    std::stack<std::shared_ptr<RPNNode>> signStack;
    size_t stack_loc = 0;
};