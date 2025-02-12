#include <string>
#include <vector>

enum TokenTypes{
    _semicolon,
    _return,
    _integer_literal
};

struct TokenData{
    std::string tokenString;
    TokenTypes tokenType;
};

const std::vector<TokenData> tokenStrings = {
    {";",       TokenTypes::_semicolon},
    {"return",  TokenTypes::_return}
};