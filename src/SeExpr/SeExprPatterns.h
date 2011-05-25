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

#ifndef SeExprPatterns_h
#define SeExprPatterns_h

#include "SeExprNode.h"

inline const SeExprVarNode*
isVariable(const SeExprNode* testee)
{
    return dynamic_cast<const SeExprVarNode*>(testee);
};

inline const SeExprNumNode*
isScalar(const SeExprNode* testee)
{
    return dynamic_cast<const SeExprNumNode*>(testee);
};

inline const SeExprVecNode*
isVector(const SeExprNode* testee)
{
    return dynamic_cast<const SeExprVecNode*>(testee);
};

inline const SeExprVecNode*
isLitVec(const SeExprNode* testee)
{
    if(const SeExprVecNode* vec = isVector(testee))
        if(isScalar(vec->child(0))
           && isScalar(vec->child(1))
           && isScalar(vec->child(2)))
            return vec;

    return 0;
};

inline const SeExprStrNode*
isString(const SeExprNode* testee)
{
    return dynamic_cast<const SeExprStrNode*>(testee);
};

inline const SeExprAssignNode*
isAssign(const SeExprNode* testee)
{
    return dynamic_cast<const SeExprAssignNode*>(testee);
};

inline const SeExprFuncNode*
isFunc(const SeExprNode* testee)
{
    return dynamic_cast<const SeExprFuncNode*>(testee);
};

inline const SeExprFuncNode*
isNamedFunc(const SeExprNode* testee, const std::string& name)
{
    if(const SeExprFuncNode* func = isFunc(testee))
       if(name.compare(func->name()) == 0)
           return func;

    return 0;
};

inline const SeExprFuncNode*
isStrFunc(const SeExprNode* testee)
{
    if(const SeExprFuncNode* func = isFunc(testee)) {
        int max = testee->numChildren();
        for(int i = 0; i < max; ++i)
            if(isString(testee->child(i)))
                return func;
    };

    return 0;
};

inline bool
hasCurveNumArgs(const SeExprFuncNode* testee)
{
    /// numChildren must be multiple of 3 plus 1
    return !((testee->numChildren() - 1) % 3);  
};

inline const SeExprFuncNode*
isCurveFunc(const SeExprNode* testee)
{
    const SeExprFuncNode* curveFunc = isNamedFunc(testee, "curve");

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

inline const SeExprFuncNode*
isCcurveFunc(const SeExprNode* testee)
{
    const SeExprFuncNode* ccurveFunc = isNamedFunc(testee, "ccurve");

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

inline const SeExprAssignNode*
isScalarAssign(const SeExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a scalar
    if(const SeExprAssignNode* assign = isAssign(testee))
        if(isScalar(assign->child(0)))
            return assign;

    return 0;
};

inline const SeExprAssignNode*
isVectorAssign(const SeExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a vector
    if(const SeExprAssignNode* assign = isAssign(testee))
        if(isLitVec(assign->child(0)))
            return assign;

    return 0;
};

inline const SeExprAssignNode*
isStrFuncAssign(const SeExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a function with a string argument
    if(const SeExprAssignNode* assign = isAssign(testee))
        if(isStrFunc(assign->child(0)))
            return assign;

    return 0;
};

inline const SeExprAssignNode*
isCurveAssign(const SeExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a curve function
    if(const SeExprAssignNode* assign = isAssign(testee))
        if(isCurveFunc(assign->child(0)))
            return assign;

    return 0;
};

inline const SeExprAssignNode*
isCcurveAssign(const SeExprNode* testee)
{
    /// if testee is an assignment statement, check if its sole child is a ccurve function
    if(const SeExprAssignNode* assign = isAssign(testee))
        if(isCcurveFunc(assign->child(0)))
            return assign;

    return 0;
};

#endif
