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

/* Parse tree nodes - this is where the expression evaluation happens.

   Some implementation notes: 

   1) Vector vs scalar - Any node can accept vector or scalar inputs
   and return a vector or scalar result.  If a node returns a scalar,
   it is only required to set the [0] component and the other
   components must be assumed to be invalid.

   2) SeExprNode::prep - This is called for all nodes during parsing
   once the syntax has been checked.  Anything can be done here, such
   as function binding, variable lookups, etc., but the only thing
   that must be done is to determine whether the result is going to be
   vector or scalar.  This can in some cases depend on whether the
   children are vector or scalar so the parser calls prep on the root
   node and each node is expected to call prep on its children and
   then set its own _isVec variable.  The wantVec param provides
   context from the parent (and ultimately the owning expression) as
   to whether a vector is desired, but nodes are not bound by this and
   may produce a scalar even when a vector is wanted.
   
   The base class method implements the default behavior which is to
   pass down the wantVec flag to all children and set isVec to true if
   any child is a vec.

   If the prep method fails, an error string should be set and false
   should be returned.
*/

#ifndef MAKEDEPEND
#include <cmath>
#endif
#include "SeVec3d.h"
#include "SeExpression.h"
#include "SeExprNode.h"
#include "SeExprFunc.h"


SeExprNode::SeExprNode(const SeExpression* expr)
    : _expr(expr), _parent(0), _isVec(0)
{
}


SeExprNode::SeExprNode(const SeExpression* expr, SeExprNode* a)
    : _expr(expr), _parent(0), _isVec(0)
{
    _children.reserve(1);
    addChild(a);
}


SeExprNode::SeExprNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b)
    : _expr(expr), _parent(0), _isVec(0)
{
    _children.reserve(2);
    addChild(a);
    addChild(b);
}


SeExprNode::SeExprNode(const SeExpression* expr, SeExprNode* a, SeExprNode* b,
		       SeExprNode* c)
    : _expr(expr), _parent(0), _isVec(0)
{
    _children.reserve(3);
    addChild(a);
    addChild(b);
    addChild(c);
}


SeExprNode::~SeExprNode()
{
    // delete children
    std::vector<SeExprNode*>::iterator iter;
    for (iter = _children.begin(); iter != _children.end(); iter++)
	delete *iter;
}


void
SeExprNode::addChild(SeExprNode* child)
{
    _children.push_back(child);
    child->_parent = this;
}


void
SeExprNode::addChildren(SeExprNode* surrogate)
{
    std::vector<SeExprNode*>::iterator iter;
    for (iter = surrogate->_children.begin();
	 iter != surrogate->_children.end(); 
	 iter++)
    {
	addChild(*iter);
    }
    surrogate->_children.clear();
    delete surrogate;
}


bool
SeExprNode::prep(bool wantVec)
{
    /* The default behavior is to pass down the wantVec flag to
       all children and set isVec to true if any child is a vec. */
    bool valid=true;
    std::vector<SeExprNode*>::iterator iter;
    _isVec = 0;
    for (iter = _children.begin(); iter != _children.end(); iter++) {
	SeExprNode* child = *iter;
	if (!child->prep(wantVec)) valid=false;
	if (child->isVec()) _isVec = 1;
    }
    return valid;
}


void
SeExprNode::eval(SeVec3d& result) const
{
    // the default behavior is to just eval all the children for
    // their side-effects (i.e. setting variables)
    // there is no result value
    SeVec3d val;
    for (int i = 0; i < numChildren(); i++)
	child(i)->eval(val);
    result = 0.0;
}


bool
SeExprBlockNode::prep(bool wantVec)
{
    // prepare variable assignments (request vector type)
    bool valid=true;

    if (!child(0)->prep(1)) valid=false;

    // prepare expression
    if (!child(1)->prep(wantVec)) valid=false;

    _isVec = child(1)->isVec();
    return valid;
}


void
SeExprBlockNode::eval(SeVec3d& result) const
{
    // eval block, then eval primary expr
    SeVec3d val;
    child(0)->eval(val);
    child(1)->eval(result);
}


