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

/**
   @file listVar.cpp
*/
//! Simple expression class to list out variable uses
class ListVarExpr : public SeExpression
{
public:
    //! Constructor that takes the expression to parse
    ListVarExpr(const std::string& expr)
	:SeExpression(expr), _hasWalked(false), examiner(), walker(&examiner)
    {};

    //! Empty constructor
    ListVarExpr()
	:SeExpression(), _hasWalked(false), examiner(), walker(&examiner)
    {};

    void walk() {
	_hasWalked = true;
	walker.walk(_parseTree);
    };

    bool hasWalked() { return _hasWalked; };

    int count() const {
        if(isValid() && _hasWalked) {
            return examiner.length();
	};
	return 0;
    };

private:
    bool _hasWalked;
    SeExprVarListExaminer examiner;
    SeExprConstWalker walker;
    
    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const {
	return 0;
    };
};


void quit(const std::string & str) {
    if(str == "quit"
       || str == "q")
	exit(0);
};


int main(int argc,char *argv[])
{
    ListVarExpr expr;
    std::string str;
        
    std::cout << "SeExpr Basic Variable Use Finder\n *Note: Does not "
	      << "find variables being assigned to, only use.*";
    
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
	    std::cout << "   " << expr.evaluate() << std::endl;
	    expr.walk();
	    std::cout << "   number of variable refs: " << expr.count() << std::endl;
	};
    };
    
    return 0;
}
