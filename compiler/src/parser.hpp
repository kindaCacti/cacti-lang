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

    std::optional<std::shared_ptr<ParseNodes::Expr>> parse_expr(bool chk_mul = true){
        if(!peek().has_value()){
            return {};
        }

        std::shared_ptr<ParseNodes::Expr> expression(new ParseNodes::Expr);
        if(peek(1).value().type == TokenTypes::_addition and chk_mul){
            std::shared_ptr<ParseNodes::ExprOper> exprOper(new ParseNodes::ExprOper);
            std::optional<std::shared_ptr<ParseNodes::Expr>> parsedExprLeft = parse_expr(false);

            if(!parsedExprLeft.has_value()){
                std::cerr << "Expected expressions on each side of operator" << std::endl;
                exit(EXIT_FAILURE);
            }

            exprOper->left = parsedExprLeft.value();
            exprOper->oper = consume().value().data.value();
            std::optional<std::shared_ptr<ParseNodes::Expr>> parsedExprRight = parse_expr();

            if(!parsedExprRight.has_value()){
                std::cerr << "Expected expression on each side of operator" << std::endl;
                exit(EXIT_FAILURE);
            }

            exprOper->right = parsedExprRight.value();
            expression->var = exprOper;

            return expression;
        }

        if(peek().value().type == TokenTypes::_integer_literal){
            std::shared_ptr<ParseNodes::ExprIntLit> exprIntLit(new ParseNodes::ExprIntLit());
            exprIntLit->int_lit = consume().value();
            (*expression).var = exprIntLit;
            return expression;
        }

        if(peek().value().type == TokenTypes::_identifier){
            std::shared_ptr<ParseNodes::ExprIdent> exprIdent(new ParseNodes::ExprIdent());
            exprIdent->identifier = consume().value();
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

            consume();
            consume();

            return statement;
        }

        if(peek().value().type == TokenTypes::_let){
            consume();
            std::shared_ptr<ParseNodes::Stmt> statement(new ParseNodes::Stmt);

            if(!peek().has_value() or peek().value().type != TokenTypes::_identifier){
                std::cerr<<"Expected Identifier"<<std::endl;
                exit(EXIT_FAILURE);
            }

            Token identifier = consume().value();

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

    std::optional<Token> consume(){
        if(t_index >= tokens.size()){
            return {};
        }

        return tokens[t_index++]; //hopefully this works
    }
};
