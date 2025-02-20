#pragma once
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
#include "tokens.hpp"

class Trie{
    struct Node{
        std::map<char, int> children;
        std::optional<TokenTypes> token;
    };

    std::vector<Node> nodes;
public:
    Trie(){
        nodes.push_back(Node());
    }

    void addToken(std::string& tokenString, TokenTypes token){
        int currentNode = 0;
        for(char c : tokenString){
            if(nodes[currentNode].children.find(c) == nodes[currentNode].children.end()){
                nodes.push_back(Node());
                nodes[currentNode].children.insert({c, nodes.size() - 1});
            }
            currentNode = nodes[currentNode].children[c];
        }
        nodes[currentNode].token = token;
    }

    std::optional<TokenTypes> getToken(std::string& tokenString){
        int currentNode = 0;
        for(char c : tokenString){
            if(nodes[currentNode].children.find(c) == nodes[currentNode].children.end()){
                return {};
            }
            currentNode = nodes[currentNode].children[c];
        }

        if(!nodes[currentNode].token.has_value()){
            return {};
        }

        return nodes[currentNode].token;
    }
};

class Tokenizer : public Trie{
public:
    void loadTokens(){
        for(auto tokenData : tokenStrings){
            addToken(tokenData.tokenString, tokenData.tokenType);
        }
    }

    Tokenizer(): Trie(){
        loadTokens();
    }

    std::optional<TokenTypes> checkAgainstSpecialChars(char data){
        // I hate switch case so I'm using if-s xD
        // Maybe this should return the whole token
        if(data == ';') return TokenTypes::_semicolon;
        if(data == ':') return TokenTypes::_colon;
        if(data == '(') return TokenTypes::_open_parentheses;
        if(data == ')') return TokenTypes::_close_parentheses;
        if(data == '=') return TokenTypes::_equals;
        if(data == '+') return TokenTypes::_addition;
        if(data == '-') return TokenTypes::_subtraction;
        if(data == '*') return TokenTypes::_multiplication;
        if(data == '/') return TokenTypes::_division;
        if(data == '!') return TokenTypes::_excl;
        if(data == '<') return TokenTypes::_lt;
        if(data == '>') return TokenTypes::_gt;
        if(data == '{') return TokenTypes::_open_block;
        if(data == '}') return TokenTypes::_close_block;
        return {};
    }

    std::optional<TokenTypes> checkForLiterals(std::string& data){
        for(char c : data){
            if(c < '0' or c > '9') return {};
        }
        return TokenTypes::_integer_literal;
    }

    Token generateToken(std::string& data){
        Token token;

        // checking for semicolons and such
        if(data.size() == 1){
            std::optional<TokenTypes> tokenType = checkAgainstSpecialChars(data[0]);
            if(tokenType.has_value()){
                token.type = tokenType.value();
                token.data = std::string(data);
                return token;
            }
        }

        // checking for special words
        std::optional<TokenTypes> tokenType = getToken(data);

        if(tokenType.has_value()){
            token.type = tokenType.value();
            return token;
        }

        tokenType = checkForLiterals(data);

        if(tokenType.has_value()){
            token.type = tokenType.value();
            token.data = data;
            return token;
        }

        return Token {.type = _identifier, .data=std::string(data)};
    }
};