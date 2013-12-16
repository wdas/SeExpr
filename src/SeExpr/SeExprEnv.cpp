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

#include "SeExprType.h"
#include "SeExprEnv.h"
#include "SeExpression.h"

#include <vector>

SeExprVarEnv::~SeExprVarEnv() {
    resetAndSetParent(0);
}

void SeExprVarEnv::resetAndSetParent(SeExprVarEnv* parent)
{
    _parent=parent;
    for(VarDictType::const_iterator ienv=_map.begin(); ienv != _map.end(); ++ienv)
        delete ienv->second;
}

SeExprLocalVar* SeExprVarEnv::find(const std::string & name)
{
    VarDictType::iterator iter=_map.find(name);
    if(iter != _map.end()) return iter->second;
    else if(_parent) return _parent->find(name);
    else return 0;
}

SeExprLocalFunctionNode* SeExprVarEnv::findFunction(const std::string& name){
    FuncDictType::iterator iter=_functions.find(name);
    if(iter != _functions.end()) return iter->second;
    else if(_parent) return _parent->findFunction(name);
    else return 0;
}


SeExprLocalVar const * SeExprVarEnv::lookup(const std::string & name) const
{
    VarDictType::const_iterator iter=_map.find(name);
    if(iter != _map.end()) return iter->second;
    else if(_parent) return _parent->lookup(name);
    return 0;
}

void SeExprVarEnv::addFunction(const std::string& name,SeExprLocalFunctionNode* prototype)
{
    // go to parent until we are at root (all functions globally declared)
    if(_parent) _parent->addFunction(name, prototype);
    else{
        FuncDictType::iterator iter=_functions.find(name);
        if(iter != _functions.end()) iter->second=prototype;
        else _functions.insert(std::make_pair(name,prototype));
    }
}
    

void SeExprVarEnv::add(const std::string& name, SeExprLocalVar* var)
{
    VarDictType::iterator iter=_map.find(name);
    if(iter != _map.end()) iter->second=var;
    else _map.insert(std::make_pair(name,var));
}

void SeExprVarEnv::mergeBranches(const SeExprType& type,SeExprVarEnv& env1,SeExprVarEnv& env2)
{
    typedef std::map<std::pair<SeExprLocalVar*,SeExprLocalVar*>,std::string> MakeMap;
    MakeMap phisToMake;
    for(VarDictType::iterator ienv=env1._map.begin(); 
        ienv != env1._map.end(); ++ienv) {
        const std::string & name = ienv->first;
        SeExprLocalVar* var  = ienv->second;
        SeExprLocalVar* env2Var = env2.find(name);
        if(env2Var){
            phisToMake[std::make_pair(var,env2Var)]=name;
        }
    }
    for(VarDictType::iterator ienv=env2._map.begin(); 
        ienv != env2._map.end(); ++ienv) {
        const std::string & name = ienv->first;
        SeExprLocalVar* var  = ienv->second;
        SeExprLocalVar* env1Var = env1.find(name);
        if(env1Var){
            phisToMake[std::make_pair(env1Var,var)]=name;
        }
    }

    for(MakeMap::iterator it=phisToMake.begin();it!=phisToMake.end();++it){
        add(it->second,new SeExprLocalVarPhi(type,it->first.first,it->first.second));
    }
}
