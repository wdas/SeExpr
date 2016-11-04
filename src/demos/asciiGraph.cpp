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
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace SeExpr2;

/**
   @file asciiGraph.cpp
*/
//! Simple expression class to support our function grapher
class GrapherExpr : public Expression {
  public:
    //! Constructor that takes the expression to parse
    GrapherExpr(const std::string& expr) : Expression(expr) {}

    //! set the independent variable
    void setX(double x_input) { x.val = x_input; }

  private:
    //! Simple variable that just returns its internal value
    struct SimpleVar : public ExprVarRef {
        SimpleVar() : ExprVarRef(ExprType().FP(1).Varying()), val(0.0) {}

        double val;  // independent variable
        void eval(double* result) { result[0] = val; }

        void eval(const char** result) {}
    };

    //! independent variable
    mutable SimpleVar x;

    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string& name) const {
        if (name == "x") return &x;
        return 0;
    }
};

int main(int argc, char* argv[]) {
    std::string exprStr =
        "\
                         $val=.5*PI*x;\
                         7*sin(val)/val \
                         ";
    if (argc == 2) {
        exprStr = argv[1];
    }
    GrapherExpr expr(exprStr);

    if (!expr.isValid()) {
        std::cerr << "expression failed " << expr.parseError() << std::endl;
        exit(1);
    } else if (!expr.returnType().isFP(1)) {
        std::cerr << "Expected expression of type " << ExprType().FP(1).Varying().toString() << " got "
                  << expr.returnType().toString() << std::endl;
        exit(1);
    }

    double xmin = -10, xmax = 10, ymin = -10, ymax = 10;
    int w = 60, h = 30;
    char* buffer = new char[w * h];
    memset(buffer, (int)' ', w * h);

    // draw x axis
    int j_zero = (-ymin) / (ymax - ymin) * h;
    if (j_zero >= 0 && j_zero < h) {
        for (int i = 0; i < w; i++) {
            buffer[i + j_zero * w] = '-';
        }
    }
    // draw y axis
    int i_zero = (-xmin) / (xmax - xmin) * w;
    if (i_zero >= 0 && i_zero < w) {
        for (int j = 0; j < h; j++) {
            buffer[i_zero + j * w] = '|';
        }
    }

    // evaluate the graph
    const int samplesPerPixel = 10;
    const double one_over_samples_per_pixel = 1. / samplesPerPixel;
    for (int i = 0; i < w; i++) {
        for (int sample = 0; sample < samplesPerPixel; sample++) {
            // transform from device to logical coordinatex
            double dx = double(sample) * one_over_samples_per_pixel;
            double x = double(dx + i) / double(w) * (xmax - xmin) + xmin;
            // prep the expression engine for evaluation
            expr.setX(x);
            const double* val = expr.evalFP();
            // evaluate and pull scalar value - currently does not work
            // TODO: fix eval and then use actual call
            // Vec3d val=0.0;//expr.evaluate();
            double y = val[0];
            // transform from logical to device coordinate
            int j = (y - ymin) / (ymax - ymin) * h;
            // store to the buffer
            if (j >= 0 && j < h) buffer[i + j * w] = '#';
        }
    }

    // draw the graph from the buffer
    for (int j = h - 1; j >= 0; j--) {
        for (int i = 0; i < w; i++) {
            std::cout << buffer[i + j * w];
        }
        std::cout << std::endl;
    }

    return 0;
}
