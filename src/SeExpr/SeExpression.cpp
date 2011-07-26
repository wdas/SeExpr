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
#include <iostream>
#include <math.h>
#include <stack>
#include <algorithm>
#include <sstream>
#endif

#include "SeExprNode.h"
#include "SeExprParser.h"
#include "SeExprFunc.h"
#include "SeExpression.h"
#include "SeExprType.h"
#include "SeExprEnv.h"

using namespace std;

SeExpression::SeExpression()
    : _wantVec(true), _returnType(SeExprType::AnyType()), _parseTree(0), _parsed(0), _prepped(0)
{
    SeExprFunc::init();
}


SeExpression::SeExpression( const std::string &e, const SeExprType & type)
    : _wantVec(true), _returnType(type), _expression(e), _parseTree(0), _parsed(0), _prepped(0)
{
    SeExprFunc::init();
}

SeExpression::~SeExpression()
{
    reset();
}

void SeExpression::reset()
{
    delete _parseTree;
    _parseTree = 0;
    _parsed = 0;
    _prepped = 0;
    _parseError = "";
    _vars.clear();
    _funcs.clear();
    _localVars.clear();
    _errors.clear();
    _threadUnsafeFunctionCalls.clear();
    _comments.clear();
}

void SeExpression::setWantVec(bool wantVec)
{
    reset();
    _wantVec = wantVec;
    std::cerr << "Use of setWantVec is deprecated. If you are seeing this, you used setWantVec.  Please use setReturnType instead." << std::endl;
}

void SeExpression::setReturnType(const SeExprType & type)
{
    reset();
    _returnType = type;
}

void SeExpression::setExpr(const std::string& e)
{
    reset();
    _expression = e;
}

bool SeExpression::syntaxOK() const
{
    parseIfNeeded();
    return _parseTree != 0;
}

bool SeExpression::isValid() const
{
    prepIfNeeded();
    return _parseTree != 0;
}

bool SeExpression::isConstant() const
{
    parseIfNeeded();
    return _vars.empty() && _funcs.empty();
}

bool SeExpression::usesVar(const std::string& name) const
{
    parseIfNeeded();
    return _vars.find(name) != _vars.end();
}

bool SeExpression::usesFunc(const std::string& name) const
{
    parseIfNeeded();
    return _funcs.find(name) != _funcs.end();
}

void
SeExpression::parse() const
{
    if (_parsed) return;
    _parsed = true;
    int tempStartPos,tempEndPos;
    SeExprParse(_parseTree,
        _parseError, tempStartPos, tempEndPos, 
        _comments, this, _expression.c_str(), _wantVec);
    if(!_parseTree){
        addError(_parseError,tempStartPos,tempEndPos);
    }
}

void
SeExpression::prep() const
{
    if (_prepped) return;
    _prepped = true;
    parseIfNeeded();
    SeExprVarEnv env;
    if (_parseTree && !_parseTree->prep(_returnType, env).isValid()) {
        // build line lookup table
        std::vector<int> lines;
        const char* start=_expression.c_str();
        const char* p=_expression.c_str();
        while(*p!=0){
            if(*p=='\n') lines.push_back(p-start);
            p++;
        }
        lines.push_back(p-start);

        std::stringstream sstream;
        sstream<<"Prep errors:"<<std::endl;
        for(unsigned int i=0;i<_errors.size();i++){
            int* bound=lower_bound(&*lines.begin(),&*lines.end(),_errors[i].startPos);
            int line=bound-&*lines.begin()+1;
            //int column=_errors[i].startPos-lines[line-1];
            sstream<<"  Line "<<line<<": "<<_errors[i].error<<std::endl;
        }
        _parseError=std::string(sstream.str());

	delete _parseTree; _parseTree = 0;
    }
}


bool
SeExpression::isVec() const
{
    prepIfNeeded();
    return _parseTree ? _parseTree->isVec() : _wantVec;
}

const SeExprType &
SeExpression::returnType() const
{
    prepIfNeeded();
    return _parseTree->type();
}

SeVec3d
SeExpression::evaluate() const
{
    prepIfNeeded();
    if (_parseTree) {
	// set all local vars to zero
	for (LocalVarTable::iterator iter = _localVars.begin();
	     iter != _localVars.end(); iter++)
	    iter->second.val = 0.0;

	SeVec3d vec;
	_parseTree->eval(vec);
	if (_wantVec && !isVec())
	    vec[1] = vec[2] = vec[0];
	return vec;
    }
    else return SeVec3d(0,0,0);
}
