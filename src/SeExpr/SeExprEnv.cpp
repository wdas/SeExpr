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
#include "SeExprType.h"
#include "SeExprEnv.h"
#include "SeExpression.h"
#include <vector>

namespace SeExpr2 {

ExprVarEnv::~ExprVarEnv() {
    resetAndSetParent(0);
}

void ExprVarEnv::resetAndSetParent(ExprVarEnv* parent)
{
    _parent=parent;
    // TODO: can't delete these, because they need to live longer than prep()
    //for(VarDictType::const_iterator ienv=_map.begin(); ienv != _map.end(); ++ienv)
    //    delete ienv->second;
}

ExprLocalVar* ExprVarEnv::find(const std::string & name)
{
    VarDictType::iterator iter=_map.find(name);
    if(iter != _map.end()) return iter->second;
    else if(_parent) return _parent->find(name);
    else return 0;
}

ExprLocalFunctionNode* ExprVarEnv::findFunction(const std::string& name){
    FuncDictType::iterator iter=_functions.find(name);
    if(iter != _functions.end()) return iter->second;
    else if(_parent) return _parent->findFunction(name);
    else return 0;
}


ExprLocalVar const * ExprVarEnv::lookup(const std::string & name) const
{
    VarDictType::const_iterator iter=_map.find(name);
    if(iter != _map.end()) return iter->second;
    else if(_parent) return _parent->lookup(name);
    return 0;
}

void ExprVarEnv::addFunction(const std::string& name,ExprLocalFunctionNode* prototype)
{
    // go to parent until we are at root (all functions globally declared)
    if(_parent) _parent->addFunction(name, prototype);
    else{
        FuncDictType::iterator iter=_functions.find(name);
        if(iter != _functions.end()) iter->second=prototype;
        else _functions.insert(std::make_pair(name,prototype));
    }
}
    

void ExprVarEnv::add(const std::string& name, ExprLocalVar* var)
{
    VarDictType::iterator iter=_map.find(name);
    if(iter != _map.end()) iter->second=var;
    else _map.insert(std::make_pair(name,var));
}

void ExprVarEnv::mergeBranches(const ExprType& type,ExprVarEnv& env1,ExprVarEnv& env2)
{
    typedef std::map<std::pair<ExprLocalVar*,ExprLocalVar*>,std::string> MakeMap;
    MakeMap phisToMake;
    for(VarDictType::iterator ienv=env1._map.begin(); 
        ienv != env1._map.end(); ++ienv) {
        const std::string & name = ienv->first;
        ExprLocalVar* var  = ienv->second;
        ExprLocalVar* env2Var = env2.find(name);
        if(env2Var){
            phisToMake[std::make_pair(var,env2Var)]=name;
        }
    }
    for(VarDictType::iterator ienv=env2._map.begin(); 
        ienv != env2._map.end(); ++ienv) {
        const std::string & name = ienv->first;
        ExprLocalVar* var  = ienv->second;
        ExprLocalVar* env1Var = env1.find(name);
        if(env1Var){
            phisToMake[std::make_pair(env1Var,var)]=name;
        }
    }

    for(MakeMap::iterator it=phisToMake.begin();it!=phisToMake.end();++it){
        add(it->second,new ExprLocalVarPhi(type,it->first.first,it->first.second));
    }
}

}
