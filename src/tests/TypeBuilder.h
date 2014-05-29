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
        virtual SeExprType retType () const { return SeExprType().FP(1).Varying(); };

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
         F1(SeExprType().FP(1).Varying() ),
         F2(SeExprType().FP(2).Varying()),
         F3(SeExprType().FP(3).Varying()),
         ST(SeExprType().String().Varying() ),
         SE(SeExprType().Error().Varying() ),
         LC(SeExprType().FP(1).Constant() ),
         LU(SeExprType().FP(1).Uniform() ),
         LV(SeExprType().FP(1).Varying() ),
         LE(SeExprType().FP(1).Error() ),
         v(SeExprType().FP(1).Varying() ),
         x(SeExprType().FP(1).Varying() ),
         y(SeExprType().FP(1).Varying() ),
         z(SeExprType().FP(1).Varying() )
    {};

    TypeBuilderExpr(const std::string &e)
        : SeExpression::SeExpression(e),
         dummyFunc(dummyFuncX,0,16),
         func(dummyFuncX,3,3),
         F1(SeExprType().FP(1).Varying() ),
         F2(SeExprType().FP(2).Varying()),
         F3(SeExprType().FP(3).Varying()),
         ST(SeExprType().String().Varying() ),
         SE(SeExprType().Error().Varying() ),
         LC(SeExprType().FP(1).Constant() ),
         LU(SeExprType().FP(1).Uniform() ),
         LV(SeExprType().FP(1).Varying() ),
         LE(SeExprType().FP(1).Error() ),
         v(SeExprType().FP(1).Varying() ),
         x(SeExprType().FP(1).Varying() ),
         y(SeExprType().FP(1).Varying() ),
         z(SeExprType().FP(1).Varying() )
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
        //TODO: rem ove
        //std::cerr<<"v is "<<v.type().toString()<<" from "<<from<<" is "<<(resolveVar(from)->type().toString())<<std::endl;
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
