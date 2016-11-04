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

#ifndef MAKEDEPEND
#include <math.h>
#include <sstream>
#endif
#include "Vec.h"
#include "ExprType.h"
#include "Expression.h"
#include "ExprEnv.h"
#include "ExprNode.h"
#include "ExprFunc.h"
#include "VarBlock.h"

// TODO: add and other binary op demote to scalar if wantScalar
// TODO: logical operations like foo<bar should they do vector returns... right now no... implicit demote
// TODO: local function evaluation
// TODO: buildInterpreter for higher level nodes so the return location can be routed back
// TODO: ExprFuncNode interpreter stuff
// TODO: check each node for possibility of strings

namespace SeExpr2 {

ExprNode::ExprNode(const Expression* expr) : _expr(expr), _parent(0), _isVec(0) {}

ExprNode::ExprNode(const Expression* expr, const ExprType& type) : _expr(expr), _parent(0), _isVec(0), _type(type) {}

ExprNode::ExprNode(const Expression* expr, ExprNode* a) : _expr(expr), _parent(0), _isVec(0) {
    _children.reserve(1);
    addChild(a);
}

ExprNode::ExprNode(const Expression* expr, ExprNode* a, const ExprType& type)
    : _expr(expr), _parent(0), _isVec(0), _type(type) {
    _children.reserve(1);
    addChild(a);
}

ExprNode::ExprNode(const Expression* expr, ExprNode* a, ExprNode* b) : _expr(expr), _parent(0), _isVec(0) {
    _children.reserve(2);
    addChild(a);
    addChild(b);
}

ExprNode::ExprNode(const Expression* expr, ExprNode* a, ExprNode* b, const ExprType& type)
    : _expr(expr), _parent(0), _isVec(0), _type(type) {
    _children.reserve(2);
    addChild(a);
    addChild(b);
}

ExprNode::ExprNode(const Expression* expr, ExprNode* a, ExprNode* b, ExprNode* c) : _expr(expr), _parent(0), _isVec(0) {
    _children.reserve(3);
    addChild(a);
    addChild(b);
    addChild(c);
}

ExprNode::ExprNode(const Expression* expr, ExprNode* a, ExprNode* b, ExprNode* c, const ExprType& type)
    : _expr(expr), _parent(0), _isVec(0), _type(type) {
    _children.reserve(3);
    addChild(a);
    addChild(b);
    addChild(c);
}

ExprNode::~ExprNode() {
    // delete children
    std::vector<ExprNode*>::iterator iter;
    for (iter = _children.begin(); iter != _children.end(); iter++) delete *iter;
}

void ExprNode::addChild(ExprNode* child) {
    _children.push_back(child);
    child->_parent = this;
}

void ExprNode::addChildren(ExprNode* surrogate) {
    std::vector<ExprNode*>::iterator iter;
    for (iter = surrogate->_children.begin(); iter != surrogate->_children.end(); iter++) {
        addChild(*iter);
    }
    surrogate->_children.clear();
    delete surrogate;
}

ExprType ExprNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    /** Default is to call prep on children (giving AnyType as desired type).
     *  If all children return valid types, returns NoneType.
     *  Otherwise,                          returns ErrorType.
     *  *Note:* Ignores wanted type.
     */
    bool error = false;

    _maxChildDim = 0;
    for (int c = 0; c < numChildren(); c++) {
        error |= !child(c)->prep(false, envBuilder).isValid();
        int childDim = child(c)->type().isFP() ? child(c)->type().dim() : 0;
        if (childDim > _maxChildDim) _maxChildDim = childDim;
    }

    if (error)
        setType(ExprType().Error());
    else
        setTypeWithChildLife(ExprType().None());

    return _type;
}

ExprType ExprModuleNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    bool error = false;

    for (int c = 0; c < numChildren(); c++) error |= !child(c)->prep(false, envBuilder).isValid();
    if (error)
        setType(ExprType().Error());
    else
        setType(child(numChildren() - 1)->type());

    return _type;
}

ExprType ExprPrototypeNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    // TODO: implement prototype
    bool error = false;
    checkCondition(false, "Prototypes are currently not supported", error);
    return ExprType().Error();
