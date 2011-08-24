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

    TypeTesterExpr(const std::string &e, const SeExprType & type = SeExprType::AnyType_varying())
        :  TypeBuilderExpr(e, type),_walker(&_examiner)
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
