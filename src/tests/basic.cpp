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
#include <SeExpression.h>
#include <SeExprFunc.h>
#include <SeVec3d.h>

#include "SeTests.h"

struct SimpleExpression:public SeExpression
{
    // Define a simple scalar variable type that just stores the value it will return
    struct Var:public SeExprScalarVarRef
    {
        double value;
        void eval(const SeExprVarNode* node,SeVec3d& result)
        {UNUSED(node); result[0]=value;}
    };
    mutable Var x,y;

    // Custom variable resolver, only allow ones we specify
    SeExprVarRef* resolveVar(const std::string& name) const
    {
        if(name=="x") return &x;
        if(name=="y") return &y;
        return 0;
    }

    // Make a custom sum function
    mutable SeExprFunc customFunc;
    static double customFuncHelper(double x,double y)
    {return x+y;}

    // Custom function resolver
/*
    SeExprFunc* resolveFunc(const std::string& name) const
    {
        if(name=="custom") return &customFunc;
    }
*/

    // Constructor
    SimpleExpression(const std::string& str)
        :SeExpression(str),customFunc(customFuncHelper)
    {}

};

int main()
{
    // Basic constant expression
    {
        SeExpression expr("3+4");
        SE_TEST_ASSERT(expr.isValid());
        SE_TEST_ASSERT(!expr.isVec());
        SeVec3d val=expr.evaluate();
        SE_TEST_ASSERT(expr.isConstant());
        SE_TEST_ASSERT_EQUAL(val[0],7);
    }

    // Simple expression with variables
    {
        SimpleExpression expr("$x+y");
        expr.x.value=3;
        expr.y.value=4;
        SE_TEST_ASSERT(expr.isValid());
        SE_TEST_ASSERT(!expr.isConstant());
        SE_TEST_ASSERT(!expr.isVec());
        SE_TEST_ASSERT(expr.usesVar("x"));
        SE_TEST_ASSERT(expr.usesVar("y"));
        SE_TEST_ASSERT(!expr.usesVar("z"));
        SeVec3d val=expr.evaluate();
        SE_TEST_ASSERT_EQUAL(val[0],7);
    }

    // Simple expression with custom function
    {
        SimpleExpression expr("custom(1,2)");
        SE_TEST_ASSERT(expr.isValid());
        SE_TEST_ASSERT(!expr.isVec());
        SE_TEST_ASSERT(!expr.isConstant());
        SE_TEST_ASSERT(expr.usesFunc("custom"));
        SE_TEST_ASSERT_EQUAL(expr.evaluate()[0],3);
    }

    // Simple precedence rules
    {
	SimpleExpression expr1("1+2*3");
	SE_TEST_ASSERT_EQUAL(expr1.evaluate()[0],7);
	SimpleExpression expr2("(1+2)*3");
	SE_TEST_ASSERT_EQUAL(expr2.evaluate()[0],9);
    }

    //  Vector assignment test
    {
	SimpleExpression expr1("$foo=[0,1,2]; $foo=3; $foo");
	SimpleExpression expr2("3");
	SE_TEST_ASSERT_VECTOR_EQUAL(expr1.evaluate(),expr2.evaluate());
	SimpleExpression expr3("$foo=3; $foo=[0,1,2]; $foo");
	SimpleExpression expr4("[0,1,2]");
	SE_TEST_ASSERT_VECTOR_EQUAL(expr3.evaluate(),expr4.evaluate());
    }

    // Test memory leaks with parse errors
    {
        SimpleExpression expr("$foo=3;$bar=10;$foo[$bar");
        bool valid=expr.isValid();
        const char* expectedError="Unexpected end of expression':\n    $foo=3;$bar=10;$foo[$bar";
        SE_TEST_ASSERT_EQUAL(expr.parseError(),std::string(expectedError));
        SE_TEST_ASSERT_EQUAL(valid,false);
    }

    return 0;

}
