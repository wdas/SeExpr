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
* @file SeExprEdExpression.h
* @brief A basic expression context for the expression previewer
* @author  aselle
*/

#ifndef SeExprEdExpression_h
#define SeExprEdExpression_h

#include <map>
#include <SeExpression.h>
#include <SeExprFunc.h>
#include <SeExprNode.h>
#include <SeExprMacros.h>

class SeExprEdExpression : public SeExpression
{
    public:
        struct ScalarRef : public SeExprScalarVarRef {
            double value;
            ScalarRef()
                : value(0.0) {}
            virtual void eval(const SeExprVarNode* node, SeVec3d& result) {
                UNUSED(node);
                result = value;
            }
        };

        struct VectorRef : public SeExprVectorVarRef {
            SeVec3d value;
            VectorRef()
                : value(0.0) {}
            virtual void eval(const SeExprVarNode* node, SeVec3d& result) {
                UNUSED(node);
                result = value;
            }
        };

        struct DummyFuncX:SeExprFuncX
        {
            DummyFuncX()
                :SeExprFuncX(true)
            {}

            bool prep(SeExprFuncNode* node,bool wantVec) 
            {
                bool valid=true;
                for(int i=0;i<node->numChildren();i++){
                    if(!node->isStrArg(i))
                        valid&=node->child(i)->prep(wantVec);
                }
                return true;}
            void eval(const SeExprFuncNode* node,SeVec3d& result) const
            {UNUSED(node); result=SeVec3d();}
        } dummyFuncX;
        mutable SeExprFunc dummyFunc;
    
        mutable ScalarRef u;
        mutable ScalarRef v;
        mutable VectorRef P;

        typedef std::map<std::string,VectorRef*> VARMAP;
        mutable VARMAP varmap;
        typedef std::map<std::string,bool> FUNCMAP;
        mutable FUNCMAP funcmap;

        SeExprEdExpression(const std::string& expr, bool vec);
        virtual ~SeExprEdExpression();
    
        SeExprVarRef* resolveVar(const std::string& name) const;
        SeExprFunc* resolveFunc(const std::string& name) const;
        void setExpr(const std::string& str);
        void clearVars();
};


#endif