bool
SeExprIfThenElseNode::prep(bool /*wantVec*/)
{
    bool valid=true;
    // prepare condition expression (request vector type)
    if (!child(0)->prep(0)) valid=false;

    // prepare then/else blocks
    if (!child(1)->prep(1)) valid=false;
    if (!child(2)->prep(1)) valid=false;
    _isVec = 0;
    return valid;
}


void
SeExprIfThenElseNode::eval(SeVec3d& result) const
{
    // eval condition, then choose then/else block
    SeVec3d val;
    child(0)->eval(val);
    if (val[0])
	child(1)->eval(val);
    else
	child(2)->eval(val);
    result = 0.0;
}


bool
SeExprAssignNode::prep(bool /*wantVec*/)
{
    // prepare expression
    if (!child(0)->prep(1)) return 0;
    _isVec = child(0)->isVec();

    // add to var table
    _var = _expr->getLocalVar(_name);
    if (_isVec) _var->setIsVec();
    return 1;
}


void
SeExprAssignNode::eval(SeVec3d& result) const
{
    if (_var) {
	// eval expression and store in variable
	const SeExprNode* node = child(0);
	node->eval(_var->val);
	if (_var->isVec() && !node->isVec())
	    _var->val[1] = _var->val[2] = _var->val[0];
    }
    else result = 0.0;
}


bool
SeExprVecNode::prep(bool wantVec)
{
    // want scalar children, result is vector
    if (!SeExprNode::prep(0)) return 0;
    _isVec = wantVec;
    return 1;
}


void
SeExprVecNode::eval(SeVec3d& result) const
{
    if (_isVec) {
	SeVec3d v;
	child(0)->eval(v); result[0] = v[0];
	child(1)->eval(v); result[1] = v[0];
	child(2)->eval(v); result[2] = v[0];
    } else {
	child(0)->eval(result);
    }
}


bool
SeExprCondNode::prep(bool wantVec)
{
    bool valid=true;
    // want scalar condition, result can be scalar or vector
    if (!child(0)->prep(0)) valid=false;
    if (!child(1)->prep(wantVec)) valid=false;
    if (!child(2)->prep(wantVec)) valid=false;
    _isVec = wantVec && (child(1)->isVec() || child(2)->isVec());
    return valid;
}


void
SeExprCondNode::eval(SeVec3d& result) const
{
    SeVec3d v;
    child(0)->eval(v);
    const SeExprNode* node = v[0] ? child(1) : child(2);
    node->eval(result);
    if (_isVec && !node->isVec())
	result[1] = result[2] = result[0];
}


bool
SeExprAndNode::prep(bool /*wantVec*/)
{
    // want scalar children, result is scalar
    if (!SeExprNode::prep(0)) return 0;
    _isVec = 0;
    return 1;
}


void
SeExprAndNode::eval(SeVec3d& result) const
{
    // operands and result must be scalar
    SeVec3d a, b;
    child(0)->eval(a);
    if (!a[0]) {
	result[0] = 0;
    } else { 
	child(1)->eval(b);
	result[0] = (b[0] != 0.0); 
    }
}


bool
SeExprOrNode::prep(bool /*wantVec*/)
{
    // want scalar children, result is scalar
    if (!SeExprNode::prep(0)) return 0;
    _isVec = 0;
    return 1;
}


void
SeExprOrNode::eval(SeVec3d& result) const
{
    // operands and result must be scalar
    SeVec3d a, b;
    child(0)->eval(a);
    if (a[0]) {
	result[0] = 1;
    } else { 
	child(1)->eval(b);
	result[0] = (b[0] != 0.0); 
    }
}


bool
SeExprSubscriptNode::prep(bool /*wantVec*/)
{
    bool valid=true;
    // want vector first child and scalar second child
    // result is scalar
    if (!child(0)->prep(1)) valid=false;
    if (!child(1)->prep(0)) valid=false;
    _isVec = 0;
    return valid;
}


