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
#include "SeExpression.h"

class SeExprVarEnv {
 private:
    std::string                typedef KeyType;
    SeExprVarRef               typedef ValType;
    std::map<KeyType, ValType *> typedef DictType;

    SeExprVarEnv(SeExprVarEnv * parent)
        : _map(), _parent(parent)
    {};

 public:
    SeExprVarEnv()
        : _map(), _parent(0)
    {};

    ValType       * find  (const KeyType      & name);
    ValType const * lookup(const KeyType      & name)                const;
    void            add   (const KeyType      & name, ValType * var);
    void            add   (const SeExprVarEnv & env);

    inline int size() const { return _map.size(); };

    static bool         branchesMatch(const SeExprVarEnv & env1, const SeExprVarEnv & env2);
    static SeExprVarEnv newBranch    (      SeExprVarEnv & env);

 protected:
    DictType::      iterator       begin()       { return _map.begin(); };
    DictType::const_iterator       begin() const { return _map.begin(); };
    DictType::      iterator const end  ()       { return _map.end  (); };
    DictType::const_iterator const end  () const { return _map.end  (); };

 private:
    ValType       * localFind  (const KeyType & name);
    ValType const * localLookup(const KeyType & name) const;
    ValType       * parFind    (const KeyType & name);
    ValType const * parLookup  (const KeyType & name) const;

    DictType       _map;
    SeExprVarEnv * _parent;
};

class SeExprFuncRef {
 public:
    SeExprFuncRef(SeExprType ret, const std::vector<SeExprType>& args)
        : _retType(ret), _args(args)
    {};

    inline       SeExprType                returnType() const { return _retType; };
    inline const std::vector<SeExprType> & argTypes  () const { return _args;    };

 private:
    SeExprType _retType;
    std::vector<SeExprType> _args;
};

class ReturnValue {
 public:
    ReturnValue();

 private:
    union Ptrs {
        float * flo;
        double* dou;
        char  * str;
    };
    Ptrs valuePtr;
};

#endif
