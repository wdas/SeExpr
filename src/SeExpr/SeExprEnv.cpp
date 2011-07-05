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

#include <vector>

SeExprVarEnv::ValType *
SeExprVarEnv::localFind(const KeyType & name)
{
    DictType::iterator iter = _map.find(name);
    if(iter == _map.end())
        return 0;
    else
        return iter->second;
};

SeExprVarEnv::ValType const *
SeExprVarEnv::localLookup(const KeyType & name) const
{
    DictType::const_iterator iter = _map.find(name);
    if(iter == _map.end())
        return 0;
    else
        return iter->second;
};

SeExprVarEnv::ValType *
SeExprVarEnv::parFind(const KeyType & name)
{
    if(_parent)
        return _parent->find(name);
    else
        return 0;
};

SeExprVarEnv::ValType const *
SeExprVarEnv::parLookup(const KeyType & name) const
{
    if(_parent)
        return _parent->lookup(name);
    else
        return 0;
};

SeExprVarEnv::ValType *
SeExprVarEnv::find(const KeyType & name)
{
    ValType * answer = localFind(name);
    if(answer)
        return answer;
    else
        return parFind(name);
};

SeExprVarEnv::ValType const *
SeExprVarEnv::lookup(const KeyType & name) const
{
    ValType const * answer = localLookup(name);
    if(answer)
        return answer;
    else
        return parLookup(name);
};

bool
SeExprVarEnv::changesMatch(const SeExprVarEnv & env1, const SeExprVarEnv & env2) const
{
    bool match = true;

    DictType::const_iterator       ienv  = env1.begin();
    DictType::const_iterator const eenv1 = env1.end  ();

    for(; match && ienv != eenv1; ++ienv) {
        const KeyType & name = ienv->first;
        const ValType * var  = ienv->second;

        if(lookup(name)->type() != var->type())
            match = env2.lookup(name)->type() == var->type();
    }

    ienv = env2.begin();
    DictType::const_iterator const eenv2 = env2.end();

    for(; match && ienv != eenv2; ++ienv) {
        const KeyType & name = ienv->first;
        const ValType * var  = ienv->second;

        if(lookup(name)->type() != var->type())
            match = env1.lookup(name)->type() == var->type();
    }

    return match;
}

void
SeExprVarEnv::add(const KeyType & name, ValType * var)
{
    ValType * old = localFind(name);

    if(old)
        *old = *var;
    else
        _map.insert(std::pair<KeyType,ValType*>(name,var));
};

void
SeExprVarEnv::add(const SeExprVarEnv & env)
{
    DictType::const_iterator       ienv = env.begin();
    DictType::const_iterator const eenv = env.end  ();

    for(; ienv != eenv; ++ienv)
        add(ienv->first, ienv->second);
};

SeExprVarEnv
SeExprVarEnv::newScope(SeExprVarEnv & env)
{
    return SeExprVarEnv(&env);
};
