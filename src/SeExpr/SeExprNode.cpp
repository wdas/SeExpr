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

#ifndef MAKEDEPEND
#include <math.h>
#include <sstream>
#endif
#include "SeVec3d.h"
#include "SeExprType.h"
#include "SeExpression.h"
#include "SeExprEnv.h"
#include "SeExprNode.h"
#include "SeExprFunc.h"


SeExprNode::SeExprNode(const SeExpression* expr)
    : _expr(expr), _parent(0), _isVec(0)
{
}


SeExprNode::SeExprNode(const SeExpression* expr,
                       const SeExprType & type)
    : _expr(expr), _parent(0), _isVec(0), _type(type)
{
}


SeExprNode::SeExprNode(const SeExpression* expr,
                       SeExprNode* a)
    : _expr(expr), _parent(0), _isVec(0)
{
    _children.reserve(1);
    addChild(a);
}


SeExprNode::SeExprNode(const SeExpression* expr,
                       SeExprNode* a,
                       const SeExprType & type)
    : _expr(expr), _parent(0), _isVec(0), _type(type)
{
    _children.reserve(1);
    addChild(a);
}


SeExprNode::SeExprNode(const SeExpression* expr,
                       SeExprNode* a,
                       SeExprNode* b)
    : _expr(expr), _parent(0), _isVec(0)
{
    _children.reserve(2);
    addChild(a);
    addChild(b);
}


SeExprNode::SeExprNode(const SeExpression* expr,
                       SeExprNode* a,
                       SeExprNode* b,
                       const SeExprType & type)
    : _expr(expr), _parent(0), _isVec(0), _type(type)
{
    _children.reserve(2);
    addChild(a);
    addChild(b);
}


SeExprNode::SeExprNode(const SeExpression* expr,
                       SeExprNode* a,
                       SeExprNode* b,
		       SeExprNode* c)
    : _expr(expr), _parent(0), _isVec(0)
{
    _children.reserve(3);
    addChild(a);
    addChild(b);
    addChild(c);
}


SeExprNode::SeExprNode(const SeExpression* expr,
                       SeExprNode* a,
                       SeExprNode* b,
		       SeExprNode* c,
                       const SeExprType & type)
    : _expr(expr), _parent(0), _isVec(0), _type(type)
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


void
SeExprNode::addChildren_without_delete(SeExprNode* surrogate)
{
    std::vector<SeExprNode*>::iterator iter;
    for (iter = surrogate->_children.begin();
	 iter != surrogate->_children.end();
	 iter++)
    {
	addChild(*iter);
    }
    surrogate->_children.clear();
}


SeExprType
SeExprNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    /** The default behavior is to call prep on children (giving AnyType as desired type).
     *  If all children return valid types, returns NoneType.
     *  Otherwise,                          returns ErrorType.
     *  *Note:* Ignores wanted type.
     */
    bool error=false;

    for(int c=0;c<numChildren();c++)
        error |= !child(c)->prep(false, env).isValid();

    if(error) setType(SeExprType().Error());
    else setTypeWithChildLife(SeExprType().None());

    return _type;
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


SeExprType
SeExprModuleNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    for(int c=0;c<numChildren();c++)
        error |= !child(c)->prep(false, env).isValid();
    if(error) setType(SeExprType().Error());
    else setType(child(numChildren()-1)->type());

    return _type;
}


void
SeExprModuleNode::eval(SeVec3d& result) const
{
    // eval block, then eval primary expr
    SeVec3d val;
    child(0)->eval(val);
    child(1)->eval(result);
}


SeExprType
SeExprPrototypeNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    if(_retTypeSet)
        checkCondition(returnType().isValid(), "Function has bad return type", error);

    for(int c = 0; c < numChildren(); c++)
        checkCondition(child(c)->type().isValid(), "Function has a parameter with a bad type", error);

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().None().Varying());

    return _type;
}


void
SeExprPrototypeNode::addArgTypes(SeExprNode* surrogate)
{
    SeExprNode::addChildren(surrogate);

    SeExprType type;
    for(int i = 0; i < numChildren(); i++)
        _argTypes.push_back(child(i)->type());
}


