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
#include "ExprMultiExpr.h"
#include <algorithm>
#include <set>

namespace SeExpr2 {
class GlobalVal : public ExprVarRef {
  public:
    GlobalVal(const std::string& varName, const SeExpr2::ExprType& et) : ExprVarRef(et), varName(varName)
    {
    }
    std::set<DExpression*> users;
    std::string varName;
};

struct GlobalFP : public GlobalVal {
    GlobalFP(const std::string& varName, int dim) : GlobalVal(varName, ExprType().FP(dim).Varying())
    {
        val.assign(dim, 0);
    }

    std::vector<double> val;
    void eval(double* result)
    {
        for (int i = 0; i < type().dim(); i++)
            result[i] = val[i];
    }
    void eval(const char**)
    {
        assert(false);
    }
    bool isVec()
    {
        return type().dim() > 1;
    }
};

struct GlobalStr : public GlobalVal {
    GlobalStr(const std::string& varName) : GlobalVal(varName, ExprType().String().Varying()), val(0)
    {
    }

    const char* val;
    void eval(double*)
    {
        assert(false);
    }
    void eval(const char** result)
    {
        *result = val;
    }
    bool isVec()
    {
        return 0;
    }
};
}

namespace {

std::set<SeExpr2::DExpression*> getAffectedExpr(SeExpr2::GlobalVal* gv)
{
    std::set<SeExpr2::DExpression*> ret;

    std::set<SeExpr2::DExpression*> workList = gv->users;
    while (workList.size()) {
        SeExpr2::DExpression* de = *workList.begin();
        workList.erase(de);
        ret.insert(de);
        workList.insert(de->val->users.begin(), de->val->users.end());
    }

    return ret;
}

std::set<SeExpr2::DExpression*> getTransitiveOperandExpr(SeExpr2::DExpression* expr)
{
    std::set<SeExpr2::DExpression*> ret;

    std::set<SeExpr2::DExpression*> workList;
    workList.insert(expr);
    while (workList.size()) {
        SeExpr2::DExpression* de = *workList.begin();
        workList.erase(de);
        ret.insert(de);
        workList.insert(de->operandExprs.begin(), de->operandExprs.end());
    }

    return ret;
}

std::set<SeExpr2::DExpression*> tmpOperandExprs;
std::set<SeExpr2::GlobalVal*> tmpOperandVars;
}