#if 0
    bool error = false;

    if (_retTypeSet) checkCondition(returnType().isValid(), "Function has bad return type", error);

    _argTypes.clear();
    for (int c = 0; c < numChildren(); c++) {
        ExprType type = child(c)->type();
        checkCondition(type.isValid(), "Function has a parameter with a bad type", error);
        _argTypes.push_back(type);
        ExprLocalVar* localVar = new ExprLocalVar(type);
        envBuilder.current()->add(((ExprVarNode*)child(c))->name(), localVar);
        std::cerr << "after create localvar phi " << localVar->getPhi() << std::endl;
        child(c)->prep(wantScalar, envBuilder);
    }

    if (error)
        setType(ExprType().Error());
    else
        setType(ExprType().None().Varying());

    return _type;
#endif
}

void ExprPrototypeNode::addArgTypes(ExprNode* surrogate) {
    ExprNode::addChildren(surrogate);

    ExprType type;
    for (int i = 0; i < numChildren(); i++) _argTypes.push_back(child(i)->type());
}

void ExprPrototypeNode::addArgs(ExprNode* surrogate) {
    ExprNode::addChildren(surrogate);
#if 0
    ExprNode * child;
    ExprType type;
    for(int i = 0; i < numChildren(); i++) {
        child = this->child(i);
        type = child->type();

        _argTypes.push_back(type);
        _env.add(((ExprVarNode*)child)->name(), new ExprLocalVar(type));
    }
#endif
}

ExprType ExprLocalFunctionNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
#if 0  // TODO: no local functions for now
    bool error = false;

    // prep prototype and check for errors
    ExprPrototypeNode* prototype = (ExprPrototypeNode*)child(0);
    ExprVarEnv functionEnv;
    functionEnv.resetAndSetParent(&env);
    if (!prototype->prep(false, functionEnv).isValid()) error = true;

    // decide what return type we want
    bool returnWantsScalar = false;
    if (!error && prototype->isReturnTypeSet()) returnWantsScalar = prototype->returnType().isFP(1);

    // prep block and check for errors
    ExprNode* block = child(1);
    ExprType blockType = block->prep(returnWantsScalar, functionEnv);

    if (!error && blockType.isValid()) {
        if (prototype->isReturnTypeSet()) {
            if (blockType != prototype->returnType()) {
                checkCondition(false,
                               "In function result of block '" + blockType.toString() +
                                   "' does not match given return type " + prototype->returnType().toString(),
                               error);
            }

        } else
            prototype->setReturnType(blockType);
        // register the function in the symbol table

        env.addFunction(prototype->name(), this);
    } else {
        checkCondition(false, "Invalid type for blockType is " + blockType.toString(), error);
        error = true;
    }
    return _type = error ? ExprType().Error() : ExprType().None().Varying();
#else
    bool error = false;
    checkCondition(false, "Local functions are currently not supported.", error);
    return ExprType().Error();
#endif
}

// TODO: write buildInterpreter for local function node
ExprType ExprLocalFunctionNode::prep(ExprFuncNode* callerNode, bool scalarWanted, ExprVarEnvBuilder& envBuilder) const {
#if 0
    bool error = false;
    callerNode->checkCondition(callerNode->numChildren() == prototype()->numChildren(),
                               "Incorrect number of arguments to function call",
                               error);
    for (int i = 0; i < callerNode->numChildren(); i++) {
        // TODO: is this right?
        // bool compatible=ExprType::valuesCompatible(callerNode->child(i)->prep(false,env), prototype()->argType(i));
        if (!callerNode->checkArg(i, prototype()->argType(i), envBuilder)) error = true;
        // callerNode->child(i)->checkCondition(compatible,"Incorrect type for argument",error);
    }
    return error ? ExprType().Error() : prototype()->returnType();
#else
    bool error = false;
    callerNode->checkCondition(false, "Local functions are currently not supported.", error);
    return ExprType().Error();
#endif
}

ExprType ExprBlockNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    ExprType assignType = child(0)->prep(false, envBuilder);
    ExprType resultType = child(1)->prep(wantScalar, envBuilder);

    if (!assignType.isValid())
        setType(ExprType().Error());
    else
        setType(resultType);

    return _type;
}

ExprType ExprIfThenElseNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    ExprType condType, thenType, elseType;

    bool error = false;

    condType = child(0)->prep(true, envBuilder);
    checkIsFP(condType, error);

    ExprVarEnv* parentEnv = envBuilder.current();
    ExprVarEnv* thenEnv = envBuilder.createDescendant(parentEnv);
    ExprVarEnv* elseEnv = envBuilder.createDescendant(parentEnv);
    envBuilder.setCurrent(thenEnv);
    thenType = child(1)->prep(false, envBuilder);
    thenEnv = envBuilder.current();
    envBuilder.setCurrent(elseEnv);
    elseType = child(2)->prep(false, envBuilder);
    elseEnv = envBuilder.current();

    if (!error && thenType.isValid() && elseType.isValid()) {
        ExprVarEnv* newEnv = envBuilder.createDescendant(parentEnv);
        _varEnvMergeIndex = newEnv->mergeBranches(condType, *thenEnv, *elseEnv);
        envBuilder.setCurrent(newEnv);
        // TODO: aselle insert the phi nodes!
    } else {
        envBuilder.setCurrent(parentEnv);  // since the conditionals broke don't include them in new environment
        error = true;
    }
    _varEnv = envBuilder.current();

    if (error)
        setType(ExprType().Error());
    else
        setType(ExprType().None().setLifetime(condType, thenType, elseType));

    return _type;
}