void
SeExprPrototypeNode::addArgs(SeExprNode* surrogate)
{
    SeExprNode::addChildren(surrogate);

    SeExprNode * child;
    SeExprType type;
    for(int i = 0; i < numChildren(); i++) {
        child = this->child(i);
        type = child->type();

        _argTypes.push_back(type);
        _env.add(((SeExprVarNode*)child)->name(), new SeExprLocalVarRef(type));
    }
}


void
SeExprPrototypeNode::eval(SeVec3d& result) const
{
    // eval block, then eval primary expr
    SeVec3d val;
    child(0)->eval(val);
    child(1)->eval(result);
}


SeExprType
SeExprLocalFunctionNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    //prep prototype and check for errors 
    SeExprPrototypeNode* prototype=(SeExprPrototypeNode*)child(0);
    SeExprVarEnv ignoredEnv;
    if(!prototype->prep(false, ignoredEnv).isValid())
        error = true;

    // decide what return type we want
    bool returnWantsScalar=false;
    if(!error && prototype->isReturnTypeSet())
        returnWantsScalar = prototype->returnType().isFP(1);

    //prep block and check for errors
    SeExprNode* block = child(1);
    SeExprType blockType = block->prep(returnWantsScalar, prototype->env());

    if(!error && blockType.isValid()) {
        if(prototype->isReturnTypeSet())
            checkCondition(blockType==prototype->returnType(), "In function result of block does not match given return type", error);
        else prototype->setReturnType(blockType);
    } else error = true;

    return _type = error ? SeExprType().Error() : SeExprType().None().Varying();
}


void
SeExprLocalFunctionNode::eval(SeVec3d& result) const
{
    // eval block, then eval primary expr
    SeVec3d val;
    child(0)->eval(val);
    child(1)->eval(result);
}


SeExprType
SeExprBlockNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    SeExprType assignType = child(0)->prep(false, env);
    SeExprType resultType = child(1)->prep(wantScalar, env);

    if(!assignType.isValid()) setType(SeExprType().Error());
    else setType(resultType);

    return _type;
}


void
SeExprBlockNode::eval(SeVec3d& result) const
{
    // eval block, then eval primary expr
    SeVec3d val;
    child(0)->eval(val);
    child(1)->eval(result);
}


SeExprType
SeExprIfThenElseNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    SeExprVarEnv           thenEnv,  elseEnv;
    SeExprType   condType, thenType, elseType;

    bool error = false;

    condType = child(0)->prep(true,env);
    checkIsFP(condType,error);

    thenEnv = SeExprVarEnv(env);
    thenType = child(1)->prep(false, thenEnv);
    elseEnv = SeExprVarEnv(env);
    elseType = child(2)->prep(false, elseEnv);

    if(!error && thenType.isValid() && elseType.isValid()
        && checkCondition(SeExprVarEnv::branchesMatch(thenEnv, elseEnv), "Types of variables do not match after if statement", error))
        env.add(thenEnv, condType);
    else error = true;

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().None().setLifetime(condType,thenType,elseType));

    return _type;
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


SeExprType
SeExprAssignNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    _assignedType = child(0)->prep(false, env);

    //TODO: This could add errors to the variable environment
    env.add(_name, new SeExprLocalVarRef(_assignedType));
    bool error=false;
	// TODO: fix
    //checkIsValue(_assignedType,error);
	checkCondition(_assignedType.isValid(),"tesT",error);

    if(error) setType(SeExprType().Error());
    else setTypeWithChildLife(SeExprType().None());
    return _type;
}


void
SeExprAssignNode::eval(SeVec3d& result) const
{
    if (_var) {
	// eval expression and store in variable
	const SeExprNode* node = child(0);
	node->eval(_var->val);
        //assume that eval made the correct assignment
	//if (_var->isVec() && !node->isVec())
        //_var->val[1] = _var->val[2] = _var->val[0];
    }
    else result = 0.0;
}