namespace SeExpr2 {

DExpression::DExpression(const std::string& varName,
                         Expressions& context,
                         const std::string& e,
                         const ExprType& type,
                         EvaluationStrategy be)
    : Expression(e, type, be), context(context)
{
    if (type.isFP())
        val = new GlobalFP(varName, type.dim());
    else if (type.isString())
        val = new GlobalStr(varName);
    else
        assert(false);

    operandExprs = context.AllExprs;
    operandVars = context.AllExternalVars;
    prep();
    operandExprs = tmpOperandExprs;
    operandVars = tmpOperandVars;
}

const std::string& DExpression::name() const
{
    return val->varName;
}

ExprVarRef* DExpression::resolveVar(const std::string& name) const
{
    // first time resolve var from all exprs & vars
    // then resolve var from used exprs & vars
    for (std::set<DExpression *>::iterator I = operandExprs.begin(), E = operandExprs.end(); I != E; ++I)
        if ((*I)->name() == name) {
            tmpOperandExprs.insert(*I);
            (*I)->val->users.insert(const_cast<DExpression*>(this));
            return (*I)->val;
        }

    for (std::set<GlobalVal *>::iterator I = operandVars.begin(), E = operandVars.end(); I != E; ++I)
        if ((*I)->varName == name) {
            tmpOperandVars.insert(*I);
            (*I)->users.insert(const_cast<DExpression*>(this));
            return *I;
        }

    addError(name + " fail resolveVar", 0, 0);
    return 0;
}

void DExpression::eval()
{
    if (_desiredReturnType.isFP()) {
        const double* ret = evalFP();
        GlobalFP* fpVal = dynamic_cast<GlobalFP*>(val);
        fpVal->val.assign(ret, ret + fpVal->val.size());
        return;
    }

    assert(_desiredReturnType.isString());
    GlobalStr* strVal = dynamic_cast<GlobalStr*>(val);
    strVal->val = evalStr();
}

Expressions::~Expressions()
{
    for (std::set<DExpression *>::iterator I = AllExprs.begin(), E = AllExprs.end(); I != E; ++I)
        delete *I;

    for (std::set<GlobalVal *>::iterator I = AllExternalVars.begin(), E = AllExternalVars.end(); I != E; ++I)
        delete *I;
}

VariableHandle Expressions::addExternalVariable(const std::string& variableName, ExprType seTy)
{
    std::pair<std::set<GlobalVal*>::iterator, bool> ret;

    if (seTy.isFP())
        ret = AllExternalVars.insert(new GlobalFP(variableName, seTy.dim()));
    else if (seTy.isString())
        ret = AllExternalVars.insert(new GlobalStr(variableName));
    else
        assert(false);

    return ret.first;
}

ExprHandle Expressions::addExpression(const std::string& varName, ExprType seTy, const std::string& expr)
{
    std::pair<std::set<DExpression*>::iterator, bool> ret;
    ret = AllExprs.insert(new DExpression(varName, *this, expr, seTy));
    return ret.first;
}

VariableSetHandle Expressions::getLoopVarSetHandle(VariableHandle vh)
{
    GlobalVal* thisvar = *vh;
    unsigned initSize = static_cast<unsigned>(thisvar->users.size());
    if (!initSize)
        return AllExternalVars.end();

    std::set<DExpression*> ret = getAffectedExpr(thisvar);
    exprToEval.insert(ret.begin(), ret.end());
    // std::cout << "exprToEval size is " << exprToEval.size() << std::endl;
    return vh;
}

void Expressions::setLoopVariable(VariableSetHandle handle, double* values, unsigned dim)
{
    if (handle == AllExternalVars.end())
        return;

    GlobalFP* thisvar = dynamic_cast<GlobalFP*>(*handle);
    assert(thisvar && "set value to variable with incompatible types.");

    assert(dim == thisvar->val.size());
    for (unsigned i = 0; i < dim; ++i)
        thisvar->val[i] = values[i];
}

void Expressions::setLoopVariable(VariableSetHandle handle, const char* values)
{
    if (handle == AllExternalVars.end())
        return;

    GlobalStr* thisvar = dynamic_cast<GlobalStr*>(*handle);
    assert(thisvar && "set value to variable with incompatible types.");
    thisvar->val = values;
}

void Expressions::setVariable(VariableHandle handle, double* values, unsigned dim)
{
    GlobalFP* thisvar = dynamic_cast<GlobalFP*>(*handle);
    assert(thisvar && "set value to variable with incompatible types.");

    assert(dim == thisvar->val.size());
    for (unsigned i = 0; i < dim; ++i)
        thisvar->val[i] = values[i];

    // eval loop invariant now.
    std::set<DExpression*> ret = getAffectedExpr(thisvar);
    for (std::set<DExpression *>::iterator I = ret.begin(), E = ret.end(); I != E; ++I)
        (*I)->eval();
}

void Expressions::setVariable(VariableHandle handle, const char* values)
{
    GlobalStr* thisvar = dynamic_cast<GlobalStr*>(*handle);
    assert(thisvar && "set value to variable with incompatible types.");
    thisvar->val = values;

    // eval loop invariant now.
    std::set<DExpression*> ret = getAffectedExpr(thisvar);
    for (std::set<DExpression *>::iterator I = ret.begin(), E = ret.end(); I != E; ++I)
        (*I)->eval();
}

bool Expressions::isValid() const
{
    bool ret = true;
    for (std::set<DExpression *>::const_iterator I = AllExprs.begin(), E = AllExprs.end(); I != E; ++I)
        ret &= (*I)->isValid();
    return ret;
}

ExprEvalHandle Expressions::getExprEvalHandle(ExprHandle eh)
{
    // std::cout << "exprToEval size is " << exprToEval.size() << std::endl;

    DExpression* de = *eh;
    std::set<DExpression*> all = getTransitiveOperandExpr(de);
    // std::cout << "all size is " << all.size() << std::endl;

    std::vector<DExpression*>::iterator it;

    std::vector<DExpression*> ret1(all.size());
    it = std::set_intersection(all.begin(), all.end(), exprToEval.begin(), exprToEval.end(), ret1.begin());
    ret1.resize(it - ret1.begin());

    std::vector<DExpression*> ret2(ret1.size());
    it = std::set_difference(ret1.begin(), ret1.end(), exprEvaled.begin(), exprEvaled.end(), ret2.begin());
    ret2.resize(it - ret2.begin());

    exprEvaled.insert(ret2.begin(), ret2.end());

    // std::cout << "ret2 size is " << ret2.size() << std::endl;
    return std::make_pair(eh, ret2);
}

const std::vector<double>& Expressions::evalFP(ExprEvalHandle eeh)
{
    // std::cout << "eeh.second.size() is " << eeh.second.size() << std::endl;
    for (std::vector<DExpression *>::iterator I = eeh.second.begin(), E = eeh.second.end(); I != E; ++I)
        (*I)->eval();

    GlobalFP* thisvar = dynamic_cast<GlobalFP*>((*eeh.first)->val);

    //    std::cout << thisvar->val[0] << ","
    //            << thisvar->val[1] << ","
    //            << thisvar->val[2] << std::endl;
    return thisvar->val;
}

const char* Expressions::evalStr(ExprEvalHandle eeh)
{
    for (std::vector<DExpression *>::iterator I = eeh.second.begin(), E = eeh.second.end(); I != E; ++I)
        (*I)->eval();

    GlobalStr* thisvar = dynamic_cast<GlobalStr*>((*eeh.first)->val);
    return thisvar->val;
}
}
