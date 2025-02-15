#pragma once

#include <sstream>
#include <string>
#include <optional>
#include <iostream>
#include <unordered_map>
#include "parse_nodes.hpp"

class CodeGenerator{
    ParseNodes::Prog root;
public:
    CodeGenerator(ParseNodes::Prog _root):
        root(_root) {}
    
    void generate_expression(const ParseNodes::Expr& expression){
        struct ExprVisitor{
            ExprVisitor(CodeGenerator* _gen) : gen(_gen) {}

            void operator()(const ParseNodes::ExprIntLit& int_lit){
                //this is kind of stupid but maybe I'll change that later
                gen->out << "   mov rax, " << int_lit.int_lit.data.value() <<"\n";
                gen->push("rax");
            }

            void operator()(const ParseNodes::ExprIdent& let_expr){
                std::string identifier = let_expr.identifier.data.value();

                if(gen->variables_loc.find(identifier) == gen->variables_loc.end()){
                    std::cerr<<"Uninitialized identifier!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                int offset = gen->stack_loc - gen->variables_loc[identifier].stack_loc - 1;

                gen->out << "   mov rax, QWORD [rsp + " << offset * 8 <<"]\n";
                gen->push("rax");
            }
        private:
            CodeGenerator* gen;
        };

        ExprVisitor visitor(this);
        std::visit(visitor, expression.var);
    }

    void generate_statement(const ParseNodes::Stmt& statement){
        std::stringstream out;

        struct StmtVisitor{
            StmtVisitor(CodeGenerator* _gen) : gen(_gen) {}

            void operator()(const ParseNodes::StmtExit& exit_statement){
                gen->generate_expression(exit_statement.expression);

                gen->out << "   mov rax, 60\n";
                gen->pop("rdi");
                gen->out << "   syscall\n";
            }

            void operator()(const ParseNodes::StmtLet& let_statement){
                std::string identifier = let_statement.identifier.data.value();
                if(gen->variables_loc.find(identifier) != gen->variables_loc.end()){
                    std::cerr<<"such variable already exists!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                gen->variables_loc[identifier] = Var{.stack_loc = gen->stack_loc};
                gen->generate_expression(let_statement.expression);
            }
        private:
            CodeGenerator* gen;
        };

        StmtVisitor visitor(this);
        std::visit(visitor, statement.var);
    }

    std::string generate_program(){
        out << "global _start\n";
        out << "_start:\n";

        for(const ParseNodes::Stmt& statement : root.statements){
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
        out << "   pop " << reg << "\n";
        stack_loc--;
    }

    std::unordered_map<std::string, Var> variables_loc;
    std::stringstream out;
    size_t stack_loc = 0;
};