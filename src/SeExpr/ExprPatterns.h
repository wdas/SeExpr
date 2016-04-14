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

#ifndef ExprPatterns_h
#define ExprPatterns_h

#include "ExprNode.h"

namespace SeExpr2 {

inline const ExprVarNode* isVariable(const ExprNode* testee) {
    return dynamic_cast<const ExprVarNode*>(testee);
};

inline const ExprNumNode* isScalar(const ExprNode* testee) {
    return dynamic_cast<const ExprNumNode*>(testee);
};

inline const ExprVecNode* isVector(const ExprNode* testee) {
    return dynamic_cast<const ExprVecNode*>(testee);
};

inline const ExprVecNode* isLitVec(const ExprNode* testee) {
    if (const ExprVecNode* vec = isVector(testee))
        if (isScalar(vec->child(0)) && isScalar(vec->child(1)) && isScalar(vec->child(2))) return vec;

    return 0;
};

inline const ExprStrNode* isString(const ExprNode* testee) {
    return dynamic_cast<const ExprStrNode*>(testee);
};

inline const ExprAssignNode* isAssign(const ExprNode* testee) {
    return dynamic_cast<const ExprAssignNode*>(testee);
};

inline const ExprFuncNode* isFunc(const ExprNode* testee) {
    return dynamic_cast<const ExprFuncNode*>(testee);
};

inline const ExprFuncNode* isNamedFunc(const ExprNode* testee, const std::string& name) {
    if (const ExprFuncNode* func = isFunc(testee))
        if (name.compare(func->name()) == 0) return func;

    return 0;
};

inline const ExprFuncNode* isStrFunc(const ExprNode* testee) {
    if (const ExprFuncNode* func = isFunc(testee)) {
        int max = testee->numChildren();
        for (int i = 0; i < max; ++i)
            if (isString(testee->child(i))) return func;
    };

    return 0;
};

inline bool hasCurveNumArgs(const ExprFuncNode* testee) {
    /// numChildren must be multiple of 3 plus 1
    return !((testee->numChildren() - 1) % 3);
};

inline const ExprFuncNode* isCurveFunc(const ExprNode* testee) {
    const ExprFuncNode* curveFunc = isNamedFunc(testee, "curve");

    if (curveFunc && hasCurveNumArgs(curveFunc)) {
        int numChildren = curveFunc->numChildren() - 2;
        for (int i = 1; i < numChildren && curveFunc; i += 3) {
            if (!isScalar(curveFunc->child(i)))
                curveFunc = 0;
            else if (!isScalar(curveFunc->child(i + 1)))
                curveFunc = 0;
            else if (!isScalar(curveFunc->child(i + 2)))
                curveFunc = 0;
        };
    };

    return curveFunc;
};

inline const ExprFuncNode* isCcurveFunc(const ExprNode* testee) {
    const ExprFuncNode* ccurveFunc = isNamedFunc(testee, "ccurve");

    if (ccurveFunc && hasCurveNumArgs(ccurveFunc)) {
        int numChildren = ccurveFunc->numChildren() - 2;
        for (int i = 1; i < numChildren && ccurveFunc; i += 3) {
            if (!isScalar(ccurveFunc->child(i)))
                ccurveFunc = 0;
            else if (!isScalar(ccurveFunc->child(i + 1)) && !isLitVec(ccurveFunc->child(i + 1)))
                ccurveFunc = 0;
            else if (!isScalar(ccurveFunc->child(i + 2)))
                ccurveFunc = 0;
        };
    };

    return ccurveFunc;
};

inline const ExprAssignNode* isScalarAssign(const ExprNode* testee) {
    /// if testee is an assignment statement, check if its sole child is a scalar
    if (const ExprAssignNode* assign = isAssign(testee))
        if (isScalar(assign->child(0))) return assign;

    return 0;
};

inline const ExprAssignNode* isVectorAssign(const ExprNode* testee) {
    /// if testee is an assignment statement, check if its sole child is a vector
    if (const ExprAssignNode* assign = isAssign(testee))
        if (isLitVec(assign->child(0))) return assign;

    return 0;
};

inline const ExprAssignNode* isStrFuncAssign(const ExprNode* testee) {
    /// if testee is an assignment statement, check if its sole child is a function with a string argument
    if (const ExprAssignNode* assign = isAssign(testee))
        if (isStrFunc(assign->child(0))) return assign;

    return 0;
};

inline const ExprAssignNode* isCurveAssign(const ExprNode* testee) {
    /// if testee is an assignment statement, check if its sole child is a curve function
    if (const ExprAssignNode* assign = isAssign(testee))
        if (isCurveFunc(assign->child(0))) return assign;

    return 0;
};

inline const ExprAssignNode* isCcurveAssign(const ExprNode* testee) {
    /// if testee is an assignment statement, check if its sole child is a ccurve function
    if (const ExprAssignNode* assign = isAssign(testee))
        if (isCcurveFunc(assign->child(0))) return assign;

    return 0;
};
}
#endif
