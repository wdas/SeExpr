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
#include <gtest/gtest.h>

#include "typeTests.h"
#include <SeExpr2/TypeIterator.h>

void TypeTesterExpr::doTest(const std::string &testStr, ExprType expectedResult, ExprType actualResult) {
    setExpr(testStr);
    setDesiredReturnType(expectedResult);
    if (actualResult.isValid()) {
        EXPECT_TRUE(isValid());
        EXPECT_TRUE(returnType() == actualResult) << "         Expression: " << testStr
                                                  << "\n    Expected type: " << expectedResult.toString()
                                                  << "\n      Actual type: " << returnType().toString();
    } else
        EXPECT_FALSE(isValid());
}

void TypeTesterExpr::testOneVar(const std::string &testStr,
                                // SingleWholeTypeIterator::ProcType proc)
                                ExprType (*proc)(const ExprType &)) {
    SingleWholeTypeIterator iter("v", proc, this);
    int remaining = iter.start();
    // std::cerr << "doTest for " << iter.givenString() << std::endl;
    doTest(testStr, iter.result(), iter.result());

    while (remaining) {
        remaining = iter.next();
        // std::cerr << "doTest for " << iter.givenString() << std::endl;
        doTest(testStr, iter.result(), iter.result());
    }
}

void TypeTesterExpr::testTwoVars(const std::string &testStr,
                                 // DoubleWholeTypeIterator::ProcType proc)
                                 ExprType (*proc)(const ExprType &, const ExprType &)) {
    DoubleWholeTypeIterator iter("x", "y", proc, this);
    int remaining = iter.start();
    // std::cerr << "doTest for " << iter.givenString() << std::endl;
    doTest(testStr, iter.result(), iter.result());

    while (remaining) {
        remaining = iter.next();
        // std::cerr << "doTest for " << iter.givenString() << std::endl;
        doTest(testStr, iter.result(), iter.result());
    };
}

ExprType identity(const ExprType &type) {
    return type;
};

ExprType numeric(const ExprType &type) {
    return type.isFP() ? type : ExprType().Error().Varying();
    return type;
};

ExprType numericToScalar(const ExprType &type) {
    ExprType ret = ExprType().Error().Varying();
    if (type.isFP()) ret = ExprType().FP(1);
    ret.setLifetime(type);
    return ret;
};

ExprType numericToScalar(const ExprType &first, const ExprType &second) {
    ExprType ret = ExprType().Error();
    if (first.isFP() && second.isFP()) ret = ExprType().FP(1).Varying();
    ret.setLifetime(first, second);
    return ret;
};

ExprType generalComparison(const ExprType &first, const ExprType &second) {
    if (ExprType::valuesCompatible(first, second)) {
        ExprType t = ExprType().FP(1);
        t.setLifetime(first, second);
        return t;
    } else
        return ExprType().Error();
};

ExprType numericComparison(const ExprType &first, const ExprType &second) {
    if (first.isFP() && second.isFP() && ExprType::valuesCompatible(first, second)) {
        ExprType t = ExprType().FP(1);
        t.setLifetime(first, second);
        return t;
    } else
        return ExprType().Error();
};

ExprType numericToNumeric(const ExprType &first, const ExprType &second) {
    ExprType type = ExprType().Error();
    if (first.isFP() && second.isFP()) {
        if (first.dim() == second.dim())
            type = first;
        else if (first.isFP(1))
            type = second;
        else if (second.isFP(1))
            type = first;
    }
    type.setLifetime(first, second);
    // std::cerr<<first.toString()<<"+"
    //          <<second.toString()<<"="
    //          <<type.toString()<<std::endl;
    return type;
};

ExprType numericTo2Vector(const ExprType &first, const ExprType &second) {
    ExprType ret = ExprType().Error();
    if (first.isFP() & second.isFP()) ret = ExprType().FP(2);
    ret.setLifetime(first, second);
    return ret;
};

TEST(TypeTests, Assignment) {
    TypeTesterExpr expr;
    expr.testOneVar("$a = $v; $a", identity);
}

TEST(TypeTests, VectorIndex) {
    TypeTesterExpr expr;
    expr.testOneVar("[$v]", numericToScalar);
}

TEST(TypeTests, UnaryNegation) {
    TypeTesterExpr expr;
    expr.testOneVar("-$v", numeric);
}

TEST(TypeTests, UnaryLogicalNOT) {
    TypeTesterExpr expr;
    expr.testOneVar("!$v", numeric);
}

TEST(TypeTests, UnaryInversion) {
    TypeTesterExpr expr;
    expr.testOneVar("~$v", numeric);
}

// Fails when operands are type varying Float[3] and varying Float[2]
// and types are varying Float[2] and varying Float[3]

// TEST(TypeTests, BinaryLogicalAND)
// {
//     TypeTesterExpr expr;
//     expr.testTwoVars("$x && $y", numericToScalar);
// }

// Fails when operands are type varying Float[3] and varying Float[2]
// and types are varying Float[2] and varying Float[3]

// TEST(TypeTests, BinaryLogicalOR)
// {
//     TypeTesterExpr expr;
//     expr.testTwoVars("$x || $y", numericToScalar);
// }

TEST(TypeTests, VectorIndex2) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x[$y]", numericToScalar);
}

// Seg fault after testing types varying String and varying String

// TEST(TypeTests, BinaryEquality)
// {
//     TypeTesterExpr expr;
//     expr.testTwoVars("$x == $y", generalComparison);
// }

// Seg fault after testing types varying String and varying String

// TEST(TypeTests, BinaryInequality)
// {
//     TypeTesterExpr expr;
//     expr.testTwoVars("$x != $y", generalComparison);
// }

TEST(TypeTests, BinaryLessThan) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x <  $y", numericComparison);
}

TEST(TypeTests, BinaryGreaterThan) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x >  $y", numericComparison);
}

TEST(TypeTests, BinaryLessEqual) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x <= $y", numericComparison);
}

TEST(TypeTests, BinaryGreaterEqual) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x >= $y", numericComparison);
}

TEST(TypeTests, BinaryAddition) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x + $y", numericToNumeric);
}

TEST(TypeTests, BinarySubtraction) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x - $y", numericToNumeric);
}

TEST(TypeTests, BinaryMultiplication) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x * $y", numericToNumeric);
}

TEST(TypeTests, BinaryDivision) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x / $y", numericToNumeric);
}

TEST(TypeTests, BinaryModulo) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x % $y", numericToNumeric);
}

TEST(TypeTests, BinaryPower) {
    TypeTesterExpr expr;
    expr.testTwoVars("$x ^ $y", numericToNumeric);
}

TEST(TypeTests, TupleIndex) {
    TypeTesterExpr expr;
    expr.testTwoVars("[$x, $y]", numericTo2Vector);
}
