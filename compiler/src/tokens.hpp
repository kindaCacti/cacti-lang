#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <set>

std::set<char> special_chars = {';', ':', '(', ')', '=', '+', '*', '-', '/', '!', '<', '>', '{', '}'};

enum TokenTypes{
    _semicolon,
    _colon,
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
    _division,
    _if,
    _excl,          // exclamation point
    _lt,            // less than sign <
    _gt,            // greater than sign >
    _cmp_eq,        // comparator equal
    _cmp_neq,       // comparator not equal
    _cmp_gt,        // greater than
    _cmp_lt,        // less than
    _cmp_leq,       // less or equal
    _cmp_geq,       // greater or equal
    _open_block,    // basically {
    _close_block,   // basically }
    _goto,
};

std::unordered_map<TokenTypes, int> operator_weights = {
    {TokenTypes::_addition, 2},
    {TokenTypes::_subtraction, 2},
    {TokenTypes::_multiplication, 1},
    {TokenTypes::_division, 1}
};

std::set<TokenTypes> comparators = {
    TokenTypes::_cmp_eq,
    TokenTypes::_cmp_neq,
    TokenTypes::_cmp_geq,
    TokenTypes::_cmp_gt,
    TokenTypes::_cmp_leq,
    TokenTypes::_cmp_lt
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
    {"let",   TokenTypes::_let},
    {"if",   TokenTypes::_if},
    {"goto",   TokenTypes::_goto}
};