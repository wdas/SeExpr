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

#ifndef SeExprWalker_h
#define SeExprWalker_h

class SeExprNode;

namespace SeExpr{

template<class T,bool constnode> struct ADD_CONST{typedef T TYPE;};
template<class T> struct ADD_CONST<T,true>{typedef const T TYPE;};


template<bool constnode=false>
class Examiner
{
public:
    typedef typename ADD_CONST<SeExprNode,constnode>::TYPE T_NODE;

    virtual bool examine(T_NODE *examinee) = 0;
    virtual void reset() = 0;
};

template<bool constnode=false> 
class Walker
{
 public:
    typedef Examiner<constnode> T_EXAMINER;
    typedef typename T_EXAMINER::T_NODE T_NODE;

    Walker(T_EXAMINER* examiner)
	: _examiner(examiner)
    { _examiner->reset(); };

    /// Preorder walk
    void walk(T_NODE * examinee);

 protected:
    void internalWalk(T_NODE* examinee);
    void walkChildren(T_NODE* parent);

 private:
    T_EXAMINER * _examiner;
};

typedef Examiner<true> ConstExaminer;
typedef Walker<true> ConstWalker;

}
#endif
