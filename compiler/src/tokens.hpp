#pragma once
#include <string>
#include <vector>
#include <optional>
#include <set>

std::set<char> special_chars = {';', '(', ')', '=', '+'};

enum TokenTypes{
    _semicolon,
    _exit,
    _integer_literal,
    _open_parentheses,
    _close_parentheses,
    _identifier,
    _let,
    _equals,
    _addition
};

struct Token{
    TokenTypes type;
    std::optional<std::string> data;
};

struct TokenData{
    std::string tokenString;
    TokenTypes tokenType;
};

const std::vector<TokenData> tokenStrings = {
    {"exit",  TokenTypes::_exit},
    {"let",   TokenTypes::_let}
};