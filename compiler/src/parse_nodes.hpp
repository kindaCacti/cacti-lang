#pragma once

#include <variant>
#include <vector>
#include <memory>
#include <string>
#include "tokens.hpp"

// just for the sake of being clean I guess
namespace ParseNodes{
    struct Expr;

    struct ExprIntLit{
        Token int_lit;
    };

    struct ExprIdent{
        Token identifier;
    };

    struct ExprSgn{
        Token oper;
    };

    struct ExprArit{
        std::variant<std::shared_ptr<ExprIntLit>, 
                     std::shared_ptr<ExprIdent>,
                     std::shared_ptr<ExprSgn>> var;
    };

    // Maybe this should only have a list of variant<
    struct ExprOper{
        //std::shared_ptr<Expr> left, right;
        //Token oper;
        std::vector<std::shared_ptr<ExprArit>> arit;
    };

    struct Expr{
        std::variant<std::shared_ptr<ExprIntLit>, 
                     std::shared_ptr<ExprIdent>,
                     std::shared_ptr<ExprOper>> var;
    };

    struct StmtExit{
        std::shared_ptr<Expr> expression;
    };

    struct StmtLet{
        Token identifier;
        std::shared_ptr<Expr> expression;
    };

    struct Stmt{
        std::variant<std::shared_ptr<StmtExit>, 
                     std::shared_ptr<StmtLet>> var;
    };

    struct Prog{
        std::vector<std::shared_ptr<Stmt>> statements;
    };
};