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
#include <Expression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ExprWalker.h"
#include "ExprPatterns.h"

using namespace SeExpr;

/// Examiner that builds a list of all variable references
class VarListExaminer : public ConstExaminer {

 public:
    virtual bool examine(T_NODE* examinee){
        if(const ExprVarNode* var = isVariable(examinee)) {
            _varList.push_back(var);
            return false;
        };
        return true;
    }
    virtual void reset() {_varList.clear();};
    inline int length() const {return _varList.size();};
    inline const ExprVarNode* var(int i) const {return _varList[i];};
 private:
    std::vector<const ExprVarNode*> _varList;
};





/**
   @file listVar.cpp
*/
//! Simple expression class to list out variable uses
class ListVarExpr : public Expression
{
public:
    //! Constructor that takes the expression to parse
    ListVarExpr(const std::string& expr)
	:Expression(expr), _hasWalked(false), examiner(), walker(&examiner)
    {};

    //! Empty constructor
    ListVarExpr()
	:Expression(), _hasWalked(false), examiner(), walker(&examiner)
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
    VarListExaminer examiner;
    ConstWalker walker;
    
    //! resolve function that only supports one external variable 'x'
    ExprVarRef* resolveVar(const std::string& name) const {
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
