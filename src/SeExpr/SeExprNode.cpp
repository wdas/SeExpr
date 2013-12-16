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


//TODO: add and other binary op demote to scalar if wantScalar
//TODO: logical operations like foo<bar should they do vector returns... right now no... implicit demote
//TODO: local function evaluation
//TODO: buildInterpreter for higher level nodes so the return location can be routed back
//TODO: SeExprFuncNode interpreter stuff
//TODO: check each node for possibility of strings




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

    _maxChildDim=0;
    for(int c=0;c<numChildren();c++){
        error |= !child(c)->prep(false, env).isValid();
        int childDim=child(c)->type().isFP() ? child(c)->type().dim() : 0;
        if(childDim>_maxChildDim) _maxChildDim=childDim;
    }

    if(error) setType(SeExprType().Error());
    else setTypeWithChildLife(SeExprType().None());

    return _type;
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


SeExprType
SeExprPrototypeNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    if(_retTypeSet)
        checkCondition(returnType().isValid(), "Function has bad return type", error);

    _argTypes.clear();
    for(int c = 0; c < numChildren(); c++){
        SeExprType type=child(c)->type();
        checkCondition(type.isValid(), "Function has a parameter with a bad type", error);
        _argTypes.push_back(type);
        env.add(((SeExprVarNode*)child(c))->name(), new SeExprLocalVar(type));
    }

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
#if 0
    SeExprNode * child;
    SeExprType type;
    for(int i = 0; i < numChildren(); i++) {
        child = this->child(i);
        type = child->type();

        _argTypes.push_back(type);
        _env.add(((SeExprVarNode*)child)->name(), new SeExprLocalVar(type));
    }
#endif
}



SeExprType
SeExprLocalFunctionNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    //prep prototype and check for errors 
    SeExprPrototypeNode* prototype=(SeExprPrototypeNode*)child(0);
    SeExprVarEnv functionEnv;
    functionEnv.resetAndSetParent(&env);
    if(!prototype->prep(false, functionEnv).isValid())
        error = true;

    // decide what return type we want
    bool returnWantsScalar=false;
    if(!error && prototype->isReturnTypeSet())
        returnWantsScalar = prototype->returnType().isFP(1);

    //prep block and check for errors
    SeExprNode* block = child(1);
    SeExprType blockType = block->prep(returnWantsScalar, functionEnv);

    if(!error && blockType.isValid()) {
        if(prototype->isReturnTypeSet())
            checkCondition(blockType==prototype->returnType(), "In function result of block does not match given return type", error);
        else prototype->setReturnType(blockType);
        // register the function in the symbol table
        env.addFunction(prototype->name(), this);
    } else error = true;

    return _type = error ? SeExprType().Error() : SeExprType().None().Varying();
}


// TODO: write buildInterpreter for local function node



SeExprType
SeExprBlockNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    SeExprType assignType = child(0)->prep(false, env);
    SeExprType resultType = child(1)->prep(wantScalar, env);

    if(!assignType.isValid()) setType(SeExprType().Error());
    else setType(resultType);

    return _type;
}


SeExprType
SeExprIfThenElseNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    SeExprType   condType, thenType, elseType;

    bool error = false;

    condType = child(0)->prep(true,env);
    checkIsFP(condType,error);

    thenEnv.resetAndSetParent(&env);
    elseEnv.resetAndSetParent(&env);

    thenType = child(1)->prep(false, thenEnv);
    elseType = child(2)->prep(false, elseEnv);

    if(!error && thenType.isValid() && elseType.isValid()){
        env.mergeBranches(condType,thenEnv,elseEnv);
        // TODO: aselle insert the phi nodes!
    }else error = true;

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().None().setLifetime(condType,thenType,elseType));

    return _type;
}

SeExprType
SeExprAssignNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    _assignedType = child(0)->prep(false, env);

    //TODO: This could add errors to the variable environment
    _localVar=new SeExprLocalVar(child(0)->type());
    env.add(_name, _localVar);
    bool error=false;
	// TODO: fix
    //checkIsValue(_assignedType,error);
	checkCondition(_assignedType.isValid(),"tesT",error);

    if(error) setType(SeExprType().Error());
    else setTypeWithChildLife(SeExprType().None());
    return _type;
}