void
SeExprSubscriptNode::eval(SeVec3d& result) const
{
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    SeVec3d a, b;
    child0->eval(a);
    child1->eval(b);
    int index = int(b[0]);

    if (child0->isVec()) {
	switch(index) {
	case 0:  result[0] = a[0]; break;
	case 1:  result[0] = a[1]; break;
	case 2:  result[0] = a[2]; break;
	default: result[0] = 0; break;
	}
    } else {
	switch(index) {
	case 0:
	case 1:
	case 2:  result[0] = a[0]; break;
	default: result[0] = 0; break;
	}
    }
}


void
SeExprNegNode::eval(SeVec3d& result) const
{
    SeVec3d a;
    const SeExprNode* child0 = child(0);
    child0->eval(a);
    result[0] = -a[0];
    if (_isVec) {
	result[1] = -a[1];
	result[2] = -a[2];
    }
}


void
SeExprInvertNode::eval(SeVec3d& result) const
{
    SeVec3d a;
    const SeExprNode* child0 = child(0);
    child0->eval(a);
    result[0] = 1-a[0];
    if (_isVec) {
	result[1] = 1-a[1];
	result[2] = 1-a[2];
    }
}


void
SeExprNotNode::eval(SeVec3d& result) const
{
    SeVec3d a;
    const SeExprNode* child0 = child(0);
    child0->eval(a);
    result[0] = !a[0];
    if (_isVec) {
	result[1] = !a[1];
	result[2] = !a[2];
    }
}


bool
SeExprCompareEqNode::prep(bool wantVec)
{
    wantVec = true; // children can be vector for ==, !=
    if (!SeExprNode::prep(wantVec)) return 0;
    _isVec = 0; // result is always scalar
    return 1;
}


bool
SeExprCompareNode::prep(bool wantVec)
{

    wantVec = false; // for <, >, <=, >=, only scalar values are used
    if (!SeExprNode::prep(wantVec)) return 0;
    _isVec = 0; // result is always scalar
    return 1;
}


void
SeExprEqNode::eval(SeVec3d& result) const
{
    SeVec3d a, b;
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    child0->eval(a);
    child1->eval(b);

    if (!child0->isVec()) a[1] = a[2] = a[0];
    if (!child1->isVec()) b[1] = b[2] = b[0];
    result[0] = a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}


void
SeExprNeNode::eval(SeVec3d& result) const
{
    SeVec3d a, b;
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    child0->eval(a);
    child1->eval(b);

    if (!child0->isVec()) a[1] = a[2] = a[0];
    if (!child1->isVec()) b[1] = b[2] = b[0];
    result[0] = a[0] != b[0] || a[1] != b[1] || a[2] != b[2];
}


void
SeExprLtNode::eval(SeVec3d& result) const
{
    SeVec3d a, b;
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    child0->eval(a);
    child1->eval(b);

    result[0] = a[0] < b[0];
}


void
SeExprGtNode::eval(SeVec3d& result) const
{
    SeVec3d a, b;
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    child0->eval(a);
    child1->eval(b);

    result[0] = a[0] > b[0];
}


void
SeExprLeNode::eval(SeVec3d& result) const
{
    SeVec3d a, b;
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    child0->eval(a);
    child1->eval(b);

    result[0] = a[0] <= b[0];
}


void
SeExprGeNode::eval(SeVec3d& result) const
{
    SeVec3d a, b;
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    child0->eval(a);
    child1->eval(b);

    result[0] = a[0] >= b[0];
}


void
SeExprAddNode::eval(SeVec3d& result) const
{
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    SeVec3d a, b;
    child0->eval(a);
    child1->eval(b);

    if (!_isVec) {
	result[0] = a[0] + b[0];
    }
    else {
	// at least one child is a vector and the result is too
	if (!child0->isVec()) a[1] = a[2] = a[0];
	if (!child1->isVec()) b[1] = b[2] = b[0];
	result = a + b;
    }
}


void
SeExprSubNode::eval(SeVec3d& result) const
{
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    SeVec3d a, b;
    child0->eval(a);
    child1->eval(b);

    if (!_isVec) {
	result[0] = a[0] - b[0];
    }
    else {
	// at least one child is a vector and the result is too
	if (!child0->isVec()) a[1] = a[2] = a[0];
	if (!child1->isVec()) b[1] = b[2] = b[0];
	result = a - b;
    }
}


