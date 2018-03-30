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
*
* @file BasicExpression.h
* @brief A basic expression context for the expression previewer
* @author  aselle
*/

#ifndef BasicExpression_h
#define BasicExpression_h

#include <map>
#include <SeExpr2/Expression.h>
#include <SeExpr2/ExprFunc.h>
#include <SeExpr2/ExprNode.h>

class BasicExpression : public SeExpr2::Expression {
  public:
    struct ScalarRef : public SeExpr2::ExprVarRef {
        double value;
        ScalarRef() : SeExpr2::ExprVarRef(SeExpr2::ExprType().FP(1).Varying()), value(0.0) {}
        void eval(double* result) { result[0] = value; }
        void eval(const char** result) { assert(false); }
    };

    struct VectorRef : public SeExpr2::ExprVarRef {
        SeExpr2::Vec3d value;
        VectorRef() : SeExpr2::ExprVarRef(SeExpr2::ExprType().FP(3).Varying()), value(0.0) {}
        void eval(double* result) {
            for (int k = 0; k < 3; k++) result[k] = value[k];
        };
        void eval(const char** result) {
            assert(false);
        };
    };

    struct DummyFuncX : SeExpr2::ExprFuncSimple {
        DummyFuncX() : SeExpr2::ExprFuncSimple(true) {}
        virtual ~DummyFuncX() {}

        virtual SeExpr2::ExprType prep(SeExpr2::ExprFuncNode* node,
                                       bool scalarWanted,
                                       SeExpr2::ExprVarEnvBuilder& envBuilder) const {
            bool valid = true;
            int nargs = node->numChildren();
            for (int i = 0; i < nargs; i++)
                valid &= node->checkArg(i, SeExpr2::ExprType().FP(3).Constant(), envBuilder);
            return valid ? SeExpr2::ExprType().FP(3).Varying() : SeExpr2::ExprType().Error();
        }

        virtual SeExpr2::ExprFuncNode::Data* evalConstant(const SeExpr2::ExprFuncNode* node, ArgHandle args) const {
            return new SeExpr2::ExprFuncNode::Data();
        }

        virtual void eval(ArgHandle args) {
            double* out = &args.outFp;
            for (int i = 0; i < 3; i++) out[i] = 0.0;
        }
    } dummyFuncX;
    mutable SeExpr2::ExprFunc dummyFunc;

    mutable ScalarRef u;
    mutable ScalarRef v;
    mutable VectorRef P;

    typedef std::map<std::string, VectorRef*> VARMAP;
    mutable VARMAP varmap;
    typedef std::map<std::string, bool> FUNCMAP;
    mutable FUNCMAP funcmap;

    BasicExpression(const std::string& expr, const SeExpr2::ExprType& type = SeExpr2::ExprType().FP(3));
    virtual ~BasicExpression();

    SeExpr2::ExprVarRef* resolveVar(const std::string& name) const;
    SeExpr2::ExprFunc* resolveFunc(const std::string& name) const;
    void setExpr(const std::string& str);
    void clearVars();
};

#endif
