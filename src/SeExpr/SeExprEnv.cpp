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
    if(!_anotherOwns) 
        for(DictType::const_iterator ienv=_map.begin(); ienv != _map.end(); ++ienv)
            delete ienv->second;
}

SeExprVarRef* SeExprVarEnv::find(const std::string & name)
{
    DictType::iterator iter=_map.find(name);
    if(iter != _map.end()) return iter->second;
    else if(_parent) return _parent->find(name);
    else return 0;
}

SeExprVarRef const * SeExprVarEnv::lookup(const std::string & name) const
{
    DictType::const_iterator iter=_map.find(name);
    if(iter != _map.end()) return iter->second;
    else if(_parent) return _parent->lookup(name);
    return 0;
}

void SeExprVarEnv::add(const std::string & name, SeExprVarRef * var)
{
    DictType::iterator iter=_map.find(name);
    if(iter != _map.end()) *iter->second=*var;
    else _map.insert(std::pair<std::string,SeExprVarRef*>(name,var));
}

void SeExprVarEnv::add(SeExprVarEnv & env, const SeExprType & modifyingType)
{
    //! Iterate over all variables in env and copy types into our scope
    for(DictType::iterator ienv=env._map.begin(); ienv != env._map.end(); ++ienv) {
        SeExprVarRef* ref=ienv->second;
        SeExprType type=ref->type();
        type.setLifetime(type, modifyingType);
        ref->setType(type);
        add(ienv->first, ref); // add to this scope
    }
    // this takes ownership of all members of env, so tell it not to delete them
    env._anotherOwns=true;
}

bool SeExprVarEnv::branchesMatch(const SeExprVarEnv & env1, const SeExprVarEnv & env2)
{
    bool match = true;
    // Check each variable in env1 by looking up and checking type against env2
    for(DictType::const_iterator ienv=env1._map.begin(); 
        match && ienv != env1._map.end(); ++ienv) {
        const std::string & name = ienv->first;
        const SeExprVarRef * var  = ienv->second;
        const SeExprVarRef* env2Var = env2.lookup(name);
        match = env2Var && env2Var->type() == var->type();
    }
    // Check other map (only need to check for variables in env2 but not in env1
    for(DictType::const_iterator ienv=env2._map.begin(); 
        match && ienv != env2._map.end(); ++ienv) {
        const std::string & name = ienv->first;
        //const SeExprVarRef * var  = ienv->second;
        match = env1.lookup(name) != 0;
    }

    return match;
}

