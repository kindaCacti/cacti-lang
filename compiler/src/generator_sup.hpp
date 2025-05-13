#pragma once

#include <string>
#include "tokens.hpp"
#include <unordered_map>
#include <string>
#include <sstream>
#include <variant>

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

enum Regs{
    rax, rbx, rdi, rcx, rsp
};

std::unordered_map<Regs, std::string> register_name = {
    {Regs::rax, "rax"},
    {Regs::rbx, "rbx"},
    {Regs::rdi, "rdi"},
    {Regs::rcx, "rcx"},
    {Regs::rsp, "rsp"},
};

struct ASMNode{
    Instructions instruction;
    std::vector<std::variant<Regs, std::string>> data;
};