SeExprType
SeExprVecNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    for(int c=0; c<numChildren(); c++) {
        SeExprType childType = child(c)->prep(true, env);
        //TODO: add way to tell what element of vector has the type mismatch
        checkIsFP(childType,error);
    }
    
    if(error) setType(SeExprType().Error());
    else setTypeWithChildLife(SeExprType().FP(numChildren()));
    return _type;
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


SeVec3d
SeExprVecNode::value() const {
    if(const SeExprNumNode* f = dynamic_cast<const SeExprNumNode*>(child(0))) {
        double first = f->value();
        if(const SeExprNumNode* s = dynamic_cast<const SeExprNumNode*>(child(1))) {
            double second = s->value();
            if(const SeExprNumNode* t = dynamic_cast<const SeExprNumNode*>(child(2))) {
                double third = t->value();
                return SeVec3d(first, second, third);
            };
        };
    };

    return SeVec3d(0.0);
};


SeExprType
SeExprUnaryOpNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    SeExprType childType=child(0)->prep(wantScalar, env);
    checkIsFP(childType,error);
    if(error) setType(SeExprType().Error());
    else setType(childType);
    return _type;
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


// TODO: start here

SeExprType
SeExprCondNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    //TODO: determine if extra environments are necessary, currently not included
    SeExprType condType, thenType, elseType;

    bool error = false;

    condType = child(0)->prep(true,env);

    checkIsFP(condType, error);

    thenType = child(1)->prep(wantScalar, env);
    elseType = child(2)->prep(wantScalar, env);

    checkIsValue(thenType,error);
    checkIsValue(elseType,error);
    checkCondition(thenType == elseType, "Types of conditional branches do not match", error);

    if(error) setType(SeExprType().Error());
    else setType(thenType.setLifetime(condType,thenType,elseType));

    return _type;
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


SeExprType
SeExprLogicalOpNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    //TODO: determine if extra environments are necessary, currently not included
    SeExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(true, env);
    checkIsFP(firstType,error);
    secondType = child(1)->prep(true, env);
    checkIsFP(secondType,error);

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().FP(1).setLifetime(firstType,secondType));

    return _type;
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


SeExprType
SeExprSubscriptNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    //TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    SeExprType vecType, scriptType;

    bool error = false;

    vecType = child(0)->prep(false, env);
    checkIsFP(vecType,error);

    scriptType = child(1)->prep(true, env);
    checkIsFP(scriptType,error);

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().FP(1).setLifetime(vecType,scriptType));

    return _type;
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


SeExprType
SeExprCompareEqNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    //TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    SeExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(false, env);
    checkIsValue(firstType,error);
    secondType = child(1)->prep(false, env);
    checkIsValue(secondType,error);

    if(firstType.isValid() && secondType.isValid())
        checkTypesCompatible(firstType, secondType, error);

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().FP(1).setLifetime(firstType,secondType));

    return _type;
}


//TODO: make sure eval handles both scalars and n-length vectors
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


//TODO: make sure eval handles both scalars and n-length vectors
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


SeExprType
SeExprCompareNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    //TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    SeExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(false, env);
    checkIsFP(firstType,error);
    secondType = child(1)->prep(false, env);
    checkIsFP(secondType,error);

    if(firstType.isValid() && secondType.isValid())
        checkTypesCompatible(firstType, secondType, error);

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().FP(1).setLifetime(firstType,secondType));

    return _type;
}


//TODO: make sure eval handles both scalars and n-length vectors
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


//TODO: make sure eval handles both scalars and n-length vectors
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


//TODO: make sure eval handles both scalars and n-length vectors
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


//TODO: make sure eval handles both scalars and n-length vectors
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


