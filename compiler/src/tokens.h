#pragma once
#include <string>
#include <vector>

enum TokenTypes{
    _semicolon,
    _exit,
    _integer_literal,
    _string_literal
};

struct TokenData{
    std::string tokenString;
    TokenTypes tokenType;
};

const std::vector<TokenData> tokenStrings = {
    {"exit",  TokenTypes::_exit}
};