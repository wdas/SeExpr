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

#ifndef TYPEBUILDER_H
#define TYPEBUILDER_H

#include <Expression.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>

#include "ExprNode.h"
#include "ExprFunc.h"

using namespace SeExpr2;

/**
   @file TypeBuilder.h
*/
//! Simple expression class to build all intermediate types
class TypeBuilderExpr : public Expression {
  public:
    struct DummyFuncX : ExprFuncX {
        DummyFuncX() : ExprFuncX(false) {};

        virtual bool isScalar() const {
            return true;
        };
        virtual ExprType retType() const {
            return ExprType().FP(1).Varying();
        };
        // TODO: fix -- just a no-op function to get code to compile.
        virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnv& env) const {
            return ExprType().None();
        };

        // TODO: fix -- just a no-op function to get code to compile.
        virtual int buildInterpreter(const ExprFuncNode* node, Interpreter* interpreter) const {
            return 0;
        };

        void eval(const ExprFuncNode* node, Vec3d& result) const { result = Vec3d(); }
    } dummyFuncX;
    mutable ExprFunc dummyFunc;
    mutable ExprFunc func;

    class ExprNothingVarRef : public ExprVarRef {
      public:
        ExprNothingVarRef(const ExprType& type) : ExprVarRef::ExprVarRef(type) {};

        void eval(const ExprVarNode* node, Vec3d& result) {
            result = 0.0;
        };

        // TODO: fix -- just a no-op function to get code to compile.
        virtual void eval(double* result) {};

        // TODO: fix -- just a no-op function to get code to compile.
        virtual void eval(const char** resultStr) {};
    };

    //! Empty constructor
    TypeBuilderExpr()
        : Expression::Expression(), dummyFunc(dummyFuncX, 0, 16), func(dummyFuncX, 3, 3),
          F1(ExprType().FP(1).Varying()), F2(ExprType().FP(2).Varying()), F3(ExprType().FP(3).Varying()),
          ST(ExprType().String().Varying()), SE(ExprType().Error().Varying()), LC(ExprType().FP(1).Constant()),
          LU(ExprType().FP(1).Uniform()), LV(ExprType().FP(1).Varying()), LE(ExprType().FP(1).Error()),
          v(ExprType().FP(1).Varying()), x(ExprType().FP(1).Varying()), y(ExprType().FP(1).Varying()),
          z(ExprType().FP(1).Varying()) {};

    TypeBuilderExpr(const std::string& e)
        : Expression::Expression(e), dummyFunc(dummyFuncX, 0, 16), func(dummyFuncX, 3, 3),
          F1(ExprType().FP(1).Varying()), F2(ExprType().FP(2).Varying()), F3(ExprType().FP(3).Varying()),
          ST(ExprType().String().Varying()), SE(ExprType().Error().Varying()), LC(ExprType().FP(1).Constant()),
          LU(ExprType().FP(1).Uniform()), LV(ExprType().FP(1).Varying()), LE(ExprType().FP(1).Error()),
          v(ExprType().FP(1).Varying()), x(ExprType().FP(1).Varying()), y(ExprType().FP(1).Varying()),
          z(ExprType().FP(1).Varying()) {};

    //! resolve function
    ExprVarRef* resolveVar(const std::string& name) const {
        if (name == "F1") return &F1;
        if (name == "F2") return &F2;
        if (name == "F3") return &F3;
        if (name == "ST") return &ST;
        if (name == "SE") return &SE;
        if (name == "LC") return &LC;
        if (name == "LU") return &LU;
        if (name == "LV") return &LV;
        if (name == "LE") return &LE;
        if (name == "v") return &v;
        if (name == "x") return &x;
        if (name == "y") return &y;
        if (name == "z") return &z;

        /*else*/
        return 0;
    };

    ExprFunc* resolveFunc(const std::string& name) const {
        if (name == "func")
            return &func;
        else
            return 0;
        /* else             return &dummyFunc; */
    };

    void setVar(const std::string& to, const std::string& from) {
        if (to == "v") v = ExprNothingVarRef(resolveVar(from)->type());
        if (to == "x") x = ExprNothingVarRef(resolveVar(from)->type());
        if (to == "y") y = ExprNothingVarRef(resolveVar(from)->type());
        if (to == "z") z = ExprNothingVarRef(resolveVar(from)->type());
        // TODO: rem ove
        // std::cerr<<"v is "<<v.type().toString()<<" from "<<from<<" is
        // "<<(resolveVar(from)->type().toString())<<std::endl;
    };

  private:
    // Various types
    mutable ExprNothingVarRef F1;
    mutable ExprNothingVarRef F2;
    mutable ExprNothingVarRef F3;
    mutable ExprNothingVarRef ST;
    mutable ExprNothingVarRef SE;
    mutable ExprNothingVarRef LC;
    mutable ExprNothingVarRef LU;
    mutable ExprNothingVarRef LV;
    mutable ExprNothingVarRef LE;

    // Use variables
    mutable ExprNothingVarRef v;
    mutable ExprNothingVarRef x;
    mutable ExprNothingVarRef y;
    mutable ExprNothingVarRef z;
};

#endif  // TYPEBUILDER_H
