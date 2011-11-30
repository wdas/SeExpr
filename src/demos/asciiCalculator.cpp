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
#include <SeVec.h>
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
	    stack[i].val = SeVec<double,3,false>(0.0);
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
        if(returnType().isString()){
            const char* s=evalStr();
        }else if(returnType().isFP()){
            const double* val=evalFP();
            int dim=returnType().dim();
            for(int k=0;k<3;k++) std::cerr<<val[k]<<" ";
            std::cerr<<std::endl;
            if(dim==1)
                stack[_count].val = SeVec<double,3,false>(val[0]);
            else if(dim==2)
                stack[_count].val = SeVec<double,3,false>(val[0],val[1],0);
            else if(dim==3)
                stack[_count].val = SeVec<double,3,true>(const_cast<double*>(&val[0]));
            else{
                std::cerr<<"Return type FP("<<dim<<") ignoring"<<std::endl;
            }

            _count++;
        }
    };
    
    //! Failed attempt; push 0 on stack
    void fail_push() {
	fail_stack[_count] = true;
	stack[_count].val = SeVec<double,3,false>(0.0);
	_count++;
    };
    
    SeVec<double,3,false> peek() {
        return stack[_count-1].val;
    }

    int count() const { return _count; };
    
private:
    //! Simple variable that just returns its internal value
    struct SimpleVar:public SeExprVarRef
    {
	SimpleVar()
	    : SeExprVarRef(SeExprType().FP(3).Varying()), val(0.0)
	{}
	
	SeVec<double,3,false> val; // independent variable
	
	void eval(double* result)
	{
	    for(int k=0;k<3;k++) result[k] = val[k];
	}

        void eval(char** reuslt)
        {}

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
		std::cerr << "Use of undefined variable." << std::endl;
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
	expr.setDesiredReturnType(SeExprType().FP(3));
	expr.setExpr(str);
	
	if(!expr.isValid()) {
	    expr.fail_push();
	    std::cerr << "Expression failed: " << expr.parseError() << std::endl;
	} else {
	    expr.push();
	    std::cout << "   " << expr.peek();
	};
    };
    
    return 0;
}
