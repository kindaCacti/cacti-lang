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
        TokenTypes next_token = peek().value().type;

        if(next_token == TokenTypes::_integer_literal){
            std::shared_ptr<ParseNodes::ExprIntLit> int_lit(new ParseNodes::ExprIntLit);
            int_lit->int_lit = consume();
            arit->var = int_lit;
            return arit;
        }

        if(next_token == TokenTypes::_identifier){
            std::shared_ptr<ParseNodes::ExprIdent> ident(new ParseNodes::ExprIdent);
            ident->identifier = consume();
            arit->var = ident;
            return arit;
        }

        if(operator_weights.find(next_token) != operator_weights.end()){
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
        TokenTypes next_token = peek().value().type;
        
        if(operator_weights.find(peek(1).value().type) != operator_weights.end() and chk_mul){

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

    std::optional<std::shared_ptr<ParseNodes::BinSign>> parse_binsign(){
        if(!peek(1).has_value()){
            std::cerr << "Invalid binary sign" << std::endl;
            exit(EXIT_FAILURE);
        }

        if(peek().value().type == TokenTypes::_equals and
        peek(1).value().type == TokenTypes::_equals){
            consume(2);
            ParseNodes::BinSign tmp{.sign={.type = TokenTypes::_cmp_eq}};
            return std::shared_ptr<ParseNodes::BinSign>(new ParseNodes::BinSign(tmp));
        }

        if(peek().value().type == TokenTypes::_excl and
        peek(1).value().type == TokenTypes::_equals){
            consume(2);
            ParseNodes::BinSign tmp{.sign={.type = TokenTypes::_cmp_neq}};
            return std::shared_ptr<ParseNodes::BinSign>(new ParseNodes::BinSign(tmp));
        }

        if(peek().value().type == TokenTypes::_gt and
        peek(1).value().type == TokenTypes::_gt){
            consume(2);
            ParseNodes::BinSign tmp{.sign={.type = TokenTypes::_cmp_gt}};
            return std::shared_ptr<ParseNodes::BinSign>(new ParseNodes::BinSign(tmp));
        }

        if(peek().value().type == TokenTypes::_gt and
        peek(1).value().type == TokenTypes::_equals){
            consume(2);
            ParseNodes::BinSign tmp{.sign={.type = TokenTypes::_cmp_geq}};
            return std::shared_ptr<ParseNodes::BinSign>(new ParseNodes::BinSign(tmp));
        }

        if(peek().value().type == TokenTypes::_lt and
        peek(1).value().type == TokenTypes::_lt){
            consume(2);
            ParseNodes::BinSign tmp{.sign={.type = TokenTypes::_cmp_lt}};
            return std::shared_ptr<ParseNodes::BinSign>(new ParseNodes::BinSign(tmp));
        }

        if(peek().value().type == TokenTypes::_lt and
        peek(1).value().type == TokenTypes::_equals){
            consume(2);
            ParseNodes::BinSign tmp{.sign={.type = TokenTypes::_cmp_leq}};
            return std::shared_ptr<ParseNodes::BinSign>(new ParseNodes::BinSign(tmp));
        }
        return {};
    }

    std::optional<std::shared_ptr<ParseNodes::StmtBlck>> parse_stmtblck(){
        std::shared_ptr<ParseNodes::StmtBlck> out(new ParseNodes::StmtBlck);
        
        while(peek().has_value() and peek().value().type != TokenTypes::_close_block){
            auto parsedStmt = parse_stmt();
            if(!parsedStmt.has_value()){
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }

            out->statements.push_back(parsedStmt.value());
        }

        if(!peek().has_value()){
            return {};
        }

        return out;
    }

    std::optional<std::shared_ptr<ParseNodes::ExprBin>> parse_exprbin(){
        auto leftExpr = parse_expr();
        if(!leftExpr.has_value()){
            std::cerr << "invalid left expression in if statement" << std::endl;
            exit(EXIT_FAILURE);
        }

        auto binSign = parse_binsign();
        if(!binSign.has_value()){
            std::cerr << "invalid comparisong sign" << std::endl;
            exit(EXIT_FAILURE);
        }

        auto rightExpr = parse_expr();
        if(!rightExpr.has_value()){
            std::cerr << "invalid right expression in if statement" << std::endl;
            exit(EXIT_FAILURE);
        }

        ParseNodes::ExprBin binExpr{.left = leftExpr.value(), 
        .right = rightExpr.value(), .sign = binSign.value()};
        std::shared_ptr<ParseNodes::ExprBin> out(new ParseNodes::ExprBin(binExpr));
        return out;
    }
    
    std::optional<std::shared_ptr<ParseNodes::Stmt>> parse_stmt(){
        if(!peek().has_value()){
            return {};
        }

        TokenTypes firstToken = peek().value().type;

        if(firstToken == TokenTypes::_exit){
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

        if(firstToken == TokenTypes::_let){
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

        if(firstToken == TokenTypes::_open_block){
            consume();
            auto parsed_stmtblck = parse_stmtblck();

            if(!parsed_stmtblck.has_value()){
                std::cerr << "Invalid statement block" << std::endl;
                exit(EXIT_FAILURE);
            }

            if(!peek().has_value()){
                std::cerr << "statement block is not closed" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::shared_ptr<ParseNodes::Stmt> statement(new ParseNodes::Stmt);
            statement->var = parsed_stmtblck.value();
            consume();

            return statement;
        }
        
        if(firstToken == TokenTypes::_if){
            consume();
            
            if(!peek().has_value() or 
            peek().value().type != TokenTypes::_open_parentheses){
                std::cerr << "Invalid if statement" << std::endl;
                exit(EXIT_FAILURE);
            }
            consume();

            auto parsedIf = parse_exprbin();
            if(!parsedIf.has_value()){
                std::cerr << "Invalid expression in if statement" << std::endl;
                exit(EXIT_FAILURE);
            }
            consume();

            auto parsedStatement = parse_stmt();
            if(!parsedStatement.has_value()){
                std::cerr << "No statement in if" << std::endl;
                exit(EXIT_FAILURE);
            }

            ParseNodes::StmtIf ifstm{.expression = parsedIf.value(), 
            .statement = parsedStatement.value()};
            ParseNodes::Stmt tmp{.var = std::shared_ptr<ParseNodes::StmtIf>(new ParseNodes::StmtIf(ifstm))};
            std::shared_ptr<ParseNodes::Stmt> out(new ParseNodes::Stmt(tmp));
            return out;
        }

        if(peek(1).has_value() and firstToken == TokenTypes::_identifier and 
        peek(1).value().type == TokenTypes::_colon){
            ParseNodes::StmtLabel tmp{.identifier=consume()};
            consume();
            ParseNodes::Stmt tmpStmt{.var=std::shared_ptr<ParseNodes::StmtLabel>(new ParseNodes::StmtLabel(tmp))};
            return std::shared_ptr<ParseNodes::Stmt>(new ParseNodes::Stmt(tmpStmt));
        }

        if(firstToken == TokenTypes::_identifier){
            Token ident = consume();
            if(!peek().has_value() or peek().value().type != TokenTypes::_equals){
                std::cerr << "Invalid variable assignment" << std::endl;
                exit(EXIT_FAILURE);
            }
            consume();

            auto parsed_expression = parse_expr();
            if(!parsed_expression.has_value()){
                std::cerr << "invalid expression in variable assignment" << std::endl;
                exit(EXIT_FAILURE);
            }
            if(!peek().has_value()  or peek().value().type != TokenTypes::_semicolon){
                std::cerr << "expected a ';'" << std::endl;
                exit(EXIT_FAILURE);
            } 
            consume();

            ParseNodes::StmtAssign expras{.identifier=ident, .expression=parsed_expression.value()};
            std::shared_ptr<ParseNodes::StmtAssign> tmp(new ParseNodes::StmtAssign(expras));
            ParseNodes::Stmt out{.var = tmp};
            return std::shared_ptr<ParseNodes::Stmt>(new ParseNodes::Stmt(out));
        }

        if(firstToken == TokenTypes::_goto){
            consume();

            if(!peek(1).has_value() or peek().value().type != TokenTypes::_identifier
            or peek(1).value().type != TokenTypes::_semicolon){
                std::cerr << "invalid goto statement!" << std::endl;
                exit(EXIT_FAILURE);
            }

            Token identifier = consume();
            consume();

            ParseNodes::StmtGoto gotostmt{.identifier=identifier};
            std::shared_ptr<ParseNodes::StmtGoto> tmp(new ParseNodes::StmtGoto(gotostmt));
            ParseNodes::Stmt out{.var=tmp};
            return std::shared_ptr<ParseNodes::Stmt>(new ParseNodes::Stmt(out));
        }
        
        if(firstToken == TokenTypes::_while){
            consume();
            if(peek().value().type != TokenTypes::_open_parentheses){
                std::cerr << "Expected '(' in while statement" << std::endl;
                exit(EXIT_FAILURE);
            }
            consume();
            auto parsedExprbin = parse_exprbin();
            if(!parsedExprbin.has_value()){
                std::cerr << "Invalid binary expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            if(peek().value().type != TokenTypes::_close_parentheses){
                std::cerr << "Expected ')' in while statement" << std::endl;
                exit(EXIT_FAILURE);
            }
            consume();
            auto parsedStatement = parse_stmt();

            if(!parsedStatement.has_value()){
                std::cerr << "Invalid statement expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            ParseNodes::StmtWhile whileStmt{.expression = parsedExprbin.value(), .statement = parsedStatement.value()};
            ParseNodes::Stmt stmt{.var = std::shared_ptr<ParseNodes::StmtWhile>(new ParseNodes::StmtWhile(whileStmt))};
            std::shared_ptr<ParseNodes::Stmt> out = std::shared_ptr<ParseNodes::Stmt>(new ParseNodes::Stmt(stmt));
            return out;
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