void
SeExprMulNode::eval(SeVec3d& result) const
{
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    SeVec3d a, b;
    child0->eval(a);
    child1->eval(b);

    if (!_isVec) {
	result[0] = a[0] * b[0];
    }
    else {
	// at least one child is a vector and the result is too
	if (!child0->isVec()) a[1] = a[2] = a[0];
	if (!child1->isVec()) b[1] = b[2] = b[0];
	result = a * b;
    }
}


void
SeExprDivNode::eval(SeVec3d& result) const
{
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    SeVec3d a, b;
    child0->eval(a);
    child1->eval(b);

    if (!_isVec) {
	result[0] = a[0] / b[0];
    }
    else {
	// at least one child is a vector and the result is too
	if (!child0->isVec()) a[1] = a[2] = a[0];
	if (!child1->isVec()) b[1] = b[2] = b[0];
	result = a / b;
    }
}


static double niceMod(double a, double b)
{
    if (b == 0) return 0;
    return a - floor(a/b)*b;
}


void
SeExprModNode::eval(SeVec3d& result) const
{
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    SeVec3d a, b;
    child0->eval(a);
    child1->eval(b);

    if (!_isVec) {
	result[0] = niceMod(a[0], b[0]);
    }
    else {
	// at least one child is a vector and the result is too
	if (!child0->isVec()) a[1] = a[2] = a[0];
	if (!child1->isVec()) b[1] = b[2] = b[0];
	result[0] = niceMod(a[0], b[0]);
	result[1] = niceMod(a[1], b[1]);
	result[2] = niceMod(a[2], b[2]);
    }
}

void
SeExprExpNode::eval(SeVec3d& result) const
{
    const SeExprNode* child0 = child(0);
    const SeExprNode* child1 = child(1);
    SeVec3d a, b;
    child0->eval(a);
    child1->eval(b);

    if (!_isVec) {
        if(a[0]<0 && std::floor(b[0])!=b[0]) a[0]=0.;
	result[0] = std::pow(a[0], b[0]);
    }
    else {
	// at least one child is a vector and the result is too
	if (!child0->isVec()) a[1] = a[2] = a[0];
	if (!child1->isVec()) b[1] = b[2] = b[0];
        for(int k=0;k<3;k++) if(a[k]<0 && std::floor(b[k])!=b[k]) a[k]=0.;
        result[0] = std::pow(a[0], b[0]);
        result[1] = std::pow(a[1], b[1]);
        result[2] = std::pow(a[2], b[2]);
    }
}


bool
SeExprVarNode::prep(bool /*wantVec*/)
{
    // ask expression to resolve var
    _var = _expr->resolveLocalVar(name());
    if (!_var) _var = _expr->resolveVar(name());
    if (!_var) {
        addError(std::string("No variable named $")+name());
	return 0;
    }
    _isVec = _var->isVec();
    return 1;
}


void
SeExprVarNode::eval(SeVec3d& result) const
{
    if (_var) _var->eval(this, result);
    else result = 0.0;
}


bool
SeExprFuncNode::prep(bool wantVec)
{
    // ask expression to resolve func
    _func = _expr->resolveFunc(_name);
    // otherwise, look in global func table
    if (!_func) _func = SeExprFunc::lookup(_name);
    if (!_func) {
	addError("No function named "+_name);
	return 0;
    }

    _nargs = numChildren();
    if (_nargs < _func->minArgs()) {
	addError("Too few args for function "+_name);
	return 0;
    }
    if (_nargs > _func->maxArgs() && _func->maxArgs() >= 0) {
	addError("Too many args for function "+_name);
	return 0;
    }

    // alloc storage for args
    _scalarArgs.resize(_nargs);
    _vecArgs.resize(_nargs);

    // funcx is a catchall that does all its own processing
    if (_func->type() == SeExprFunc::FUNCX) {
	_isVec = 1; // assume vec result - funcx can override
        if(!_func->funcx()->isThreadSafe()) _expr->setThreadUnsafe(_name);
	return _func->funcx()->prep(this, wantVec);
    }

    // if a vector result is wanted or the function expects vector args,
    // then prepare the arguments for vector evaluation
    if (!SeExprNode::prep(wantVec || _func->hasVecArgs())){ return 0; }
    
    // a vector result will be produced if a vector result is wanted
    // and the either the function can produce a vector or the function
    // is purely scalar but one or more args are vectors.
    _isVec = 0;
    if (wantVec) {
	if (_func->isVec()) _isVec = 1;
	else if (!_func->hasVecArgs()) {
	    for (int i = 0; i < _nargs; i++)
		if (child(i)->isVec()) { _isVec = 1; break; }
	}
    }
    return 1;
}

