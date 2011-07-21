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
#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "SeExprNode.h"
#include "SeExprFunc.h"
#include "TypeBuilder.h"
#include "TypeTester.h"
#include "TypeIterator.h"

/**
   @file TypeTester.cpp
*/

inline void
TypeTesterExpr::test(const std::string & expr, SeExprType result, const std::string & givenString, int verbosity_level) {
    setExpr(expr);
    setReturnType(result);

    if(verbosity_level >= 2)
        std::cout << "\t\tChecking expression: " << expr              << std::endl
                  << "\t\t\tGiven:        "      << givenString       << std::endl
                  << "\t\t\tAgainst type: "      << result.toString() << "\t";

    if(isValid()) {
        if(result != returnType()) {
            std::cerr << "Failed check." << std::endl;
            if(verbosity_level >= 1)
                std::cerr << "Expression: "  << expr                    << std::endl
                          << "\tExpected: "  << result      .toString() << std::endl
                          << "\tReceived: "  << returnType().toString() << std::endl;
        }
        else if(verbosity_level >= 2)
            std::cout << "Check passed!" << std::endl;
    } else //error
        if(result.isError() || result.isLTError())
            if(verbosity_level >= 2)
                std::cout << "Check passed!" << std::endl;
};

SeExprType identity(const SeExprType & type) { return type; };

inline void
TypeTesterExpr::testSingle(const std::string & expr, int verbosity_level) {
    SeExprType result;
    AbstractTypeIterator::FindResultOne proc = identity;
    SingleWholeTypeIterator iter("v", proc, this);

    if(verbosity_level >= 2)
        std::cout << "\tChecking type of expression: " << expr << std::endl;

    for(iter.start(); !iter.isEnd(); iter.next())
        test(expr, iter.result(), iter.givenString(), verbosity_level);
};

int main(int argc,char *argv[])
{
    int verbosity_level = 0;

    for(int i = 0; i < argc; i++)
        switch(argv[i][0]) {
        case '0':
            break;
        case '1':
            verbosity_level = 1;
            break;
        case '2':
            verbosity_level = 2;
            break;
        default:
            break;
        };

    TypeTesterExpr expr;
    std::string str;

    std::cout << "SeExpr Type Tester:" << std::endl;

    str = "$a = $v; $a";
    std::cout << "Testing " << str << std::endl;
    expr.testSingle(str, verbosity_level);

    return 0;
}
