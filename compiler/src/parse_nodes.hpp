#pragma once

#include "tokens.hpp"

// just for the sake of being clean I guess
namespace ParseNodes{
    struct Expr{
        Token int_lit; // just for now because we don't have any math
    };

    struct Exit{
        Expr expression;
    };
};