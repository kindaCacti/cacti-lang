#pragma once

#include <string>
#include "tokens.hpp"

struct RPNNode{
    TokenTypes type;
    std::string value;
};