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
#ifndef TYPETESTER_H
#define TYPETESTER_H

#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "SeExprNode.h"
#include "SeExprFunc.h"
#include "TypeBuilder.h"
#include "TypePrinter.h"

/**
   @file TypeTester.h
*/
//! Simple expression class to check all final types of tests
class TypeTesterExpr : public TypeBuilderExpr
{
public:
    typedef SeExprType(*FindResultOne)  (const SeExprType &);
    typedef SeExprType(*FindResultTwo)  (const SeExprType &, const SeExprType &);
    typedef SeExprType(*FindResultThree)(const SeExprType &, const SeExprType &, const SeExprType &);

    TypePrintExaminer _examiner;
    SeExpr::ConstWalker  _walker;

    TypeTesterExpr()
        : TypeBuilderExpr(),_walker(&_examiner)
    {};

    TypeTesterExpr(const std::string &e)
        :  TypeBuilderExpr(e),_walker(&_examiner)
    {};

    void test(const std::string & expr,
              SeExprType expected_result,
              SeExprType actual_result,
              const std::string & givenString,
              int verbosity_level);

    void testSingle(const std::string & expr,
                    FindResultOne proc,
                    int verbosity_level);

    void testDouble(const std::string & expr,
                    FindResultTwo proc,
                    int verbosity_level);

    void testTriple(const std::string & expr,
                    FindResultThree proc,
                    int verbosity_level);

    virtual SeExprVarRef* resolveVar(const std::string& name) const {
        return TypeBuilderExpr::resolveVar(name);
    };

    SeExprFunc* resolveFunc(const std::string& name) const
    {
        return TypeBuilderExpr::resolveFunc(name);
    };
};

#endif // TYPETESTER_H
