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

#include <gtest.h>

#include <Expression.h>
#include <ExprFunc.h>
#include <Vec3d.h>
#include "SeTests.h"
using namespace SeExpr2;

struct SimpleExpression:public Expression
{
    // Define simple scalar variable type that just stores the value it returns
    struct Var:public ExprScalarVarRef
    {
        double value;
        void eval(const ExprVarNode* node,Vec3d& result)
        {result[0]=value;}
    };
    mutable Var x,y;

    // Custom variable resolver, only allow ones we specify
    ExprVarRef* resolveVar(const std::string& name) const
    {
        if(name=="x") return &x;
        if(name=="y") return &y;
        return 0;
    }

    // Make a custom sum function
    mutable ExprFunc customFunc;
    static double customFuncHelper(double x,double y)
    {return x+y;}

    // Custom function resolver
    ExprFunc* resolveFunc(const std::string& name) const
    {
        if(name=="custom") return &customFunc;
        return 0;
    }

    // Constructor
    SimpleExpression(const std::string& str)
        :Expression(str),customFunc(customFuncHelper)
    {}

};

TEST(BasicTests, Constant)
{
    Expression expr("3+4");
    EXPECT_TRUE(expr.isValid());
    EXPECT_TRUE(!expr.isVec());
    const double* val=expr.evalFP();
    EXPECT_TRUE(expr.isConstant());
    EXPECT_EQ(val[0],7);
}

TEST(BasicTests, Variables)
{
    SimpleExpression expr("$x+y");
    expr.x.value=3;
    expr.y.value=4;
    EXPECT_TRUE(expr.isValid());
    EXPECT_TRUE(!expr.isConstant());
    EXPECT_TRUE(!expr.isVec());
    EXPECT_TRUE(expr.usesVar("x"));
    EXPECT_TRUE(expr.usesVar("y"));
    EXPECT_TRUE(!expr.usesVar("z"));
    const double* val=expr.evalFP();
    EXPECT_EQ(val[0],7);
}

TEST(BasicTests, Custom)
{
    SimpleExpression expr("custom(1,2)");
    EXPECT_TRUE(expr.isValid());
    EXPECT_TRUE(!expr.isVec());
    EXPECT_TRUE(expr.isConstant());
    EXPECT_TRUE(expr.usesFunc("custom"));
    EXPECT_EQ(expr.evalFP()[0],3);
}

TEST(BasicTests, Precedence)
{
    SimpleExpression expr1("1+2*3");
    EXPECT_EQ(expr1.evalFP()[0],7);
    SimpleExpression expr2("(1+2)*3");
    EXPECT_EQ(expr2.evalFP()[0],9);
}

TEST(BasicTests, VectorAssignment)
{
    SimpleExpression expr1("$foo=[0,1,2]; $foo=3; $foo");
    double val1=expr1.evalFP()[0];
    EXPECT_EQ(val1,3);

    SimpleExpression expr3("$foo=3; $foo=[0,1,2]; $foo");
    SimpleExpression expr4("[0,1,2]");
    Vec<double,3,true> val3(const_cast<double*>(expr3.evalFP()));
    Vec<double,3,true> val4(const_cast<double*>(expr4.evalFP()));
    EXPECT_EQ(val3,val4);
}

