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

void
TypeTesterExpr::test(const std::string & expr, SeExprType result, const std::string & givenString, int verbosity_level) {
    bool       error      = false;
    bool       parseError = false;
    SeExprType returned   = SeExprType::ErrorType();

    setExpr(expr);
    setReturnType(result);

    if(verbosity_level >= 3)
        std::cout << "\tChecking expression:\t" << expr                     << std::endl
                  << "\t\tGiven:        "       << givenString              << std::endl
                  << "\t\tAgainst type: "       << result.toUniformString() << "\t";

    if(isValid()) {
        if(result != returnType())
            error = true;
    } else {
        parseError = true;
        if(!result.isError() &&
           !result.isLTError())
            error = true;
    }

    if(!parseError) returned = returnType();

    if(error) {
        std::cerr << "Failed check." << std::endl;
        if     (verbosity_level == 1)
            std::cerr << "\t\tExpression: "  << expr                       << std::endl
                      << "\t\tExpected:   "  << result  .toUniformString() << std::endl
                      << "\t\tReceived:   "  << returned.toUniformString() << std::endl;
        else if(verbosity_level == 2)
            std::cerr << "\t\tExpression: "  << expr                       << std::endl
                      << "\t\tGiven:      "  << givenString                << std::endl
                      << "\t\tExpected:   "  << result  .toUniformString() << std::endl
                      << "\t\tReceived:   "  << returned.toUniformString() << std::endl;
        else if(verbosity_level >= 3)
            std::cerr << "\t\tExpected:   "  << result  .toUniformString() << std::endl
                      << "\t\tReceived:   "  << returned.toUniformString() << std::endl;
    }
    else if(verbosity_level >= 3)
        std::cout << "Check passed!" << std::endl;
};

SeExprType
identity(const SeExprType & type)
{
    return type;
};

SeExprType
numeric(const SeExprType & type)
{
    if(type.isUnderNumeric()) return type;
    else                      return SeExprType::ErrorType();
};

SeExprType
numericToScalar(const SeExprType & type)
{
    if(type.isUnderNumeric()) return SeExprType::FP1Type  ().becomeLT(type);
    else                      return SeExprType::ErrorType();
};

SeExprType
numericToScalar(const SeExprType & first,
                const SeExprType & second)
{
    if(first .isUnderNumeric() &&
       second.isUnderNumeric())   return SeExprType::FP1Type  ().becomeLT(first, second);
    else                          return SeExprType::ErrorType();
};

SeExprType
generalComparison(const SeExprType & first,
                  const SeExprType & second)
{
    if(first .isValid()    &&
       second.isValid()    &&
       first.match(second))   return SeExprType::FP1Type  ().becomeLT(first, second);
    else                      return SeExprType::ErrorType();
};

SeExprType
numericComparison(const SeExprType & first,
                  const SeExprType & second)
{
    if(first .isUnderNumeric() &&
       second.isUnderNumeric() &&
       first.match(second))       return SeExprType::FP1Type  ().becomeLT(first, second);
    else                          return SeExprType::ErrorType();
};

SeExprType
numericToNumeric(const SeExprType & first,
                 const SeExprType & second)
{
    //if first and second are both numeric and both match:
    //   both first and second are FP1,
    //   first is FP1 and second is FPN,
    //   first is FPN and second is FP1, or
    //   both first and second are FPN (and the same N)
    if(first .isUnderNumeric() &&
       second.isUnderNumeric() &&
       first.match(second))       return SeExprType::FPNType  ((first.isFP1()) ? second.dim() : first.dim()).becomeLT(first, second);
    else                          return SeExprType::ErrorType();
};

SeExprType
numericTo2Vector(const SeExprType & first,
                 const SeExprType & second)
{
    if(first .isUnderNumeric() &&
       second.isUnderNumeric())   return SeExprType::FPNType  (2).becomeLT(first, second);
    else                          return SeExprType::ErrorType();
};

void
TypeTesterExpr::testSingle(const std::string & expr, AbstractTypeIterator::FindResultOne proc, int verbosity_level) {
    SeExprType result;
    SingleWholeTypeIterator iter("v", proc, this);

    if(verbosity_level >= 1)
        std::cout << "Checking expression: " << expr << std::endl;

    for(iter.start(); !iter.isEnd(); iter.next())
        test(expr, iter.result(), iter.givenUniformString(), verbosity_level);
};

void
TypeTesterExpr::testDouble(const std::string & expr, AbstractTypeIterator::FindResultTwo proc, int verbosity_level) {
    SeExprType result;
    DoubleWholeTypeIterator iter("x", "y", proc, this);

    if(verbosity_level >= 1)
        std::cout << "Checking expression: " << expr << std::endl;

    for(iter.start(); !iter.isEnd(); iter.next())
        test(expr, iter.result(), iter.givenUniformString(), verbosity_level);
};

void
testOne(const std::string & str, TypeTesterExpr & expr, AbstractTypeIterator::FindResultOne proc, int verbosity_level) {
    expr.testSingle(str, proc, verbosity_level);
};

void
testTwo(const std::string & str, TypeTesterExpr & expr, AbstractTypeIterator::FindResultTwo proc, int verbosity_level) {
    expr.testDouble(str, proc, verbosity_level);
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
        case '3':
            verbosity_level = 3;
            break;
        default:
            break;
        };

    TypeTesterExpr expr;
    std::string str;

    testOne("$a = $v; $a", expr, identity,          verbosity_level);
    testOne("[$v]",        expr, numericToScalar,   verbosity_level);
    testOne("-$v",         expr, numeric,           verbosity_level);
    testOne("!$v",         expr, numeric,           verbosity_level);
    testOne("~$v",         expr, numeric,           verbosity_level);
    testTwo("$x && $y",    expr, numericToScalar,   verbosity_level);
    testTwo("$x || $y",    expr, numericToScalar,   verbosity_level);
    testTwo("$x[$y]",      expr, numericToScalar,   verbosity_level);
    testTwo("$x == $y",    expr, generalComparison, verbosity_level);
    testTwo("$x != $y",    expr, generalComparison, verbosity_level);
    testTwo("$x <  $y",    expr, numericComparison, verbosity_level);
    testTwo("$x >  $y",    expr, numericComparison, verbosity_level);
    testTwo("$x <= $y",    expr, numericComparison, verbosity_level);
    testTwo("$x >= $y",    expr, numericComparison, verbosity_level);
    testTwo("$x + $y",     expr, numericToNumeric,  verbosity_level);
    testTwo("$x - $y",     expr, numericToNumeric,  verbosity_level);
    testTwo("$x * $y",     expr, numericToNumeric,  verbosity_level);
    testTwo("$x / $y",     expr, numericToNumeric,  verbosity_level);
    testTwo("$x % $y",     expr, numericToNumeric,  verbosity_level);
    testTwo("$x ^ $y",     expr, numericToNumeric,  verbosity_level);
    testTwo("[$x, $y]",    expr, numericTo2Vector,  verbosity_level);

    return 0;
}
