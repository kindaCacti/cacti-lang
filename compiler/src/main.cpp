#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "tokenizer.h"

int main(int argc, char* argv[]){
    if(argc == 1){
        std::cerr<<"no files to compile"<<std::endl;
        return EXIT_FAILURE;
    }

    std::string data;
    {
        std::stringstream stream_data;
        std::ifstream fin(argv[1]);
        stream_data << fin.rdbuf();
        fin.close();
        data = stream_data.str();
    }

    std::vector<Token> tokens;

    Tokenizer tokenizer;
    std::string buffer;

    for(char c : data){
        // TODO: change to checking for one of special caracters
        if(c == ' ' or c == '\n' or c == ';'){
            tokens.push_back(tokenizer.generateToken(buffer));
            buffer.clear();
        }else{
            buffer.push_back(c);
        }

        if(c == ';'){
            std::string semString(";");
            tokens.push_back(tokenizer.generateToken(semString));
        }
    }

    for(auto& tok : tokens){
        if(tok.type == TokenTypes::_return) std::cout<<"return "<<std::endl;
        if(tok.type == TokenTypes::_integer_literal) std::cout<<"int_lit "<<std::endl;
        if(tok.type == TokenTypes::_semicolon) std::cout<<"; "<<std::endl;
    }

    return EXIT_SUCCESS;
}