SeExprType
SeExprVecNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    int max_child_d=0;
    for(int c=0; c<numChildren(); c++) {
        SeExprType childType = child(c)->prep(true, env);
        //TODO: add way to tell what element of vector has the type mismatch
        checkIsFP(childType,error);
        max_child_d=std::max(max_child_d,childType.dim());
    }
    
    if(error) setType(SeExprType().Error());
    else setTypeWithChildLife(SeExprType().FP(numChildren()));
    return _type;
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

    // TODO: aselle may want to implicitly demote to FP[1] if wantScalar is true!
    SeExprType childType=child(0)->prep(wantScalar, env);
    checkIsFP(childType,error);
    if(error) setType(SeExprType().Error());
    else setType(childType);
    return _type;
}

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


SeExprType
SeExprSubscriptNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    //TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    SeExprType vecType, scriptType;

    bool error = false;

    vecType = child(0)->prep(false, env); // want scalar is false because we aren't just doing foo[0]
    checkIsFP(vecType,error);

    scriptType = child(1)->prep(true, env);
    checkIsFP(scriptType,error);

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().FP(1).setLifetime(vecType,scriptType));

    return _type;
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


SeExprType
SeExprCompareNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    // TODO: assume we want scalar
    //TODO: double-check order of evaluation - order MAY effect environment evaluation (probably not, though)
    SeExprType firstType, secondType;

    bool error = false;

    firstType = child(0)->prep(true, env);
    checkIsFP(firstType,error);
    secondType = child(1)->prep(true, env);
    checkIsFP(secondType,error);

    if(firstType.isValid() && secondType.isValid())
        checkTypesCompatible(firstType, secondType, error);

    if(error) setType(SeExprType().Error());
    else setType(SeExprType().FP(1).setLifetime(firstType,secondType));

    return _type;
}


SeExprType
SeExprBinaryOpNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    // TODO: aselle this probably should set the type to be FP1 if wantScalar is true!
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

SeExprType
SeExprVarNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    // ask expression to resolve var
    bool error=false;
    if((_localVar = env.find(name()))){
        setType(_localVar->type());
    }else if((_var = _expr->resolveVar(name()))){
        setType(_var->type());
    }else{
        checkCondition(_var || _localVar, std::string("No variable named $") + name(),error);
        setType(SeExprType().Error());
    }
    return _type;
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

SeExprType
SeExprFuncNode::prep(bool wantScalar, SeExprVarEnv & env)
{
    bool error = false;

    int nargs=numChildren();
    _promote.resize(nargs,0);

    // find function using per-expression callback and then global table
    // TODO: put lookup of local functions here
    _func=0;
    if(env.findFunction(_name)){
        std::cerr<<"found local func"<<std::endl;
    }
    if(!_func) _func = _expr->resolveFunc(_name);
    if(!_func) _func = SeExprFunc::lookup(_name);

    //check that function exisits and that the function has the right number of arguments
    if(checkCondition(_func,"Function " + _name + " has no definition", error) 
        && checkCondition(nargs >= _func->minArgs(),"Too few args for function"+_name,error)
        && checkCondition(nargs <= _func->maxArgs() || _func->maxArgs() < 0, "Too many args for function "+_name,error)) {

        const SeExprFuncX* funcx=_func->funcx();
        SeExprType type=funcx->prep(this,wantScalar,env);
        setTypeWithChildLife(type);
    }else{ // didn't match num args or function not found
        SeExprNode::prep(false,env); // prep arguments anyways to catch as many errors as possible!
        setTypeWithChildLife(SeExprType().Error());
    }

    return _type;
}

int SeExprFuncNode::
buildInterpreter(SeInterpreter* interpreter) const
{
    return _func->funcx()->buildInterpreter(this,interpreter);
}

bool SeExprFuncNode::
checkArg(int arg,SeExprType type,SeExprVarEnv& env)
{
    SeExprType childType=child(arg)->prep(type.isFP(1),env);
    std::cerr<<"we have "<<childType.toString()<<" and want "<<type.toString()<<std::endl;
    _promote[arg]=0;
    if(SeExprType::valuesCompatible(type,childType) && type.isLifeCompatible(childType)){
        if(type.isFP() && type.dim() > childType.dim()){
            std::cerr<<"setting promote "<<std::endl;
            _promote[arg]=type.dim();
        }
        return true;
    }
    child(arg)->addError("Expected "+type.toString()+" for argument, got "+childType.toString());
    return false;
}
