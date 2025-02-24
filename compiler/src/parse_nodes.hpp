#pragma once

#include <variant>
#include <vector>
#include <memory>
#include <string>
#include "tokens.hpp"

// just for the sake of being clean I guess
namespace ParseNodes{
    struct Expr;
    struct StmtBlck;
    struct StmtIf;
    struct StmtWhile;

    struct BinSign{
        Token sign;
    };

    struct ExprBin{
        std::shared_ptr<Expr> left, right;
        std::shared_ptr<BinSign> sign;
    };

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

    struct StmtAssign{
        Token identifier;
        std::shared_ptr<Expr> expression;
    };
    
    struct StmtLabel{
        Token identifier;
    };

    struct StmtGoto{
        Token identifier;
    };
    
    struct Stmt{
        std::variant<std::shared_ptr<StmtExit>, 
        std::shared_ptr<StmtLet>,
        std::shared_ptr<StmtBlck>,
        std::shared_ptr<StmtIf>,
                     std::shared_ptr<StmtAssign>,
                     std::shared_ptr<StmtLabel>,
                     std::shared_ptr<StmtGoto>,
                     std::shared_ptr<StmtWhile>> var;
    };

    struct StmtBlck{
        std::vector<std::shared_ptr<Stmt>> statements;
    };
    
    struct StmtIf{
        std::shared_ptr<ExprBin> expression;
        std::shared_ptr<Stmt> statement;
    };

    struct StmtWhile{
        std::shared_ptr<ExprBin> expression;
        std::shared_ptr<Stmt> statement;
    };
    
    struct Prog{
        std::vector<std::shared_ptr<Stmt>> statements;
    };
};
