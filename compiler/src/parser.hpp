#pragma once

#include <optional>
#include <vector>
#include <iostream>
#include "parse_nodes.hpp"
#include "tokens.hpp"

class Parser{
public:
    Parser(std::vector<Token> _tokens)
        : tokens(_tokens), t_index(0) {}

    std::optional<ParseNodes::Expr> parse_expr(){
        if(!peek().has_value()){
            return {};
        }

        if(peek().value().type == TokenTypes::_integer_literal){
            return ParseNodes::Expr {.var = ParseNodes::ExprIntLit {.int_lit = consume().value()}};
        }

        if(peek().value().type == TokenTypes::_identifier){
            return ParseNodes::Expr {.var = ParseNodes::ExprIdent {.identifier = consume().value()}};
        }

        return {};
    }

    std::optional<ParseNodes::Stmt> parse_stmt(){
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

            std::optional<ParseNodes::Expr> expression = parse_expr();
            if(!expression.has_value()){
                return {};
            }

            ParseNodes::Stmt statement =  ParseNodes::Stmt {.var = ParseNodes::StmtExit {.expression = expression.value()}};

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
            ParseNodes::Stmt letStatement;

            if(!peek().has_value() or peek().value().type != TokenTypes::_identifier){
                std::cerr<<"Expected Identifier"<<std::endl;
                exit(EXIT_FAILURE);
            }

            Token identifier = consume().value();

            if(!peek().has_value() or peek().value().type != TokenTypes::_equals){
                return {};
            }

            consume();

            std::optional<ParseNodes::Expr> expression = parse_expr();

            if(!expression.has_value()){
                std::cerr<<"noexpr"<<std::endl;
                return {};
            }

            letStatement.var = ParseNodes::StmtLet {.identifier = identifier, .expression = expression.value()};

            if(!peek().has_value() or peek().value().type != TokenTypes::_semicolon){
                return {};
            }

            consume();

            return letStatement;
        }

        return {};
    }

    std::optional<ParseNodes::Prog> parse_prog(){
        ParseNodes::Prog progNode;
        while(peek().has_value()){
            auto statement = parse_stmt();
            if(!statement.has_value()){
                std::cerr<<"Syntax Error Not a Statement!" << std::endl;
                exit(EXIT_FAILURE);
            }

            progNode.statements.push_back(statement.value());
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