SeExprType
SeExprBinaryOpNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    //TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    SeExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(false, env);
    checkIsFP(firstType,error);
    secondType = child(1)->prep(false, env);
    checkIsFP(secondType,error);
    checkTypesCompatible(firstType, secondType, error);

    if(error) setType(SeExprType().Error());
    else setType((firstType.isFP(1) ? secondType : firstType)
        .setLifetime(firstType,secondType));

    return _type;
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
	result[0] = pow(a[0], b[0]);
    }
    else {
	// at least one child is a vector and the result is too
	if (!child0->isVec()) a[1] = a[2] = a[0];
	if (!child1->isVec()) b[1] = b[2] = b[0];
	result[0] = pow(a[0], b[0]);
	result[1] = pow(a[1], b[1]);
	result[2] = pow(a[2], b[2]);
    }
}


SeExprType
SeExprVarNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    // ask expression to resolve var
    _var = env.find(name());

    if(!_var)
        _var = _expr->resolveVar(name());

    bool error=false;
    checkCondition(_var, std::string("No variable named $") + name(),error);
    // TODO: remove
    //std::cerr<<"we have gah is "<<_var->type().toString();
    return _type=error?SeExprType().Error():_var->type();
}


void
SeExprVarNode::eval(SeVec3d& result) const
{
    if (_var) _var->eval(this, result);
    else result = 0.0;
}


SeExprType
SeExprNumNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    _type=SeExprType().FP(1).Constant();
    return _type;
}


SeExprType
SeExprStrNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    _type=SeExprType().String().Constant();
    return _type;
}

bool
SeExprFuncNode::prepArgs(std::string const & name, bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    for(int c=0;c<numChildren();c++){
        // TODO: this is not necesssarily right
        SeExprType childType = child(c)->prep(false, env);
        // TODO: this is not necessarily right, you probably want to type check against signature!!!
        checkIsValue(childType,error);
        //TODO: give actual name, not placeholder - or take out name altogether
    }

    return !error; //return convention is true if no errors
}

SeExprType
SeExprFuncNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;
    int  _dim  = 1; //used for promotion checking

    _nargs = numChildren();

    _vecArgs.resize(_nargs);
    _scalarArgs.resize(_nargs);

    // ask expression to resolve func
    _func = _expr->resolveFunc(_name);
    // otherwise, look in global func table
    if (!_func) _func = SeExprFunc::lookup(_name);

    //check that function exisits and that the function has the right number of arguments
    if(checkCondition(_func,                        "Function " + _name + " has no definition", error) &&
       checkCondition(_nargs >= _func->minArgs(),   "Too few args for function "  + _name,     error) &&
       checkCondition(_nargs <= _func->maxArgs() ||
                         0  > _func->maxArgs(),   "Too many args for function " + _name,     error)) {
        //check if the fuction is a FuncX
        if(_func->type() == SeExprFunc::FUNCX) {
            //FuncX function:
            if(!_func->funcx()->isThreadSafe())                    _expr->setThreadUnsafe(_name);
            if(!_func->funcx()->prep(this, wantScalar, env).isValid()) error = true;
            //TODO: Make sure FuncX puts proper return type in retType(), NOT in _type - this may be a change in convention
        }
        else {
            //standard function:
            //check if arguments have errors
            error = !prepArgs(_name,false,env);

            if(!error                   && //no errors
               _func->isScalar()        && //takes scalar arguments only
                _func->retType().isFP(1))   //returns a scalar
                for(int i = 0; i < _nargs; i++) {
                    int cdim = child(i)->type().dim();

                    //Note: This assumes that every type (except FPN types) have dim() == 1
                    if(checkCondition(cdim == 1    ||
                                    _dim == 1    ||
                                    _dim == cdim,   "Arguments to promotable function, " + _name + ", are of different lengths", error))
                        if(cdim  > 1 &&
                           _dim == 1)
                            _dim = cdim;
                }
        }
    }else{
        SeExprNode::prep(false,env);
    }

    if(error)
        setType(SeExprType().Error());
    else if(_func->type() == SeExprFunc::FUNCX) //FuncX function
        setTypeWithChildLife(_func->retType());
    else  //standard function
        setTypeWithChildLife(SeExprType().FP(_dim).Varying());

    return _type;
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
    bool applyScalarToVec = _isVec && _func->isScalar();
    //bool applyScalarToVec = _isVec && !_func->isVec();
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
