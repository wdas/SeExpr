/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#ifndef _GrapherExpr_
#define _GrapherExpr_
#include <SeExpression.h>

//! Simple variable that just returns its internal value
struct SimpleVar:public SeExprScalarVarRef
{
    double val; // independent variable
        void eval(const SeExprVarNode* /*node*/,SeVec3d& result)
    {result[0]=val;}
};


//! Simple expression class to support our function grapher
class GrapherExpr:public SeExpression
{
    const std::map<std::string,SimpleVar>& vars;
public:
    //! Constructor that takes the expression to parse
    GrapherExpr(const std::string& expr,const std::map<std::string,SimpleVar>& vars)
        :SeExpression(expr),vars(vars)
    {}

    //! set the independent variable
    void setX(double x_input)
    {x.val=x_input;}

private:
    //! independent variable
    mutable SimpleVar x;

    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const
    {
        // check my internal variable
        if(name == "x") return &x;
        // check external variable table
        std::map<std::string,SimpleVar>::const_iterator i=vars.find(name);
        if(i!=vars.end()) return const_cast<SimpleVar*>(&i->second);
        // nothing found
        return 0;
    }
};
#endif
