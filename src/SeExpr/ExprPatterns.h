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

#ifndef ExprPatterns_h
#define ExprPatterns_h

#include "ExprNode.h"

namespace SeExpr2 {

inline const ExprVarNode*
isVariable(const ExprNode* testee)
{
    return dynamic_cast<const ExprVarNode*>(testee);
};

inline const ExprNumNode*
isScalar(const ExprNode* testee)
{
    return dynamic_cast<const ExprNumNode*>(testee);
};

inline const ExprVecNode*
isVector(const ExprNode* testee)
{
    return dynamic_cast<const ExprVecNode*>(testee);
};

inline const ExprVecNode*
isLitVec(const ExprNode* testee)
{
    if(const ExprVecNode* vec = isVector(testee))
        if(isScalar(vec->child(0))
           && isScalar(vec->child(1))
           && isScalar(vec->child(2)))
            return vec;

    return 0;
};

inline const ExprStrNode*
isString(const ExprNode* testee)
{
    return dynamic_cast<const ExprStrNode*>(testee);
};

inline const ExprAssignNode*
isAssign(const ExprNode* testee)
{
    return dynamic_cast<const ExprAssignNode*>(testee);
};

inline const ExprFuncNode*
isFunc(const ExprNode* testee)
{
    return dynamic_cast<const ExprFuncNode*>(testee);
};

inline const ExprFuncNode*
isNamedFunc(const ExprNode* testee, const std::string& name)
{
    if(const ExprFuncNode* func = isFunc(testee))
       if(name.compare(func->name()) == 0)
           return func;

    return 0;
};

inline const ExprFuncNode*
isStrFunc(const ExprNode* testee)
{
    if(const ExprFuncNode* func = isFunc(testee)) {
        int max = testee->numChildren();
        for(int i = 0; i < max; ++i)
            if(isString(testee->child(i)))
                return func;
    };

    return 0;
};

inline bool
hasCurveNumArgs(const ExprFuncNode* testee)
{
    /// numChildren must be multiple of 3 plus 1
    return !((testee->numChildren() - 1) % 3);  
};

inline const ExprFuncNode*
isCurveFunc(const ExprNode* testee)
{
    const ExprFuncNode* curveFunc = isNamedFunc(testee, "curve");

    if(curveFunc && hasCurveNumArgs(curveFunc)) {
        int numChildren = curveFunc->numChildren() - 2;
        for(int i = 1;
            i < numChildren && curveFunc;
            i += 3) {
            if(!isScalar(curveFunc->child(i)))
                curveFunc = 0;
            else if(!isScalar(curveFunc->child(i+1)))
                curveFunc = 0;
            else if(!isScalar(curveFunc->child(i+2)))
                curveFunc = 0;
        };
    };

    return curveFunc;
};

inline const ExprFuncNode*
isCcurveFunc(const ExprNode* testee)
{
    const ExprFuncNode* ccurveFunc = isNamedFunc(testee, "ccurve");

    if(ccurveFunc && hasCurveNumArgs(ccurveFunc)) {
        int numChildren = ccurveFunc->numChildren() - 2;
        for(int i = 1;
            i < numChildren && ccurveFunc;
            i += 3) {
            if(!isScalar(ccurveFunc->child(i)))
                ccurveFunc = 0;
            else if(!isScalar(ccurveFunc->child(i+1))
                    && !isLitVec(ccurveFunc->child(i+1)))
                ccurveFunc = 0;
            else if(!isScalar(ccurveFunc->child(i+2)))
                ccurveFunc = 0;
        };
    };

    return ccurveFunc;
};

inline const ExprAssignNode*
isScalarAssign(const ExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a scalar
    if(const ExprAssignNode* assign = isAssign(testee))
        if(isScalar(assign->child(0)))
            return assign;

    return 0;
};

inline const ExprAssignNode*
isVectorAssign(const ExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a vector
    if(const ExprAssignNode* assign = isAssign(testee))
        if(isLitVec(assign->child(0)))
            return assign;

    return 0;
};

inline const ExprAssignNode*
isStrFuncAssign(const ExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a function with a string argument
    if(const ExprAssignNode* assign = isAssign(testee))
        if(isStrFunc(assign->child(0)))
            return assign;

    return 0;
};

inline const ExprAssignNode*
isCurveAssign(const ExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a curve function
    if(const ExprAssignNode* assign = isAssign(testee))
        if(isCurveFunc(assign->child(0)))
            return assign;

    return 0;
};

inline const ExprAssignNode*
isCcurveAssign(const ExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a ccurve function
    if(const ExprAssignNode* assign = isAssign(testee))
        if(isCcurveFunc(assign->child(0)))
            return assign;

    return 0;
};

}
#endif
