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
#ifndef _GrapherExpr_
#define _GrapherExpr_
#include <SeExpression.h>

//! Simple variable that just returns its internal value
struct SimpleVar : public SeExprScalarVarRef {
    double val;  // independent variable
    void eval(const SeExprVarNode* /*node*/, SeVec3d& result) { result[0] = val; }
};

//! Simple expression class to support our function grapher
class GrapherExpr : public SeExpression {
    const std::map<std::string, SimpleVar>& vars;

  public:
    //! Constructor that takes the expression to parse
    GrapherExpr(const std::string& expr, const std::map<std::string, SimpleVar>& vars)
        : SeExpression(expr), vars(vars) {}

    //! set the independent variable
    void setX(double x_input) { x.val = x_input; }

  private:
    //! independent variable
    mutable SimpleVar x;

    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const {
        // check my internal variable
        if (name == "x") return &x;
        // check external variable table
        std::map<std::string, SimpleVar>::const_iterator i = vars.find(name);
        if (i != vars.end()) return const_cast<SimpleVar*>(&i->second);
        // nothing found
        return 0;
    }
};
#endif
