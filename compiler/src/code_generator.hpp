#pragma once

#include <sstream>
#include <string>
#include <optional>
#include <iostream>
#include "parse_nodes.hpp"

class CodeGenerator{
    ParseNodes::Exit tree;
public:
    CodeGenerator(ParseNodes::Exit root):
        tree(root) {}

    std::stringstream generate(){
        std::stringstream out;
        out << "global _start\n";
        out << "_start:\n";
        out << "   mov rax, 60\n";
        std::cout<<out.str()<<std::endl;
        out << "   mov rdi, " << tree.expression.int_lit.data.value() << "\n";
        std::cout<<out.str()<<std::endl;
        out << "   syscall\n";

        return out;
    }
};