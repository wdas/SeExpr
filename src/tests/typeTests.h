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
#ifndef TYPETESTS_H
#define TYPETESTS_H

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <Expression.h>
#include <ExprNode.h>
#include <ExprFunc.h>

#include "TypeBuilder.h"
#include "TypePrinter.h"

using namespace SeExpr2;

/**
   @file typeTests.h
*/
//! Simple expression class to check all final types of tests
class TypeTesterExpr : public TypeBuilderExpr {
  public:
    typedef ExprType (*FindResultOne)(const ExprType &);
    typedef ExprType (*FindResultTwo)(const ExprType &, const ExprType &);
    typedef ExprType (*FindResultThree)(const ExprType &, const ExprType &, const ExprType &);

    TypePrintExaminer _examiner;
    SeExpr2::ConstWalker _walker;

    TypeTesterExpr() : TypeBuilderExpr(), _walker(&_examiner) {};

    TypeTesterExpr(const std::string &e) : TypeBuilderExpr(e), _walker(&_examiner) {};

    virtual ExprVarRef *resolveVar(const std::string &name) const {
        return TypeBuilderExpr::resolveVar(name);
    };

    ExprFunc *resolveFunc(const std::string &name) const {
        return TypeBuilderExpr::resolveFunc(name);
    };

    void doTest(const std::string &testStr, ExprType expectedResult, ExprType actualResult);

    void testOneVar(const std::string &testStr,
                    // SingleWholeTypeIterator::ProcType proc);
                    ExprType (*proc)(const ExprType &));

    void testTwoVars(const std::string &testStr,
                     // DoubleWholeTypeIterator::ProcType proc);
                     ExprType (*proc)(const ExprType &, const ExprType &));
};

#endif  // TYPETESTS_H
