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
#ifndef ExprEnv_h
#define ExprEnv_h

#include <vector>
#include <map>
#include <cassert>

#include "ExprType.h"
#include <iostream>


namespace SeExpr2 {
class ExprVarRef;
class ExprLocalVar;
class ExprNode;
class ExprLocalFunctionNode;


//! ExprLocalVar reference
class ExprLocalVar
{
protected:
    ExprType _type;
    ExprLocalVar* _phi;
public:

    ExprLocalVar(const ExprType& type):_type(type),_phi(0)
    {}

    virtual ~ExprLocalVar(){}

    //! get the primary representative phi node (i.e. the global parent of a dependent phi node)
    const ExprLocalVar* getPhi() const{return _phi;}
    //! returns type of the variable
    ExprType type() const{return _type;}
    //! sets the representative phi node (like a brute force set unioning operation) phi is the set representative
    virtual void setPhi(ExprLocalVar* phi){_phi=phi;}
};

//! ExprLocalVar join reference. This connects to other var references by a condition and
class ExprLocalVarPhi:public ExprLocalVar
{
public:
    ExprLocalVarPhi(ExprType condLife,ExprLocalVar* thenVar,ExprLocalVar* elseVar)
        :ExprLocalVar(ExprType()),_thenVar(thenVar),_elseVar(elseVar)
    {
        if(_thenVar->type() != _elseVar->type()){
            _type=ExprType().Error();
        }else{
            _type=ExprType(_thenVar->type()).setLifetime(condLife);
            setPhi(this);
        }
    }

    void setPhi(ExprLocalVar* phi){
        _phi=phi;
        _thenVar->setPhi(phi);
        _elseVar->setPhi(phi);
    }

    ExprNode* _condNode;
    ExprLocalVar *_thenVar,*_elseVar;
};

//! Variable scope for tracking variable lookup
class ExprVarEnv {
 private:
    typedef std::map<std::string,ExprLocalVar*> VarDictType;
    VarDictType _map;
    typedef std::map<std::string,ExprLocalFunctionNode*> FuncDictType;
    FuncDictType _functions;

    ExprVarEnv * _parent;

protected:
    ExprVarEnv(ExprVarEnv& other);
    ExprVarEnv& operator=(ExprVarEnv& other);

 public:
    // TODO: figure out when anotherOwns is needed
    //! Create a scope with no parent
    ExprVarEnv()
        : _parent(0)
    {};

    ~ExprVarEnv();
    
    //! Resets the scope (deletes all variables) and sets parent
    void resetAndSetParent(ExprVarEnv* parent);
    //! Find a function by name (recursive to parents)
    ExprLocalFunctionNode* findFunction(const std::string& name);
    //! Find a variable name by name (recursive to parents)
    ExprLocalVar* find(const std::string& name);
    //! Find a const variable reference name by name (recursive to parents)
    ExprLocalVar const* lookup(const std::string& name) const;
    //! Add a function
    void addFunction(const std::string& name,ExprLocalFunctionNode* prototype);
    //! Add a variable refernece
    void add(const std::string& name,ExprLocalVar* var);
    //! Add all variables into scope by name, but modify their lifetimes to the given type's lifetime
//    void add(ExprVarEnv & env,const ExprType & modifyingType);
    //! Checks if each branch shares the same items and the same types!
    // static bool branchesMatch(const ExprVarEnv & env1, const ExprVarEnv & env2);
    void mergeBranches(const ExprType& type,ExprVarEnv& env1,ExprVarEnv& env2);
};

//! Evaluation result.
struct ExprEvalResult
{
    ExprEvalResult()
        :n(0),fp(0),str(0)
        {}
    ExprEvalResult(int n,double* fp)
        :n(n),fp(fp),str(0)
        {}
    ExprEvalResult(const char** c)
        :n(1),fp(0),str(c)
        {}
    ExprEvalResult(int n,double* fp,const char** c)
        :n(n),fp(fp),str(c)
    {}

    int n;
    double* fp;
    const char** str;

};

}
#endif
