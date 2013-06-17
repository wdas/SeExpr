/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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