SeVec3d*
SeExprFuncNode::evalArgs() const
{
    SeVec3d* a = vecArgs();
    for (int i = 0; i < _nargs; i++) {
	const SeExprNode* child = SeExprNode::child(i);
	SeVec3d& A = a[i];
	child->eval(A);
	if (!child->isVec()) A[1] = A[2] = A[0];
    }
    return a;
}

SeVec3d
SeExprFuncNode::evalArg(int n) const
{
    SeVec3d arg;
    const SeExprNode* child = SeExprNode::child(n);
    child->eval(arg);
    if (!child->isVec()) arg[1] = arg[2] = arg[0];
    return arg;
}

bool
SeExprFuncNode::isStrArg(int n) const
{
    return n < _nargs && dynamic_cast<const SeExprStrNode*>(child(n)) != 0;
}

std::string
SeExprFuncNode::getStrArg(int n) const
{
    if (n < _nargs)
	return static_cast<const SeExprStrNode*>(child(n))->str();
    return "";
}


void
SeExprFuncNode::eval(SeVec3d& result) const
{
    if (!_func) { result = 0.0; return; }

    // funcx is a catchall that does all its own processing
    if (_func->type() == SeExprFunc::FUNCX) {
	_func->funcx()->eval(this, result);
	return;
    }

    // handle the case of a scalar func applied to a vector
    bool applyScalarToVec = _isVec && !_func->isVec();
    int niter = applyScalarToVec ? 3 : 1;

    // eval args and call the function
    SeVec3d* a = evalArgs();
    for (int i = 0; i < niter; i++) {
	switch (_func->type()) {
	default: 
	    result[i] = result[1] = result[2] = 0;
	    break;
	case SeExprFunc::FUNC0:
	    result[i] = _func->func0()(); 
	    break;
	case SeExprFunc::FUNC1:
	    result[i] = _func->func1()(a[0][i]);
	    break;
	case SeExprFunc::FUNC2:
	    result[i] = _func->func2()(a[0][i], a[1][i]);
	    break;
	case SeExprFunc::FUNC3:
	    result[i] = _func->func3()(a[0][i], a[1][i], a[2][i]);
	    break;
	case SeExprFunc::FUNC4:
	    result[i] = _func->func4()(a[0][i], a[1][i], a[2][i], a[3][i]);
	    break;
	case SeExprFunc::FUNC5:
	    result[i] = _func->func5()(a[0][i], a[1][i], a[2][i], a[3][i],
				       a[4][i]);
	    break;
	case SeExprFunc::FUNC6:
	    result[i] = _func->func6()(a[0][i], a[1][i], a[2][i], a[3][i],
				       a[4][i], a[5][i]);
	    break;
	case SeExprFunc::FUNCN: 
	    {
		double* d = scalarArgs();
		for (int n = 0; n < _nargs; n++) d[n] = a[n][i];
		result[i] = _func->funcn()(_nargs, d);
		break;
	    }
	case SeExprFunc::FUNC1V:
	    result[i] = _func->func1v()(a[0]);
	    break;
	case SeExprFunc::FUNC2V:
	    result[i] = _func->func2v()(a[0], a[1]);
	    break;
	case SeExprFunc::FUNCNV: 
	    result[i] = _func->funcnv()(_nargs, a);
	    break;
	case SeExprFunc::FUNC1VV:
	    result = _func->func1vv()(a[0]);
	    break;
	case SeExprFunc::FUNC2VV:
	    result = _func->func2vv()(a[0], a[1]);
	    break;
	case SeExprFunc::FUNCNVV: 
	    result = _func->funcnvv()(_nargs, a);
	    break;
	}
    }

}
