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

#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>

#include "SeExprNode.h"
#include "SeExprFunc.h"

/**
   @file TypeBuilder.h
*/
//! Simple expression class to build all intermediate types
class TypeBuilderExpr : public SeExpression
{
public:
    struct DummyFuncX:SeExprFuncX
    {
        DummyFuncX()
            : SeExprFuncX(false)
        {};

        virtual bool       isScalar() const { return true;                          };
        virtual SeExprType retType () const { return SeExprType::FP1Type_varying(); };

        void eval(const SeExprFuncNode* node,SeVec3d& result) const
        {result=SeVec3d();}
    } dummyFuncX;
    mutable SeExprFunc dummyFunc;
    mutable SeExprFunc func;

    class SeExprNothingVarRef : public SeExprVarRef {
    public:
        SeExprNothingVarRef(const SeExprType & type)
            : SeExprVarRef::SeExprVarRef(type)
        {};

        void eval(const SeExprVarNode* node, SeVec3d& result)
        {
            result = 0.0;
        };
    };

    //! Empty constructor
    TypeBuilderExpr()
        : SeExpression::SeExpression(),
          dummyFunc(dummyFuncX,0,16),
          func(dummyFuncX,3,3),
          F1(SeExprType::FP1Type_varying   () ),
          F2(SeExprType::FPNType_varying   (2)),
          F3(SeExprType::FPNType_varying   (3)),
          ST(SeExprType::StringType_varying() ),
          SE(SeExprType::ErrorType_varying () ),
          LC(SeExprType::FP1Type_constant  () ),
          LU(SeExprType::FP1Type_uniform   () ),
          LV(SeExprType::FP1Type_varying   () ),
          LE(SeExprType::FP1Type_error     () ),
          v (SeExprType::FP1Type_varying   () ),
          x (SeExprType::FP1Type_varying   () ),
          y (SeExprType::FP1Type_varying   () ),
          z (SeExprType::FP1Type_varying   () )
    {};

    TypeBuilderExpr(const std::string &e, const SeExprType & type = SeExprType::AnyType_varying())
        : SeExpression::SeExpression(e, type),
          dummyFunc(dummyFuncX,0,16),
          F1(SeExprType::FP1Type_varying   () ),
          F2(SeExprType::FPNType_varying   (2)),
          F3(SeExprType::FPNType_varying   (3)),
          ST(SeExprType::StringType_varying() ),
          SE(SeExprType::ErrorType_varying () ),
          LC(SeExprType::FP1Type_constant  () ),
          LU(SeExprType::FP1Type_uniform   () ),
          LV(SeExprType::FP1Type_varying   () ),
          LE(SeExprType::FP1Type_error     () ),
          v (SeExprType::FP1Type_varying   () ),
          x (SeExprType::FP1Type_varying   () ),
          y (SeExprType::FP1Type_varying   () ),
          z (SeExprType::FP1Type_varying   () )
    {};

    //! resolve function
    SeExprVarRef* resolveVar(const std::string& name) const {
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

    SeExprFunc* resolveFunc(const std::string& name) const
    {
        if(name=="func") return &func;
        else             return 0;
        /* else             return &dummyFunc; */
    };

    void setVar(const std::string & to,
                const std::string & from) {
        if(to=="v") v = SeExprNothingVarRef(resolveVar(from)->type());
        if(to=="x") x = SeExprNothingVarRef(resolveVar(from)->type());
        if(to=="y") y = SeExprNothingVarRef(resolveVar(from)->type());
        if(to=="z") z = SeExprNothingVarRef(resolveVar(from)->type());
    };

private:
    //Various types
    mutable SeExprNothingVarRef F1;
    mutable SeExprNothingVarRef F2;
    mutable SeExprNothingVarRef F3;
    mutable SeExprNothingVarRef ST;
    mutable SeExprNothingVarRef SE;
    mutable SeExprNothingVarRef LC;
    mutable SeExprNothingVarRef LU;
    mutable SeExprNothingVarRef LV;
    mutable SeExprNothingVarRef LE;

    //Use variables
    mutable SeExprNothingVarRef v;
    mutable SeExprNothingVarRef x;
    mutable SeExprNothingVarRef y;
    mutable SeExprNothingVarRef z;
};

#endif //TYPEBUILDER_H
