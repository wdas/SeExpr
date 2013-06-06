/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file SeExprEdExpression.cpp
* @brief A basic expression context for the expression previewer
* @author  aselle
*/

#include "SeExprEdExpression.h"

SeExprEdExpression::SeExprEdExpression(const std::string& expr,bool vec)
    :SeExpression(expr,vec),dummyFunc(dummyFuncX,0,16)
{}

SeExprEdExpression::~SeExprEdExpression()
{
    clearVars();
}

template<class T_MAP> void deleteAndClear(T_MAP& map){
    for(typename T_MAP::iterator i=map.begin();i!=map.end();++i) delete i->second;
    map.clear();
}

void SeExprEdExpression::clearVars()
{
    deleteAndClear(varmap);
    funcmap.clear();
}

void SeExprEdExpression::setExpr(const std::string& str)
{
    clearVars();
    SeExpression::setExpr(str);
}

SeExprVarRef* SeExprEdExpression::resolveVar(const std::string& name) const
{
    if(name=="u") return &u;
    else if(name=="v") return &v;
    else if(name=="P") return &P;
    else{
        // make a variable to resolve any unknown
        VARMAP::iterator i=varmap.find(name);
        if(i!=varmap.end()) return i->second;
        else{
            varmap[name]=new VectorRef();
            return varmap[name];
        }
    }
}

SeExprFunc* SeExprEdExpression::resolveFunc(const std::string& name) const
{
    // check if it is builtin so we get proper behavior
    if(SeExprFunc::lookup(name)) return 0;

    funcmap[name]=true;
    return &dummyFunc;
}
