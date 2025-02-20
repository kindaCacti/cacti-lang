#pragma once

#include <string>
#include "tokens.hpp"

struct RPNNode{
    TokenTypes type;
    std::string value;
};

struct Var{
    size_t stack_loc;
    std::string identifier;
};

enum Instructions{
    _global,
    _syscall,
    _push,      
    _pop,       
    _mov,       
    _add,       
    _sub,       
    _imul,
    _idiv,
    _cmp,
    _jmp,
    _jne,
    _jnae,
    _jng,
    _jnbe,
    _jnl,
    _je,
    _label
};

struct ASMNode{
    Instructions instruction;
    std::string data;
};