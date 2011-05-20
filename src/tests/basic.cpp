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
        {result[0]=value;}
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
    SeExprFunc* resolveFunc(const std::string& name) const
    {
        if(name=="custom") return &customFunc;
    }

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

    return 0;

}
