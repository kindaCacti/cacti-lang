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

    
    std::optional<ParseNodes::Exit> parse(){
        std::optional<ParseNodes::Exit> exitNode;

        while(peek().has_value()){
            std::cout<<"here1"<<std::endl;
            Token currentToken = consume().value();
            if(currentToken.type == TokenTypes::_exit){
                if(!peek().has_value()){
                    std::cerr<<"Invalid Syntax\n";
                    exit(1);
                }

                Token int_lit = consume().value();
                Token semi = consume().value();

                if(int_lit.type == TokenTypes::_integer_literal and
                semi.type == TokenTypes::_semicolon){
                    ParseNodes::Exit tmp;
                    tmp.expression.int_lit = int_lit;
                    exitNode = tmp;
                }
            }
        }

        return exitNode;
    }
private:
    std::vector<Token> tokens;
    size_t t_index;

    std::optional<Token> peek(int ahead = 1){
        if(t_index + ahead >= tokens.size()){
            return {};
        }

        return tokens[t_index];
    }

    std::optional<Token> consume(){
        if(t_index >= tokens.size()){
            return {};
        }

        return tokens[t_index++]; //hopefully this works
    }
};
