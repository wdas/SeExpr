/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/

#include <SeExpr2/Expression.h>
#include <SeExpr2/ExprFunc.h>
#include <SeExpr2/Vec.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define STACK_DEPTH 256

using namespace SeExpr2;

/**
   @file asciiCalculator.cpp
*/
//! Simple expression class to support our function calculator
class CalculatorExpr : public Expression {
  public:
    //! Constructor that takes the expression to parse
    CalculatorExpr(const std::string& expr) : Expression(expr), _count(0) {
        for (int i = 0; i < STACK_DEPTH; i++) {
            stack[i].val = Vec<double, 3, false>(0.0);
            fail_stack[i] = false;
        }
    };

    //! Empty constructor
    CalculatorExpr() : Expression(), _count(0) {
        for (int i = 0; i < STACK_DEPTH; i++) fail_stack[i] = false;
    };

    //! Push current result on stack
    void push() {
        if (returnType().isString()) {
            evalStr();
        } else if (returnType().isFP()) {
            const double* val = evalFP();
            int dim = returnType().dim();
            for (int k = 0; k < 3; k++) std::cerr << val[k] << " ";
            std::cerr << std::endl;
            if (dim == 1)
                stack[_count].val = Vec<double, 3, false>(val[0]);
            else if (dim == 2)
                stack[_count].val = Vec<double, 3, false>(val[0], val[1], 0);
            else if (dim == 3)
                stack[_count].val = Vec<double, 3, true>(const_cast<double*>(&val[0]));
            else {
                std::cerr << "Return type FP(" << dim << ") ignoring" << std::endl;
            }

            _count++;
        }
    };

    //! Failed attempt; push 0 on stack
    void fail_push() {
        fail_stack[_count] = true;
        stack[_count].val = Vec<double, 3, false>(0.0);
        _count++;
    };

    Vec<double, 3, false> peek() { return stack[_count - 1].val; }

    int count() const {
        return _count;
    };

  private:
    //! Simple variable that just returns its internal value
    struct SimpleVar : public ExprVarRef {
        SimpleVar() : ExprVarRef(ExprType().FP(3).Varying()), val(0.0) {}

        Vec<double, 3, false> val;  // independent variable

        void eval(double* result) {
            for (int k = 0; k < 3; k++) result[k] = val[k];
        }

        void eval(const char** result) {}
    };

    //! previous computations
    mutable SimpleVar stack[STACK_DEPTH];
    mutable bool fail_stack[STACK_DEPTH];
    mutable int _count;

    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string& name) const {
        if (name[0] == '_') {
            int position = atoi(name.substr(1, name.size() - 1).c_str());
            if (position >= count()) std::cerr << "Use of unused result line." << std::endl;
            if (fail_stack[position]) std::cerr << "Use of invalid result line." << std::endl;
            return &(stack[position]);
        };
        addError("Use of undefined variable.", 0, 0);
        return 0;
    };
};

int main(int argc, char* argv[]) {

    std::cout << "SeExpr Basic Calculator";

    CalculatorExpr expr;
    while (true) {
        std::string str;
        std::cout << std::endl << expr.count() << "> ";
        // std::cin >> str;
        getline(std::cin, str);

        if (std::cin.eof()) {
            std::cout << std::endl;
            str = "q";
        };

        if (str == "quit" || str == "q") break;
        expr.setDesiredReturnType(ExprType().FP(3));
        expr.setExpr(str);

        if (!expr.isValid()) {
            expr.fail_push();
            std::cerr << "Expression failed: " << expr.parseError() << std::endl;
        } else {
            expr.push();
            std::cout << "   " << expr.peek();
        }
    }
    ExprFunc::cleanup();
    return 0;
}
