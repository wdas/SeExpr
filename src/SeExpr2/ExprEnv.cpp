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

#include "ExprType.h"
#include "ExprEnv.h"
#include "Expression.h"
#include <vector>

namespace SeExpr2 {

ExprVarEnv::~ExprVarEnv()
{
    resetAndSetParent(0);
}

void ExprVarEnv::resetAndSetParent(ExprVarEnv* parent)
{
    _parent = parent;
}

ExprLocalVar* ExprVarEnv::find(const std::string& name)
{
    VarDictType::iterator iter = _map.find(name);
    if (iter != _map.end())
        return iter->second.get();
    else if (_parent)
        return _parent->find(name);
    else
        return 0;
}

ExprLocalFunctionNode* ExprVarEnv::findFunction(const std::string& name)
{
    FuncDictType::iterator iter = _functions.find(name);
    if (iter != _functions.end())
        return iter->second;
    else if (_parent)
        return _parent->findFunction(name);
    else
        return 0;
}

ExprLocalVar const* ExprVarEnv::lookup(const std::string& name) const
{
    VarDictType::const_iterator iter = _map.find(name);
    if (iter != _map.end())
        return iter->second.get();
    else if (_parent)
        return _parent->lookup(name);
    return 0;
}

void ExprVarEnv::addFunction(const std::string& name, ExprLocalFunctionNode* prototype)
{
    // go to parent until we are at root (all functions globally declared)
    if (_parent)
        _parent->addFunction(name, prototype);
    else {
        FuncDictType::iterator iter = _functions.find(name);
        if (iter != _functions.end())
            iter->second = prototype;
        else
            _functions.insert(std::make_pair(name, prototype));
    }
}

void ExprVarEnv::add(const std::string& name, std::unique_ptr<ExprLocalVar> var)
{
    VarDictType::iterator iter = _map.find(name);
    if (iter != _map.end()) {
        // throw std::runtime_error("Invalid creation of existing variable in same scope!");
        shadowedVariables.emplace_back(std::move(iter->second));
        iter->second = std::move(var);
    } else
        _map.insert(std::make_pair(name, std::move(var)));
}

size_t ExprVarEnv::mergeBranches(const ExprType& type, ExprVarEnv& env1, ExprVarEnv& env2)
{
    typedef std::map<std::pair<ExprLocalVar*, ExprLocalVar*>, std::string> MakeMap;
    MakeMap phisToMake;
    /// For each thing in env1 see if env2 has an entry
    for (auto& ienv : env1._map) {
        const std::string& name = ienv.first;
        ExprLocalVar* var = ienv.second.get();
        if (ExprLocalVar* env2Var = env2.find(name)) {
            phisToMake[std::make_pair(var, env2Var)] = name;
        }
    }
    /// For each thing in env2 see if env1 has an entry
    for (auto& ienv : env2._map) {
        const std::string& name = ienv.first;
        ExprLocalVar* var = ienv.second.get();
        if (ExprLocalVar* env1Var = env1.find(name)) {
            phisToMake[std::make_pair(env1Var, var)] = name;
        }
    }

    std::vector<std::pair<std::string, ExprLocalVarPhi*>> mergedVariablesInThisCall;
    for (MakeMap::iterator it = phisToMake.begin(); it != phisToMake.end(); ++it) {
        std::unique_ptr<ExprLocalVar> newVar(new ExprLocalVarPhi(type, it->first.first, it->first.second));
        mergedVariablesInThisCall.emplace_back(it->second, static_cast<ExprLocalVarPhi*>(newVar.get()));
        add(it->second, std::move(newVar));
    }
    _mergedVariables.emplace_back(std::move(mergedVariablesInThisCall));
    return _mergedVariables.size() - 1;
}
}
