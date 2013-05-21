/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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

using namespace std;

SeExpression::SeExpression()
    : _wantVec(true), _parseTree(0), _parsed(0), _prepped(0)
{
    SeExprFunc::init();
}


SeExpression::SeExpression( const std::string &e, bool wantVec )
    : _wantVec(wantVec),  _expression(e), _parseTree(0),
      _parsed(0), _prepped(0)
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
    for(size_t i=0;i<_stringTokens.size();i++) free(_stringTokens[i]);
    _stringTokens.clear();
    _threadUnsafeFunctionCalls.clear();
}

void SeExpression::setWantVec(bool wantVec)
{
    reset();
    _wantVec = wantVec;
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
    SeExprParse(_parseTree, _parseError, tempStartPos, tempEndPos, 
        this, _expression.c_str(), &_stringTokens);
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
    if (_parseTree && !_parseTree->prep(wantVec())) {
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
