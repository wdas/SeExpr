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
#ifndef _SeExprFuncX_h_
#define _SeExprFuncX_h_

#include "SeExprType.h"
#include "SeVec.h"
#include "SeExprNode.h"


class SeExprFuncNode;
class SeInterpreter;
class SeExprVarEnv;

//! Extension function spec, used for complicated argument custom functions.
/** Provides the ability to handle all argument type checking and processing manually.
    Derive from this class and then make your own SeExprFunc that takes this object.
    This is necessary if you need string arguments or you have variable numbers of
    arguments.  See SeExprBuiltins.h for some examples */
class SeExprFuncX {
public:
    //! Create an SeExprFuncX. If the functions and type checking you implement
    //! is thread safe your derived class should call this with true. If not,
    //! then false.  If you mark a function as thread unsafe,  and it is used
    //! in an expression then bool SeExpression::isThreadSafe() will return false
    //! and the controlling software should not attempt to run multiple threads
    //! of an expression.
    SeExprFuncX(const bool threadSafe)
        :_threadSafe(threadSafe)
    {}

    /** prep the expression by doing all type checking argument checking, etc. */
    virtual SeExprType prep(SeExprFuncNode* node, bool scalarWanted, SeExprVarEnv & env) const=0;
    virtual SeExprType type() const {return _type;}

    /** evaluate the expression. the given node is where in the parse tree
        the evaluation is for */
    //virtual void eval(const SeExprFuncNode* node, SeVec3d& result) const = 0;
    //! Build an interpreter to evaluate the expression
    virtual int buildInterpreter(const SeExprFuncNode* node,SeInterpreter* interpreter) const = 0;
    virtual ~SeExprFuncX(){}

    bool isThreadSafe() const {return _threadSafe;}

 protected:
    bool _isScalar;
    SeExprType _type;
private:
    bool _threadSafe;
};

class SeExprFuncSimple:public SeExprFuncX
{
public:
    SeExprFuncSimple(const bool threadSafe)
        :SeExprFuncX(threadSafe)
    {}
    
    class ArgHandle{
    public:
        ArgHandle(int* opData,double* fp,char** c,std::stack<int>& callStack)
            :outFp(fp[opData[2]]),outStr(c[opData[2]]),
            data(reinterpret_cast<SeExprFuncNode::Data*>(c[opData[1]])),
            _nargs((int)fp[opData[3]]), // TODO: would be good not to have to convert to int!
            opData(opData+4),fp(fp),c(c)
        {}

        template<int d> SeVec<double,d,true> inFp(int i){return SeVec<double,d,true>(&fp[opData[i]]);}
        char* inStr(int i){return c[opData[i]];}
        int nargs() const{return _nargs;}

        double& outFp;
        char*& outStr;
        SeExprFuncNode::Data* data;
    private:
        int _nargs;
        int* opData;
        double* fp;
        char** c;
        //std::stack<int>& callStack;
    };

    virtual int buildInterpreter(const SeExprFuncNode* node,SeInterpreter* interpreter) const;

    virtual SeExprType prep(SeExprFuncNode* node,bool scalarWanted,SeExprVarEnv& env) const=0;
    virtual SeExprFuncNode::Data* evalConstant(ArgHandle args) const=0;
    virtual void eval(ArgHandle args)=0;

private:
    static int EvalOp(int* opData,double* fp,char** c,std::stack<int>& callStack);
};

class SeExprFuncLocal:public SeExprFuncX{
    SeExprFuncLocal()
    :SeExprFuncX(true)
    {}
    
        /** prep the expression by doing all type checking argument checking, etc. */
    virtual SeExprType prep(SeExprFuncNode* node, bool scalarWanted, SeExprVarEnv & env) const;
    //! Build an interpreter to evaluate the expression
    virtual int buildInterpreter(const SeExprFuncNode* node,SeInterpreter* interpreter) const;

};

#endif
