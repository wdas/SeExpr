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
*
* @file BasicExpression.cpp
* @brief A basic expression context for the expression previewer
* @author  aselle
*/

#include "BasicExpression.h"

#include <SeExpr2/VarBlock.h>

BasicExpression::BasicExpression(const std::string& expr, const SeExpr2::ExprType& type)
    : Expression(expr, type), dummyFunc(dummyFuncX, 0, 16)
{
}

BasicExpression::~BasicExpression()
{
    clearVars();
}

template <class T_MAP>
void deleteAndClear(T_MAP& map)
{
    for (typename T_MAP::iterator i = map.begin(); i != map.end(); ++i)
        delete i->second;
    map.clear();
}

void BasicExpression::clearVars()
{
    deleteAndClear(varmap);
    funcmap.clear();
}

void BasicExpression::reset()
{
    Expression::reset();
    clearVars();
}

SeExpr2::ExprVarRef* BasicExpression::resolveVar(const std::string& name) const
{
    const SeExpr2::VarBlockCreator* symbols = varBlockCreator();
    if (!symbols || !symbols->resolveVar(name)) {
        // make a variable to resolve any unknown
        VARMAP::iterator i = varmap.find(name);
        if (i != varmap.end())
            return i->second;
        else {
            varmap[name] = new VectorRef();
            return varmap[name];
        }
    }
}

SeExpr2::ExprFunc* BasicExpression::resolveFunc(const std::string& name) const
{
    // check if it is builtin so we get proper behavior
    if (SeExpr2::ExprFunc::lookup(name))
        return 0;

    funcmap[name] = true;
    return &dummyFunc;
}
