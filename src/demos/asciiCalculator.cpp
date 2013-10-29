/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/
#include <SeExpression.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define STACK_DEPTH 256                                                                                                                                                                            
/**
   @file asciiCalculator.cpp
*/
//! Simple expression class to support our function calculator
class CalculatorExpr : public SeExpression
{
public:
    //! Constructor that takes the expression to parse
    CalculatorExpr(const std::string& expr)
	:SeExpression(expr), _count(0)
    {
	for(int i = 0; i < STACK_DEPTH; i++) {
	    stack[i].val = SeVec3d(0.0);
	    fail_stack[i] = false;
	};
    };
    
    //! Empty constructor
    CalculatorExpr()
	:SeExpression(), _count(0)
    {
	for(int i = 0; i < STACK_DEPTH; i++)
	    fail_stack[i] = false;
    };
    
    //! Push current result on stack
    void push() {
	stack[_count].val = evaluate();
	_count++;
    };
    
    //! Failed attempt; push 0 on stack
    void fail_push() {
	fail_stack[_count] = true;
	stack[_count].val = SeVec3d(0.0);
	_count++;
    };
    
    int count() const { return _count; };
    
private:
    //! Simple variable that just returns its internal value
    struct SimpleVar:public SeExprVectorVarRef
    {
	SimpleVar()
	    :val(SeVec3d(0.0))
	{}
	
	SeVec3d val; // independent variable
	
	void eval(const SeExprVarNode* /*node*/,SeVec3d& result)
	{
	    result = val;
	}
    };
    
    //! previous computations
    mutable SimpleVar stack     [STACK_DEPTH];
    mutable bool      fail_stack[STACK_DEPTH];
    mutable int       _count;
    
    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const {
	if(name[0] == '_') {
	    int position = atoi(name.substr(1,name.size() - 1).c_str());
	    if(position >= count())
		std::cerr << "Use of unused result line." << std::endl;
	    if(fail_stack[position])
		std::cerr << "Use of invalid result line." << std::endl;
	    return &(stack[position]);
	};
	return 0;
    };
};


void quit(const std::string & str) {
    if(str == "quit"
       || str == "q")
	exit(0);
};


int main()
{
    CalculatorExpr expr;
    std::string str;
    
    std::cout << "SeExpr Basic Calculator";
    
    while(true) {
	std::cout << std::endl << expr.count() << "> ";
	//std::cin >> str;
	getline(std::cin, str);
	
	if(std::cin.eof()) {
	    std::cout << std::endl;
	    str = "q";
	};
	
	quit(str);
	expr.setWantVec(true);
	expr.setExpr(str);
	
	if(!expr.isValid()) {
	    expr.fail_push();
	    std::cerr << "Expression failed: " << expr.parseError() << std::endl;
	} else {
	    expr.push();
	    std::cout << "   " << expr.evaluate();
	};
    };
    
    return 0;
}
