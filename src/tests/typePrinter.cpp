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
#include "SeExprNode.h"
#include "SeExprFunc.h"

/**
   @file typePrinter.cpp
*/
class TypePrintExaminer : public SeExpr::Examiner<true> {
public:
    virtual bool examine(const SeExprNode* examinee);
    virtual void reset  ()                           {};
};


bool
TypePrintExaminer::examine(const SeExprNode* examinee)
{
    std::cout <<           examinee->       toString() << std::endl;
    std::cout << "    " << examinee->type().toString() << std::endl;

    return true;
};


//! Simple expression class to print out all intermediate types
class TypePrinterExpr : public SeExpression
{
public:
    struct DummyFuncX:SeExprFuncX
    {
        DummyFuncX()
            : SeExprFuncX(false)
        {};

        virtual bool       isScalar() const { return true;                  };
        virtual SeExprType retType () const { return SeExprType::FP1Type(); };

        void eval(const SeExprFuncNode* node,SeVec3d& result) const
        {result=SeVec3d();}
    } dummyFuncX;
    mutable SeExprFunc dummyFunc;

    //! Constructor that takes the expression to parse
    TypePrinterExpr(const std::string& expr) :
        SeExpression(expr), dummyFunc(dummyFuncX,0,16),
        _examiner(),
        _walker(&_examiner)
    {};

    //! Empty constructor
    TypePrinterExpr() :
        SeExpression(), dummyFunc(dummyFuncX,0,16),
        _examiner(),
        _walker(&_examiner)
    {};

    inline void walk () { if(isValid()) _walker.walk(_parseTree); };

private:
    TypePrintExaminer _examiner;
    SeExpr::ConstWalker  _walker;

    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const {
	return 0;
    };

    SeExprFunc* resolveFunc(const std::string& name) const
    {
        return &dummyFunc;
    }
};


void get_or_quit(std::string & str) {
    getline(std::cin, str);

    if(std::cin.eof())
        exit(0);
};


int main(int argc,char *argv[])
{
    TypePrinterExpr expr;
    std::string str;

    std::cout << "SeExpr Basic Pattern Matcher:";

    while(true) {
	std::cout << std::endl << "> ";

        get_or_quit(str);

        expr.setExpr(str);

	if(!expr.isValid()) {
	    std::cerr << "Expression failed: " << expr.parseError() << std::endl;
	} else {
            std::cout << "Expression types:"
                      << std::endl;
            expr.walk();
	};
    };

    return 0;
}