ExprType ExprAssignNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    _assignedType = child(0)->prep(false, envBuilder);

    std::unique_ptr<ExprLocalVar> localVar(new ExprLocalVar(child(0)->type()));
    _localVar = localVar.get();
    envBuilder.current()->add(_name, std::move(localVar));
    bool error = false;
    checkCondition(
        _assignedType.isValid(), std::string("Assignment operation has bad type: ") + _type.toString(), error);

    if (error)
        setType(ExprType().Error());
    else
        setTypeWithChildLife(ExprType().None());
    return _type;
}

ExprType ExprVecNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    bool error = false;

    int max_child_d = 0;
    for (int c = 0; c < numChildren(); c++) {
        ExprType childType = child(c)->prep(true, envBuilder);
        // TODO: add way to tell what element of vector has the type mismatch
        checkIsFP(childType, error);
        max_child_d = std::max(max_child_d, childType.dim());
    }

    if (error)
        setType(ExprType().Error());
    else
        setTypeWithChildLife(ExprType().FP(numChildren()));
    return _type;
}

Vec3d ExprVecNode::value() const {
    if (const ExprNumNode* f = dynamic_cast<const ExprNumNode*>(child(0))) {
        double first = f->value();
        if (const ExprNumNode* s = dynamic_cast<const ExprNumNode*>(child(1))) {
            double second = s->value();
            if (const ExprNumNode* t = dynamic_cast<const ExprNumNode*>(child(2))) {
                double third = t->value();
                return Vec3d(first, second, third);
            };
        };
    };

    return Vec3d(0.0);
};

ExprType ExprUnaryOpNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    bool error = false;

    // TODO: aselle may want to implicitly demote to FP[1] if wantScalar is true!
    ExprType childType = child(0)->prep(wantScalar, envBuilder);
    checkIsFP(childType, error);
    if (error)
        setType(ExprType().Error());
    else
        setType(childType);
    return _type;
}

ExprType ExprCondNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    // TODO: determine if extra environments are necessary, currently not included
    ExprType condType, thenType, elseType;

    bool error = false;

    condType = child(0)->prep(true, envBuilder);

    checkIsFP(condType, error);

    thenType = child(1)->prep(wantScalar, envBuilder);
    elseType = child(2)->prep(wantScalar, envBuilder);

    checkIsValue(thenType, error);
    checkIsValue(elseType, error);
    checkCondition(ExprType::valuesCompatible(thenType, elseType), "Types of conditional are not compatible", error);

    if (error)
        setType(ExprType().Error());
    else {
        if (thenType.isString())
            setType(thenType);
        else
            setType(thenType.isFP(1) ? elseType : thenType);
        _type.setLifetime(condType, thenType, elseType);
    }

    return _type;
}

ExprType ExprSubscriptNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    // TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    ExprType vecType, scriptType;

    bool error = false;

    vecType = child(0)->prep(false, envBuilder);  // want scalar is false because we aren't just doing foo[0]
    checkIsFP(vecType, error);

    scriptType = child(1)->prep(true, envBuilder);
    checkIsFP(scriptType, error);

    if (error)
        setType(ExprType().Error());
    else
        setType(ExprType().FP(1).setLifetime(vecType, scriptType));

    return _type;
}

ExprType ExprCompareEqNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    // TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    ExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(false, envBuilder);
    checkIsValue(firstType, error);
    secondType = child(1)->prep(false, envBuilder);
    checkIsValue(secondType, error);

    if (firstType.isValid() && secondType.isValid()) checkTypesCompatible(firstType, secondType, error);

    if (error)
        setType(ExprType().Error());
    else
        setType(ExprType().FP(1).setLifetime(firstType, secondType));

    return _type;
}

