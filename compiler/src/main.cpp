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

    // just to test out the basic functionality
    std::stringstream out;
    out << "global _start\n   _start:\n";

    if(tokens[0].type == TokenTypes::_return and tokens[1].type == TokenTypes::_integer_literal and tokens[2].type == TokenTypes::_semicolon){
        out << "   mov rax, 60\n";
        out << "   mov rdi, " << tokens[1].data.value() << "\n";
        out << "   syscall";
    }

    std::cout<<out.str();

    return EXIT_SUCCESS;
}