#pragma once

#include <variant>
#include <vector>
#include "tokens.hpp"

// just for the sake of being clean I guess
namespace ParseNodes{
    struct ExprIntLit{
        Token int_lit;
    };

    struct ExprIdent{
        Token identifier;
    };
    
    struct Expr{
        std::variant<ExprIntLit, ExprIdent> var;
    };

    struct StmtExit{
        Expr expression;
    };

    struct StmtLet{
        Token identifier;
        Expr expression;
    };

    struct Stmt{
        std::variant<StmtExit, StmtLet> var;
    };

    struct Prog{
        std::vector<Stmt> statements;
    };
};