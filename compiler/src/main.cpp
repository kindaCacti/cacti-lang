#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

    std::cout<<data;

    return EXIT_SUCCESS;
}