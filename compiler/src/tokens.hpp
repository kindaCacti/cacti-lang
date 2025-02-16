#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <set>

std::set<char> special_chars = {';', '(', ')', '=', '+', '*', '-', '/'};

enum TokenTypes{
    _semicolon,
    _exit,
    _integer_literal,
    _open_parentheses,
    _close_parentheses,
    _identifier,
    _let,
    _equals,
    _addition,
    _subtraction,
    _multiplication,
    _division
};

std::unordered_map<TokenTypes, int> operator_weights = {
    {TokenTypes::_addition, 2},
    {TokenTypes::_subtraction, 2},
    {TokenTypes::_multiplication, 1},
    {TokenTypes::_division, 1}
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