ExprType ExprCompareNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    // TODO: assume we want scalar
    // TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    ExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(true, envBuilder);
    checkIsFP(firstType, error);
    secondType = child(1)->prep(true, envBuilder);
    checkIsFP(secondType, error);

    if (firstType.isValid() && secondType.isValid()) checkTypesCompatible(firstType, secondType, error);

    if (error)
        setType(ExprType().Error());
    else
        setType(ExprType().FP(1).setLifetime(firstType, secondType));

    return _type;
}

ExprType ExprBinaryOpNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    // TODO: aselle this probably should set the type to be FP1 if wantScalar is true!
    // TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    ExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(false, envBuilder);
    checkIsFP(firstType, error);
    secondType = child(1)->prep(false, envBuilder);
    checkIsFP(secondType, error);
    checkTypesCompatible(firstType, secondType, error);

    if (error)
        setType(ExprType().Error());
    else
        setType((firstType.isFP(1) ? secondType : firstType).setLifetime(firstType, secondType));

    return _type;
}

ExprType ExprVarNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    // ask expression to resolve var
    bool error = false;
    if ((_localVar = envBuilder.current()->find(name()))) {
        if (_localVar->type().isError()) {
            /// Some friendlier error suggestions
            if (ExprLocalVarPhi* phi = dynamic_cast<ExprLocalVarPhi*>(_localVar)) {
                if (!phi->_thenVar->type().isError() && !phi->_elseVar->type().isError()) {
                    addError(std::string("Variable ") + name() + " defined in conditionals inconsistently.");
                }
            }
        }
        setType(_localVar->type());
        return _type;
    } else {
        // user defined external variable
        _var = _expr->resolveVar(name());
        if (!_var) {
            if (const VarBlockCreator* creator = _expr->varBlockCreator()) {
                // data block defined external var
                _var = creator->resolveVar(name());
            }
        }
        if (_var) {
            _expr->addVar(name());  // register used variable so _expr->usedVar() works
            setType(_var->type());
            return _type;
        }
    }
    // If we get here we do not have a variable!
    checkCondition(_var || _localVar, std::string("No variable named ''") + name() + "'", error);
    setType(ExprType().Error());
    return _type;
}

ExprType ExprNumNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    _type = ExprType().FP(1).Constant();
    return _type;
}

ExprType ExprStrNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    _type = ExprType().String().Constant();
    return _type;
}

ExprType ExprFuncNode::prep(bool wantScalar, ExprVarEnvBuilder& envBuilder) {
    bool error = false;

    int nargs = numChildren();
    _promote.resize(nargs, 0);

    // find function using per-expression callback and then global table
    // TODO: put lookup of local functions here
    _func = 0;
    if (ExprLocalFunctionNode* localFunction = envBuilder.current()->findFunction(_name)) {
        _localFunc = localFunction;
        setTypeWithChildLife(localFunction->prep(this, wantScalar, envBuilder));
        // TODO: we need to type check arguments here
    } else {
        if (!_func) _func = _expr->resolveFunc(_name);
        if (!_func) _func = ExprFunc::lookup(_name);

        // check that function exists and that the function has the right number of arguments
        if (checkCondition(_func, "Function " + _name + " has no definition", error) &&
            checkCondition(nargs >= _func->minArgs(), "Too few args for function" + _name, error) &&
            checkCondition(
                nargs <= _func->maxArgs() || _func->maxArgs() < 0, "Too many args for function " + _name, error)) {

            const ExprFuncX* funcx = _func->funcx();
            ExprType type = funcx->prep(this, wantScalar, envBuilder);
            setTypeWithChildLife(type);
        } else {                                // didn't match num args or function not found
            ExprNode::prep(false, envBuilder);  // prep arguments anyways to catch as many errors as possible!
            setTypeWithChildLife(ExprType().Error());
        }
    }

    return _type;
}

int ExprFuncNode::buildInterpreter(Interpreter* interpreter) const {
    if (_localFunc)
        return _localFunc->buildInterpreterForCall(this, interpreter);
    else if (_func)
        return _func->funcx()->buildInterpreter(this, interpreter);

    assert(false);
    return 0;
}

bool ExprFuncNode::checkArg(int arg, ExprType type, ExprVarEnvBuilder& envBuilder) {
    ExprType childType = child(arg)->prep(type.isFP(1), envBuilder);
    _promote[arg] = 0;
    if (ExprType::valuesCompatible(type, childType) && type.isLifeCompatible(childType)) {
        if (type.isFP() && type.dim() > childType.dim()) {
            _promote[arg] = type.dim();
        }
        return true;
    }
    child(arg)->addError("Expected " + type.toString() + " for argument, got " + childType.toString());
    return false;
}
}
