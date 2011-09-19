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

#include <vector>
#include <iostream>
#include <cmath>
#include <SeExprNode.h>
#include <SeExprWalker.h>
#include <SeExpression.h>
#include <typeinfo>



class Expr:public SeExpression
{
public:
    void walk()
    {
    }

    struct Var:public SeExprVarRef
    {
        Var()
            :SeExprVarRef(SeExprType().Varying().FP(3))
            {}
        double val;
        void eval(double* result,char** resultStr)
            {
                for(int k=0;k<3;k++) result[k]=val;
            }
    } ;
    mutable Var X;

    struct VarStr:public SeExprVarRef
    {
        VarStr()
            :SeExprVarRef(SeExprType().Varying().String())
            {}
        void eval(double* result,char** resultStr)
            {
                resultStr[0]="testo";
            }
    };
    mutable VarStr s;

    SeExprVarRef* resolveVar(const std::string& name) const {
        std::cerr<<"trying to resolve "<<name<<std::endl;
        if(name=="X") return &X;
        if(name=="s") return &s;
        return 0;
    }
};


int main(int argc,char* argv[])
{
    Expr expr;
    expr.setExpr(argv[1]);
    if(!expr.isValid()){
        std::cerr<<"parse error "<<expr.parseError()<<std::endl;
    }else{
        //expr._interpreter->print();
        double sum=0;
        for(int i=0;i<200000000;i++){
            expr.X.val=(double)i;
            double* d=expr.evalNew();
            sum+=d[0];
        }
        std::cerr<<"sum "<<sum<<std::endl;
        expr._interpreter->print();
    }

    return 0;
}




