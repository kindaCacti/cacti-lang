#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "tokenizer.hpp"
#include "parser.hpp"
#include "code_generator.hpp"

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
        auto specialCharsIt = special_chars.find(c);
        if(c == ' ' or c == '\n' or specialCharsIt != special_chars.end()){
            if(buffer.size()){
                tokens.push_back(tokenizer.generateToken(buffer));
                buffer.clear();
            }
        }else{
            buffer.push_back(c);
        }

        if(specialCharsIt != special_chars.end()){
            std::string tokenString;
            tokenString.push_back(c);
            tokens.push_back(tokenizer.generateToken(tokenString));
        }
    }

    Parser parser(std::move(tokens));
    std::optional<ParseNodes::Prog> tree = parser.parse_prog();

    if(!tree.has_value()){
        std::cerr<<"INVALID SYNTAX\n";
        exit(EXIT_FAILURE);
    }

    CodeGenerator generator(tree.value());
    std::cout << generator.generate_program();
    return EXIT_SUCCESS;
}