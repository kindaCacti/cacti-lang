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
        pop("rbx");
        pop("rax");
        if(type == TokenTypes::_addition) //out << "   add rax, rbx\n";
            add_ins(ASMNode{.instruction=Instructions::_add, .data="rax, rbx"});
        if(type == TokenTypes::_subtraction) //out << "   sub rax, rbx\n";
            add_ins(ASMNode{.instruction=Instructions::_sub, .data="rax, rbx"});
        if(type == TokenTypes::_multiplication) //out << "   imul rbx\n";
            add_ins(ASMNode{.instruction=Instructions::_imul, .data="rbx"});
        if(type == TokenTypes::_division) //out << "   idiv rbx\n";
            add_ins(ASMNode{.instruction=Instructions::_idiv, .data="rbx"});
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

                int offset = gen->stack_loc - gen->variables_loc[identifier]->stack_loc - 1;
                
                std::string data = "rax, QWORD [rsp+" + std::to_string(offset * 8) + "]";
                gen->add_ins(ASMNode{.instruction=Instructions::_mov, .data=data});
                //gen->out << "   mov rax, QWORD [rsp + " << offset * 8 <<"]\n";
                gen->push("rax");
            }

            void operator()(const std::shared_ptr<ParseNodes::ExprIntLit> int_lit){
                //this is kind of stupid but maybe I'll change that later
                std::string data = "rax, " + int_lit->int_lit.data.value();
                gen->add_ins(ASMNode{.instruction=Instructions::_mov, .data=data});
                //gen->out << "   mov rax, " << int_lit->int_lit.data.value() <<"\n";
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
                std::string data = "rax, " + int_lit->int_lit.data.value();
                gen->add_ins(ASMNode{.instruction=Instructions::_mov, .data=data});
                //gen->out << "   mov rax, " << int_lit->int_lit.data.value() <<"\n";
                gen->push("rax");
            }

            void operator()(const std::shared_ptr<ParseNodes::ExprIdent> ident_expr){
                std::string identifier = ident_expr->identifier.data.value();

                if(gen->variables_loc.find(identifier) == gen->variables_loc.end()){
                    std::cerr<<"Uninitialized identifier!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                int offset = gen->stack_loc - gen->variables_loc[identifier]->stack_loc - 1;

                std::string data = "rax, QWORD [rsp+" + std::to_string(offset * 8) + "]";
                gen->add_ins(ASMNode{.instruction=Instructions::_mov, .data=data});
                //gen->out << "   mov rax, QWORD [rsp + " << offset * 8 <<"]\n";
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

    void generate_sign(std::shared_ptr<ParseNodes::BinSign> bin_sign, std::string label){
        if(bin_sign->sign.type == TokenTypes::_cmp_eq) 
            add_ins(ASMNode{.instruction=Instructions::_jne, .data=label});
            //out << "   jne _if" << std::to_string(ifid) <<"\n";
        if(bin_sign->sign.type == TokenTypes::_cmp_geq) 
            add_ins(ASMNode{.instruction=Instructions::_jnae, .data=label});
            //out << "   jnae _if" << std::to_string(ifid) <<"\n";
        if(bin_sign->sign.type == TokenTypes::_cmp_gt)
            add_ins(ASMNode{.instruction=Instructions::_jng, .data=label});
            // out << "   jng _if" << std::to_string(ifid) <<"\n";
        if(bin_sign->sign.type == TokenTypes::_cmp_leq) 
            add_ins(ASMNode{.instruction=Instructions::_jnbe, .data=label});
            //out << "   jnbe _if" << std::to_string(ifid) <<"\n";
        if(bin_sign->sign.type == TokenTypes::_cmp_lt)
            add_ins(ASMNode{.instruction=Instructions::_jnl, .data=label});
            //out << "   jnl _if" << std::to_string(ifid) <<"\n";
        if(bin_sign->sign.type == TokenTypes::_cmp_neq) 
            add_ins(ASMNode{.instruction=Instructions::_je, .data=label});
            //out << "   je _if" << std::to_string(ifid) <<"\n";
    }

    void generate_exprbin(const std::shared_ptr<ParseNodes::ExprBin> bin_expression, const std::string& label){
        generate_expression(bin_expression->left);
        generate_expression(bin_expression->right);
        pop("rbx");
        pop("rax");

        add_ins(ASMNode{.instruction=Instructions::_cmp, .data="rax, rbx"});
        out << "   cmp rax, rbx\n";
        generate_sign(bin_expression->sign, label);
    }

    void generate_statement(const std::shared_ptr<ParseNodes::Stmt> statement){
        std::stringstream out;

        struct StmtVisitor{
            StmtVisitor(CodeGenerator* _gen) : gen(_gen) {}

            void operator()(const std::shared_ptr<ParseNodes::StmtExit> exit_statement){
                gen->generate_expression(exit_statement->expression);

                gen->add_ins(ASMNode{.instruction=Instructions::_mov, .data="rax, 60"});
                gen->out << "   mov rax, 60\n";
                gen->pop("rdi");
                gen->add_ins(ASMNode{.instruction=Instructions::_syscall, .data=""});
                gen->out << "   syscall\n";
            }

            void operator()(const std::shared_ptr<ParseNodes::StmtLet> let_statement){
                std::string identifier = let_statement->identifier.data.value();
                if(gen->variables_loc.find(identifier) != gen->variables_loc.end()){
                    std::cerr<<"such variable already exists!"<<std::endl;
                    exit(EXIT_FAILURE);
                }

                Var tmp{.stack_loc = gen->stack_loc,
                .identifier = let_statement->identifier.data.value()};
                gen->variables_loc[identifier] = std::shared_ptr<Var>(new Var(tmp));
                gen->var_stack.push(gen->variables_loc[identifier]);

                gen->generate_expression(let_statement->expression);
            }
        
            void operator()(const std::shared_ptr<ParseNodes::StmtBlck> statement_block){
                gen->stack_positions.push(gen->stack_loc);

                for(auto stmt : statement_block->statements){
                    gen->generate_statement(stmt);
                }

                while(gen->stack_loc > gen->stack_positions.top()){
                    // might change to just moving stack pointer
                    gen->pop("rax");
                    gen->variables_loc.erase(gen->variables_loc.find(gen->var_stack.top()->identifier));
                    gen->var_stack.pop();
                }

                gen->stack_positions.pop();
            }
        
            void operator()(const std::shared_ptr<ParseNodes::StmtIf> if_statement){
                size_t cid = gen->if_id;
                gen->if_id++;
                std::string label = "_if" + std::to_string(cid);

                gen->generate_exprbin(if_statement->expression, label);
                gen->generate_statement(if_statement->statement);

                gen->add_ins(ASMNode{.instruction=Instructions::_label, .data=label + ":"});
                gen->out << "_if" << std::to_string(cid) << ":" << std::endl;
            }
        
            void operator()(const std::shared_ptr<ParseNodes::StmtAssign> assign_statement){
                std::string& identifier = assign_statement->identifier.data.value();
                if(gen->variables_loc.find(identifier) == gen->variables_loc.end()){
                    std::cerr << "Variable " << identifier << " does not exist" << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->generate_expression(assign_statement->expression);
                gen->update_variable(identifier);
            }
        private:
            CodeGenerator* gen;
        };

        StmtVisitor visitor(this);
        std::visit(visitor, statement->var);
    }

    std::string generate_program(){
        add_ins(ASMNode{.instruction=Instructions::_global, .data="global _start"});
        add_ins(ASMNode{.instruction=Instructions::_label, .data="_start:"});
        out << "global _start\n";
        out << "_start:\n";
        

        for(const std::shared_ptr<ParseNodes::Stmt> statement : root->statements){
            generate_statement(statement);
        }

        add_ins(ASMNode{.instruction=Instructions::_mov, .data="rax, 60"});
        add_ins(ASMNode{.instruction=Instructions::_mov, .data="rdi, 0"});
        add_ins(ASMNode{.instruction=Instructions::_syscall, .data=""});
        out << "   mov rax, 60\n";
        out << "   mov rdi, 0\n";
        out << "   syscall\n";

        streamify_code();
        return out.str();
    }

    void push(const std::string reg){
        add_ins(ASMNode{.instruction=Instructions::_push, .data=reg});
        out << "   push "<< reg <<"\n";
        stack_loc++;
    }

    void pop(const std::string reg){
        if(stack_loc <= 0){
            std::cerr << "value not ton stack!" << std::endl;
            exit(EXIT_FAILURE);
        }

        add_ins(ASMNode{.instruction=Instructions::_pop, .data=reg});
        out << "   pop " << reg << "\n";
        stack_loc--;
    }

    void update_variable(const std::string& var){
        pop("rcx");
        size_t ammount = (stack_loc - variables_loc[var]->stack_loc) * 8;
        add_ins(ASMNode{.instruction=Instructions::_mov, .data="rbx, rsp"});
        add_ins(ASMNode{.instruction=Instructions::_mov, .data="rax, " + std::to_string(ammount)});
        add_ins(ASMNode{.instruction=Instructions::_add, .data="rsp, rax"});
        add_ins(ASMNode{.instruction=Instructions::_mov, .data="rax, rcx"});
        add_ins(ASMNode{.instruction=Instructions::_push, .data="rax"});
        add_ins(ASMNode{.instruction=Instructions::_mov, .data="rsp, rbx"});
        out << "   mov rbx, rsp\n";
        out << "   mov rax, " << ammount <<"\n";
        out << "   add rsp, rax\n";
        out << "   mov rax, rcx" << "\n";
        out << "   push rax\n";
        out << "   mov rsp, rbx\n";
    }

    void add_ins(ASMNode ins){
        if(!instructions.size()){
            instructions.push_back(ins);
            return;
        }

        if(ins.instruction == Instructions::_pop and 
        instructions.back().instruction == Instructions::_push and
        instructions.back().data == ins.data){
            instructions.pop_back();
            return;
        }

        if(ins.instruction == Instructions::_push and 
        instructions.back().instruction == Instructions::_pop and
        instructions.back().data == ins.data){
            instructions.pop_back();
            return;
        }

        instructions.push_back(ins);
    }

    std::stringstream conv_instruction_type(Instructions ins_type){
        std::stringstream tmp;

        if(ins_type == Instructions::_syscall)  tmp << "   syscall ";
        if(ins_type == Instructions::_push)     tmp << "   push ";
        if(ins_type == Instructions::_pop)      tmp << "   pop ";
        if(ins_type == Instructions::_mov)      tmp << "   mov ";
        if(ins_type == Instructions::_add)      tmp << "   add ";
        if(ins_type == Instructions::_sub)      tmp << "   sub ";
        if(ins_type == Instructions::_imul)     tmp << "   imul ";
        if(ins_type == Instructions::_idiv)     tmp << "   idiv ";
        if(ins_type == Instructions::_cmp)      tmp << "   cmp ";
        if(ins_type == Instructions::_jne)      tmp << "   jne ";
        if(ins_type == Instructions::_jnae)     tmp << "   jnae ";
        if(ins_type == Instructions::_jng)      tmp << "   jng ";
        if(ins_type == Instructions::_jnbe)     tmp << "   jnbe ";
        if(ins_type == Instructions::_jnl)      tmp << "   jnl ";
        if(ins_type == Instructions::_je)       tmp << "   je ";

        return tmp;
    }

    void streamify_code(){
        out = std::stringstream();
        for(ASMNode ins : instructions){
            out << conv_instruction_type(ins.instruction).str() << ins.data << "\n";
        }
    }

    std::vector<ASMNode> instructions;
    std::unordered_map<std::string, std::shared_ptr<Var>> variables_loc;
    std::stringstream out;
    std::stack<std::shared_ptr<RPNNode>> signStack;
    std::stack<size_t> stack_positions;
    std::stack<std::shared_ptr<Var>> var_stack;
    size_t stack_loc = 0;
    size_t if_id = 0;
};