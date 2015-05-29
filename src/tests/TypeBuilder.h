/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
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
class TypeBuilderExpr : public Expression
{
public:
    struct DummyFuncX:ExprFuncX
    {
        DummyFuncX()
            : ExprFuncX(false)
        {};

        virtual bool       isScalar() const { return true; };
        virtual ExprType retType () const { return ExprType().FP(1).Varying(); };
        // TODO: fix -- just a no-op function to get code to compile.
        virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnv & env) const {return ExprType().None();};

        // TODO: fix -- just a no-op function to get code to compile.
        virtual int buildInterpreter(const ExprFuncNode* node,Interpreter* interpreter) const {return 0;};

        void eval(const ExprFuncNode* node,Vec3d& result) const
        {result=Vec3d();}
    } dummyFuncX;
    mutable ExprFunc dummyFunc;
    mutable ExprFunc func;

    class ExprNothingVarRef : public ExprVarRef {
    public:
        ExprNothingVarRef(const ExprType & type)
            : ExprVarRef::ExprVarRef(type)
        {};

        void eval(const ExprVarNode* node, Vec3d& result)
        {
            result = 0.0;
        };

        // TODO: fix -- just a no-op function to get code to compile.
        virtual void eval(double* result) {};

        // TODO: fix -- just a no-op function to get code to compile.
        virtual void eval(const char** resultStr) {};
    };

    //! Empty constructor
    TypeBuilderExpr()
        : Expression::Expression(),
         dummyFunc(dummyFuncX,0,16),
         func(dummyFuncX,3,3),
         F1(ExprType().FP(1).Varying() ),
         F2(ExprType().FP(2).Varying()),
         F3(ExprType().FP(3).Varying()),
         ST(ExprType().String().Varying() ),
         SE(ExprType().Error().Varying() ),
         LC(ExprType().FP(1).Constant() ),
         LU(ExprType().FP(1).Uniform() ),
         LV(ExprType().FP(1).Varying() ),
         LE(ExprType().FP(1).Error() ),
         v(ExprType().FP(1).Varying() ),
         x(ExprType().FP(1).Varying() ),
         y(ExprType().FP(1).Varying() ),
         z(ExprType().FP(1).Varying() )
    {};

    TypeBuilderExpr(const std::string &e)
        : Expression::Expression(e),
         dummyFunc(dummyFuncX,0,16),
         func(dummyFuncX,3,3),
         F1(ExprType().FP(1).Varying() ),
         F2(ExprType().FP(2).Varying()),
         F3(ExprType().FP(3).Varying()),
         ST(ExprType().String().Varying() ),
         SE(ExprType().Error().Varying() ),
         LC(ExprType().FP(1).Constant() ),
         LU(ExprType().FP(1).Uniform() ),
         LV(ExprType().FP(1).Varying() ),
         LE(ExprType().FP(1).Error() ),
         v(ExprType().FP(1).Varying() ),
         x(ExprType().FP(1).Varying() ),
         y(ExprType().FP(1).Varying() ),
         z(ExprType().FP(1).Varying() )
    {};

    //! resolve function
    ExprVarRef* resolveVar(const std::string& name) const {
        if(name=="F1") return &F1;
        if(name=="F2") return &F2;
        if(name=="F3") return &F3;
        if(name=="ST") return &ST;
        if(name=="SE") return &SE;
        if(name=="LC") return &LC;
        if(name=="LU") return &LU;
        if(name=="LV") return &LV;
        if(name=="LE") return &LE;
        if(name=="v")  return &v;
        if(name=="x")  return &x;
        if(name=="y")  return &y;
        if(name=="z")  return &z;

        /*else*/
	return 0;
    };

    ExprFunc* resolveFunc(const std::string& name) const
    {
        if(name=="func") return &func;
        else             return 0;
        /* else             return &dummyFunc; */
    };

    void setVar(const std::string & to,
                const std::string & from) {
        if(to=="v") v = ExprNothingVarRef(resolveVar(from)->type());
        if(to=="x") x = ExprNothingVarRef(resolveVar(from)->type());
        if(to=="y") y = ExprNothingVarRef(resolveVar(from)->type());
        if(to=="z") z = ExprNothingVarRef(resolveVar(from)->type());
        //TODO: rem ove
        //std::cerr<<"v is "<<v.type().toString()<<" from "<<from<<" is "<<(resolveVar(from)->type().toString())<<std::endl;
    };

private:
    //Various types
    mutable ExprNothingVarRef F1;
    mutable ExprNothingVarRef F2;
    mutable ExprNothingVarRef F3;
    mutable ExprNothingVarRef ST;
    mutable ExprNothingVarRef SE;
    mutable ExprNothingVarRef LC;
    mutable ExprNothingVarRef LU;
    mutable ExprNothingVarRef LV;
    mutable ExprNothingVarRef LE;

    //Use variables
    mutable ExprNothingVarRef v;
    mutable ExprNothingVarRef x;
    mutable ExprNothingVarRef y;
    mutable ExprNothingVarRef z;
};

#endif //TYPEBUILDER_H
