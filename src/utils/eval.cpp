/*
 Copyright Disney Enterprises, Inc.  All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License
 and the following modification to it: Section 6 Trademarks.
 deleted and replaced with:

 6. Trademarks. This License does not grant permission to use the
 trade names, trademarks, service marks, or product names of the
 Licensor and its affiliates, except as required for reproducing
 the content of the NOTICE file.

 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
*/

#include <vector>
#include <iostream>
#include <cmath>
#include <typeinfo>
#include <SeExpr2/ExprNode.h>
#include <SeExpr2/ExprWalker.h>
#include <SeExpr2/Expression.h>
using namespace SeExpr2;

class Expr : public Expression {
  public:
    void walk()
    {
    }

    struct Var : public ExprVarRef {
        Var() : ExprVarRef(ExprType().Varying().FP(3))
        {
        }
        double val;
        void eval(double* result)
        {
            for (int k = 0; k < 3; k++)
                result[k] = val;
        }
        void eval(const char**)
        {
            assert(false);
        }
    };
    mutable Var X;

    struct VarStr : public ExprVarRef {
        VarStr() : ExprVarRef(ExprType().Varying().String())
        {
        }
        void eval(double*)
        {
            assert(false);
        }

        void eval(const char** resultStr)
        {
            resultStr[0] = "testo";
        }
    };
    mutable VarStr s;

    ExprVarRef* resolveVar(const std::string& name) const
    {
        std::cerr << "trying to resolve " << name << std::endl;
        if (name == "X")
            return &X;
        if (name == "s")
            return &s;
        return 0;
    }
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "usage: eval <expression>" << std::endl;
        exit(1);
    }

    std::cerr << "fun fun" << std::endl;
    Expr expr;
    expr.setExpr(argv[1]);
    if (!expr.isValid()) {
        std::cerr << "parse error " << expr.parseError() << std::endl;
    } else {
        // pre eval
        std::cerr << "pre eval interp" << std::endl;
        expr.evaluator()->dump();
        std::cerr << "starting eval interp" << std::endl;

        // expr._interpreter->print();
        double sum = 0;
        //        for(int i=0;i<200000000;i++){
        for (int i = 0; i < 5; i++) {
            std::cerr << "eval iter " << i << " ";
            expr.X.val = (double)i;
            const double* d = expr.evalFP();
            for (int k = 0; k < expr.returnType().dim(); k++)
                std::cerr << d[k] << " ";
            std::cerr << std::endl;
            sum += d[0];
        }
        std::cerr << "sum " << sum << std::endl;
        expr.evaluator()->dump();
    }

    return 0;
}
