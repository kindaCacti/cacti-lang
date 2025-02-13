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

    Parser parser(std::move(tokens));
    std::optional<ParseNodes::Exit> tree = parser.parse();

    if(!tree.has_value()){
        std::cerr<<"INVALID SYNTAX\n";
        exit(EXIT_FAILURE);
    }

    CodeGenerator generator(tree.value());
    std::cout << generator.generate().str();

    return EXIT_SUCCESS;
}