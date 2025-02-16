// This file really needs some cleaning up
// I should do this

#pragma once

#include <optional>
#include <vector>
#include <iostream>
#include <memory>
#include "parse_nodes.hpp"
#include "tokens.hpp"

class Parser{
public:
    Parser(std::vector<Token> _tokens)
        : tokens(_tokens), t_index(0) {}

    std::optional<std::shared_ptr<ParseNodes::ExprArit>> parse_arit(){
        if(!peek().has_value()){
            return {};
        }

        std::shared_ptr<ParseNodes::ExprArit> arit(new ParseNodes::ExprArit);

        if(peek().value().type == TokenTypes::_integer_literal){
            std::shared_ptr<ParseNodes::ExprIntLit> int_lit(new ParseNodes::ExprIntLit);
            int_lit->int_lit = consume();
            arit->var = int_lit;
            return arit;
        }

        if(peek().value().type == TokenTypes::_identifier){
            std::shared_ptr<ParseNodes::ExprIdent> ident(new ParseNodes::ExprIdent);
            ident->identifier = consume();
            arit->var = ident;
            return arit;
        }

        if(peek().value().type == TokenTypes::_addition or
        peek().value().type == TokenTypes::_multiplication){
            std::shared_ptr<ParseNodes::ExprSgn> sign(new ParseNodes::ExprSgn);
            sign->oper = consume();
            arit->var = sign;
            return arit;
        }

        return {};
    }

    std::optional<std::shared_ptr<ParseNodes::ExprOper>> parse_oper(){
        if(!peek().has_value()){
            return {};
        }

        std::shared_ptr<ParseNodes::ExprOper> parsed_oper(new ParseNodes::ExprOper);
        while(peek().has_value() and peek().value().type != TokenTypes::_semicolon){
            auto parsed_arit = parse_arit();
            if(!parsed_arit.has_value()){
                std::cerr<<"Invalid arithmetic expression";
                exit(EXIT_FAILURE);
            }
            parsed_oper->arit.push_back(parsed_arit.value());
        }

        return parsed_oper;
    }

    std::optional<std::shared_ptr<ParseNodes::Expr>> parse_expr(bool chk_mul = true){
        if(!peek().has_value()){
            return {};
        }

        std::shared_ptr<ParseNodes::Expr> expression(new ParseNodes::Expr);
        if((peek(1).value().type == TokenTypes::_addition or 
        peek(1).value().type == TokenTypes::_multiplication) and chk_mul){

            /*std::shared_ptr<ParseNodes::ExprOper> exprOper(new ParseNodes::ExprOper);
            std::optional<std::shared_ptr<ParseNodes::Expr>> parsedExprLeft = parse_expr(false);

            if(!parsedExprLeft.has_value()){
                std::cerr << "Expected expressions on each side of operator" << std::endl;
                exit(EXIT_FAILURE);
            }

            exprOper->left = parsedExprLeft.value();
            exprOper->oper = consume();
            std::optional<std::shared_ptr<ParseNodes::Expr>> parsedExprRight = parse_expr();

            if(!parsedExprRight.has_value()){
                std::cerr << "Expected expression on each side of operator" << std::endl;
                exit(EXIT_FAILURE);
            }

            exprOper->right = parsedExprRight.value();
            expression->var = exprOper;*/

            auto parsed_oper = parse_oper();
            if(!parsed_oper.has_value()){
                std::cerr << "invalid operation!" << std::endl;
                exit(EXIT_FAILURE);
            }

            (*expression).var = parsed_oper.value();
            return expression;
        }

        if(peek().value().type == TokenTypes::_integer_literal){
            std::shared_ptr<ParseNodes::ExprIntLit> exprIntLit(new ParseNodes::ExprIntLit());
            exprIntLit->int_lit = consume();
            (*expression).var = exprIntLit;
            return expression;
        }

        if(peek().value().type == TokenTypes::_identifier){
            std::shared_ptr<ParseNodes::ExprIdent> exprIdent(new ParseNodes::ExprIdent());
            exprIdent->identifier = consume();
            (*expression).var = exprIdent;
            return expression;
        }

        return {};
    }

    std::optional<std::shared_ptr<ParseNodes::Stmt>> parse_stmt(){
        if(!peek().has_value()){
            return {};
        }


        if(peek().value().type == TokenTypes::_exit){
            consume();

            if(!peek().has_value() or peek().value().type != TokenTypes::_open_parentheses){
                std::cerr<<"Expected '('"<<std::endl;
                exit(EXIT_FAILURE);
            }

            consume();

            std::optional<std::shared_ptr<ParseNodes::Expr>> expression = parse_expr();
            if(!expression.has_value()){
                return {};
            }

            std::shared_ptr<ParseNodes::Stmt> statement(new ParseNodes::Stmt);
            std::shared_ptr<ParseNodes::StmtExit> exitStatement(new ParseNodes::StmtExit);
            exitStatement->expression = expression.value();
            statement->var = exitStatement;

            if(!peek(1).has_value() or peek().value().type != TokenTypes::_close_parentheses or 
            peek(1).value().type != TokenTypes::_semicolon){
                std::cerr<<"Invalid Syntax!"<<std::endl;
                exit(EXIT_FAILURE);
            }

            consume(2);

            return statement;
        }

        if(peek().value().type == TokenTypes::_let){
            consume();
            std::shared_ptr<ParseNodes::Stmt> statement(new ParseNodes::Stmt);

            if(!peek().has_value() or peek().value().type != TokenTypes::_identifier){
                std::cerr<<"Expected Identifier"<<std::endl;
                exit(EXIT_FAILURE);
            }

            Token identifier = consume();

            if(!peek().has_value() or peek().value().type != TokenTypes::_equals){
                return {};
            }

            consume();

            std::optional<std::shared_ptr<ParseNodes::Expr>> expression = parse_expr();

            if(!expression.has_value()){
                std::cerr<<"noexpr"<<std::endl;
                return {};
            }

            std::shared_ptr<ParseNodes::StmtLet> letStatement(new ParseNodes::StmtLet);
            letStatement->identifier = identifier;
            letStatement->expression = expression.value();
            statement->var = letStatement;

            if(!peek().has_value() or peek().value().type != TokenTypes::_semicolon){
                return {};
            }

            consume();

            return statement;
        }

        return {};
    }

    std::optional<std::shared_ptr<ParseNodes::Prog>> parse_prog(){
        if(!peek().has_value()){
            return {};
        }

        std::shared_ptr<ParseNodes::Prog> progNode(new ParseNodes::Prog);

        while(peek().has_value()){
            auto statement = parse_stmt();
            if(!statement.has_value()){
                std::cerr<<"Syntax Error Not a Statement!" << std::endl;
                exit(EXIT_FAILURE);
            }

            progNode->statements.push_back(statement.value());
        }

        return progNode;
    }

private:
    std::vector<Token> tokens;
    size_t t_index;

    std::optional<Token> peek(int ahead = 0){
        if(t_index + ahead >= tokens.size()){
            return {};
        }

        return tokens[t_index + ahead];
    }

    Token consume(int ammount = 1){
        if(t_index >= tokens.size()){
            std::cerr<<"Invalid consume operation!"<<std::endl;
            exit(EXIT_FAILURE);
        }

        while(--ammount) t_index++;
        return tokens[t_index++]; //hopefully this works
    }
};
