#pragma once
#include <string>
#include <vector>

enum TokenTypes{
    _semicolon,
    _return,
    _integer_literal,
    _string_literal
};

struct TokenData{
    std::string tokenString;
    TokenTypes tokenType;
};

const std::vector<TokenData> tokenStrings = {
    {"return",  TokenTypes::_return}
};