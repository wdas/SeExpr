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

#include <gtest/gtest.h>

#include <SeExpr2/Expression.h>
#include <SeExpr2/ExprFunc.h>

using namespace SeExpr2;


struct StringFunc : public ExprFuncSimple {
    StringFunc() : ExprFuncSimple(true) {
    }

    struct StringData : public SeExpr2::ExprFuncNode::Data, public std::string
    {
    };

    virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnvBuilder& envBuilder) const {
        bool constant = true;
        for (int i = 0, iend = node->numChildren(); i < iend; ++i) {
            SeExpr2::ExprType t = node->child(i)->prep(!scalarWanted, envBuilder);
            if (t.isString() == false) {
                return SeExpr2::ExprType().Error().Varying();
            }
            if (t.isLifetimeConstant() == false) {
                constant = false;
            }
        }
        return constant == true ? SeExpr2::ExprType().String().Constant() : SeExpr2::ExprType().String().Varying();
    }

    virtual ExprFuncNode::Data* evalConstant(const ExprFuncNode* node, ArgHandle args) const {
        return new StringData();
    }

    virtual void eval(ArgHandle args) {
        StringData& data = *reinterpret_cast<StringData*>(args.data);
        data.clear();
        for (int i = 0, iend = args.nargs(); i < iend; ++i) {
            data += args.inStr(i);
            if (i != iend - 1) {
                data += "/";
            }
        }
        args.outStr = const_cast<char*>(data.c_str());
    }
} joinPath;
ExprFunc joinPathFunc(joinPath, 2, 100);


struct StringExpression : public Expression {
    // Define simple string variable type that just stores the value it returns
    struct Var : public ExprVarRef {
        std::string value;
        Var() : ExprVarRef(ExprType().String().Varying()) {}
        void eval(double*) { }
        void eval(const char** result) { result[0] = value.c_str(); }
        Var& operator = (const char* input) { value = input; return *this; }
    };
    mutable Var stringVar;

    // Custom variable resolver, only allow ones we specify
    ExprVarRef* resolveVar(const std::string& name) const {
        if (name == "stringVar") return &stringVar;
        return 0;
    }

    // Custom function resolver
    ExprFunc* resolveFunc(const std::string& name) const {
        if (name == "join_path") return &joinPathFunc;
        return 0;
    }

    // Constructor
    StringExpression(const std::string& str)
        : Expression(str, ExprType().String()) {}
};

TEST(StringTests, Constant) {
    StringExpression expr("\"hello world !\"");
    EXPECT_TRUE(expr.isValid() == true);
    EXPECT_TRUE(expr.returnType().isString() == true);
    EXPECT_TRUE(expr.isConstant() == true);
    EXPECT_STREQ(expr.evalStr(), "hello world !");
}

TEST(StringTests, Variable) {
    StringExpression expr("stringVar");
    expr.stringVar = "hey, it's working !";
    EXPECT_TRUE(expr.isValid());
    EXPECT_TRUE(expr.returnType().isString());
    EXPECT_TRUE(!expr.isConstant());
    EXPECT_STREQ(expr.evalStr(), "hey, it's working !");
}

TEST(StringTests, FunctionConst) {
    StringExpression expr("join_path(\"/home/foo\", \"some\", \"relative\", \"path\")");
    EXPECT_TRUE(expr.isValid() == true);
    EXPECT_TRUE(expr.returnType().isString() == true);
    EXPECT_TRUE(expr.isConstant() == true);
    EXPECT_STREQ(expr.evalStr(), "/home/foo/some/relative/path");
}

TEST(StringTests, FunctionVarying) {
    StringExpression expr("join_path(stringVar, \"some\", \"relative\", \"path\")");
    expr.stringVar = "/home/foo";
    EXPECT_TRUE(expr.isValid() == true);
    EXPECT_TRUE(expr.returnType().isString() == true);
    EXPECT_TRUE(expr.isConstant() == false);
    EXPECT_STREQ(expr.evalStr(), "/home/foo/some/relative/path");
}

TEST(StringTests, BinaryOp) {
    StringExpression expr1("\"hello \" + \"world!\"");
    EXPECT_TRUE(expr1.isValid() == true);
    EXPECT_TRUE(expr1.returnType().isString() == true);
    EXPECT_TRUE(expr1.isConstant() == true);
    EXPECT_STREQ(expr1.evalStr(), "hello world!");

    StringExpression expr2("\"hello \" + \"world\" + \"!\"");
    EXPECT_TRUE(expr2.isValid() == true);
    EXPECT_TRUE(expr2.returnType().isString() == true);
    EXPECT_TRUE(expr2.isConstant() == true);
    EXPECT_STREQ(expr2.evalStr(), "hello world!");

    StringExpression expr3("stringVar + \"world!\"");
    expr3.stringVar = "hello ";
    EXPECT_TRUE(expr3.isValid() == true);
    EXPECT_TRUE(expr3.returnType().isString() == true);
    EXPECT_TRUE(expr3.isConstant() == false);
    EXPECT_STREQ(expr3.evalStr(), "hello world!");

    StringExpression expr4("join_path(\"a\", \"b\") + \"/c/\" + stringVar");
    expr4.stringVar = "d";
    EXPECT_TRUE(expr4.isValid() == true);
    EXPECT_TRUE(expr4.returnType().isString() == true);
    EXPECT_TRUE(expr4.isConstant() == false);
    EXPECT_STREQ(expr4.evalStr(), "a/b/c/d");
}
