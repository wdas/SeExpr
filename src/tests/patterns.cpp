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
#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "SeExprWalker.h"
#include "SeExprPatterns.h"
#include "SeExprFunc.h"
#include "SeControlSpec.h"


/**
   @file assignmentPatterns.cpp
*/
//! Simple expression class to list out variable uses
class PatternExpr : public SeExpression
{
public:
    struct DummyFuncX:SeExprFuncX
    {
        DummyFuncX()
            : SeExprFuncX(false)
        {};

        bool prep(SeExprFuncNode* node,bool wantVec,std::string& error) 
        {
            bool valid=true;
            for(int i=0;i<node->numChildren();i++){
                if(!node->isStrArg(i))
                    valid&=node->child(i)->prep(wantVec);
            }
            return true;}
        void eval(const SeExprFuncNode* node,SeVec3d& result) const
        {result=SeVec3d();}
    } dummyFuncX;
    mutable SeExprFunc dummyFunc;

    //! Constructor that takes the expression to parse
    PatternExpr(const std::string& expr) :
        SeExpression(expr), dummyFunc(dummyFuncX,0,16),
        _examiner(),
        _walker(&_examiner)
    {};

    //! Empty constructor
    PatternExpr() :
        SeExpression(), dummyFunc(dummyFuncX,0,16),
        _examiner(),
        _walker(&_examiner)
    {};
    
    inline void walk () { _walker.walk (_parseTree); };
    void specs() { if(isValid()) { walk(); printSpecs(_examiner); }; };

private:
    SeExpr::SpecExaminer _examiner;
    SeExpr::ConstWalker  _walker;
    
    template<typename Examiner>
    void printSpecs(Examiner examiner) {
        if(isValid()) {
            for(int i = 0; i < examiner.length(); ++i)
                std::cout << examiner.spec(i)->toString()
                          << std::endl;
        };
    };

    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const {
	return 0;
    };

    SeExprFunc* resolveFunc(const std::string& name) const
    {
        return &dummyFunc;
    }
};


void quit(const std::string & str) {
    if(str == "quit"
       || str == "q")
	exit(0);
};


int main(int argc,char *argv[])
{
    PatternExpr expr;
    std::string str;

    std::cout << "SeExpr Basic Pattern Matcher:";

    while(true) {
	std::cout << std::endl << "> ";
	//std::cin >> str;
	getline(std::cin, str);

	if(std::cin.eof()) {
	    std::cout << std::endl;
	    str = "q";
	};

	quit(str);
        expr.setExpr(str);

	if(!expr.isValid()) {
	    std::cerr << "Expression failed: " << expr.parseError() << std::endl;
	} else {
            std::cout << "Expression value:"
                      << std::endl;
	    std::cout << "   " << expr.evaluate() << std::endl;
            std::cout << "Expression patterns:"
                      << std::endl;
            expr.specs();
	};
    };

    return 0;
}
