#pragma once
#include <string>
#include <vector>
#include <optional>

enum TokenTypes{
    _semicolon,
    _exit,
    _integer_literal,
    _string_literal
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
    {"exit",  TokenTypes::_exit}
};