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
#ifndef SeExprEnv_h
#define SeExprEnv_h

#include <vector>
#include <map>
#include <cassert>

#include "SeExprType.h"

class SeExprVarRef;

//! Variable scope for tracking variable lookup
class SeExprVarEnv {
 private:
    typedef std::map<std::string,SeExprVarRef*> DictType;
    DictType       _map;
    SeExprVarEnv * _parent;
    //! False if this env owns, true if some other env has taken ownership
    //! e.g. see the IfThenElseNode
    mutable bool   _anotherOwns;

 public:
    //! Create a scope that inherits from parent
    SeExprVarEnv(SeExprVarEnv * parent)
        : _map(), _parent(parent), _anotherOwns(false)
    {};

    // TODO: figure out when anotherOwns is needed
    //! Create a scope with no parent
    SeExprVarEnv()
        : _map(), _parent(0), _anotherOwns(false)
    {};

    ~SeExprVarEnv();

    //! Find a variable name by name (recursive to parents)
    SeExprVarRef* find(const std::string& name);
    //! Find a const variable reference name by name (recursive to parents)
    SeExprVarRef const* lookup(const std::string& name) const;
    //! Add a variable refernece
    void add(const std::string& name,SeExprVarRef* var);
    //! Add all variables into scope by name, but modify their lifetimes to the given type's lifetime
    void add(SeExprVarEnv & env,const SeExprType & modifyingType);
    //! Checks if each branch shares the same items and the same types!
    static bool branchesMatch(const SeExprVarEnv & env1, const SeExprVarEnv & env2);
};

#endif
