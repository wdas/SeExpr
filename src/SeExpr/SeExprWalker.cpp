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
#include <string.h>
#include <string>
#include <vector>
#endif

#include "SeExprNode.h"
#include "SeExprPatterns.h"
#include "SeExprSpec.h"
#include "SeExprWalker.h"

void
SeExprWalker::walk(SeExprNode* examinee)
{
    _examiner->reset();
    internalWalk(examinee);
};

void
SeExprWalker::internalWalk(SeExprNode* examinee)
{
    /// If examine returns false, do not recurse
    if(_examiner->examine(examinee)) walkChildren(examinee);
};

void
SeExprWalker::walkChildren(SeExprNode* parent)
{
    for(int i = 0; i < parent->numChildren(); i++)
	internalWalk(parent->child(i));
};

void
SeExprConstWalker::walk(const SeExprNode* examinee)
{
    _examiner->reset();
    internalWalk(examinee);
};

void
SeExprConstWalker::internalWalk(const SeExprNode* examinee)
{
    /// If examine returns false, do not recurse
    if(_examiner->examine(examinee)) walkChildren(examinee);
};

void
SeExprConstWalker::walkChildren(const SeExprNode* parent)
{
    for(int i = 0; i < parent->numChildren(); i++)
	internalWalk(parent->child(i));
};

bool
SeExprVarListExaminer::examine(const SeExprNode* examinee)
{
    if(const SeExprVarNode* var = isVariable(examinee)) {
        _varList.push_back(var);
	return false;
    };

    return true;
};

SeExprSpecExaminer::~SeExprSpecExaminer() {
    std::vector<const SeExprSpec*>::iterator       i = _specList.begin();
    std::vector<const SeExprSpec*>::iterator const e = _specList.end  ();
    for(; i != e; ++i)
        delete *i;
};

bool
SeExprSpecExaminer::examine(const SeExprNode* examinee)
{
    if(const SeExprScalarAssignSpec* s_spec =
       SeExprScalarAssignSpec::match(examinee)) {
        _specList.push_back(s_spec);
        return false;
    } else if(const SeExprVectorAssignSpec* v_spec =
              SeExprVectorAssignSpec::match(examinee)) {
        _specList.push_back(v_spec);
        return false;
    } else if(const SeExprCurveAssignSpec* c_spec =
              SeExprCurveAssignSpec::match(examinee)) {
        _specList.push_back(c_spec);
        return false;
    } else if(const SeExprCcurveAssignSpec* cc_spec =
              SeExprCcurveAssignSpec::match(examinee)) {
        _specList.push_back(cc_spec);
        return false;
    } else if(const SeExprStrSpec* str_spec =
              SeExprStrSpec::match(examinee)) {
        _specList.push_back(str_spec);
        return false;
    };

    return true;
};

inline std::vector<const SeExprSpec*>::const_iterator
SeExprSpecExaminer::begin() const
{
    return _specList.begin();
};

inline std::vector<const SeExprSpec*>::const_iterator const
SeExprSpecExaminer::end() const
{
    return _specList.end();
};
