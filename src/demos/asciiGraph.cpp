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
/**
   @file asciiGraph.cpp
*/
//! Simple expression class to support our function grapher
class GrapherExpr:public SeExpression
{
public:
    //! Constructor that takes the expression to parse
    GrapherExpr(const std::string& expr)
        :SeExpression(expr)
    {}

    //! set the independent variable
    void setX(double x_input)
    {x.val=x_input;}

private:
    //! Simple variable that just returns its internal value
    struct SimpleVar:public SeExprVarRef
    {
	SimpleVar()
	    : SeExprVarRef(SeExprType::FP1Type_varying()), val(0.0)
	{}

        double val; // independent variable
        void eval(const SeExprVarNode* node,SeVec3d& result)
        {result[0]=val;}
    };
    
    //! independent variable
    mutable SimpleVar x;

    //! resolve function that only supports one external variable 'x'
    SeExprVarRef* resolveVar(const std::string& name) const
    {
        if(name == "x") return &x;
        return 0;
    }
};



int main(int argc,char *argv[])
{
    std::string exprStr="\
                         $val=.5*PI*x;\
                         7*sin(val)/val \
                         ";
    if(argc == 2){
        exprStr=argv[1];
    }
    GrapherExpr expr(exprStr);

    if(!expr.isValid()){
        std::cerr<<"expression failed "<<expr.parseError()<<std::endl;
        exit(1);
    }
    double xmin=-10,xmax=10,ymin=-10,ymax=10;
    int w=60,h=30;
    char *buffer=new char[w*h];
    memset(buffer,(int)' ',w*h);

    // draw x axis
    int j_zero=(-ymin)/(ymax-ymin)*h;
    if(j_zero>=0 && j_zero<h){
        for(int i=0;i<w;i++){
            buffer[i+j_zero*w]='-';
        }
    }
    // draw y axis
    int i_zero=(-xmin)/(xmax-xmin)*w;
    if(i_zero>=0 && i_zero<w){
        for(int j=0;j<h;j++){
            buffer[i_zero+j*w]='|';
        }
    }

    // evaluate the graph
    const int samplesPerPixel=10;
    const double one_over_samples_per_pixel=1./samplesPerPixel;
    for(int i=0;i<w;i++){
        for(int sample=0;sample<samplesPerPixel;sample++){
            // transform from device to logical coordinatex
            double dx=double(sample)*one_over_samples_per_pixel;
            double x=double(dx+i)/double(w)*(xmax-xmin)+xmin;
            // prep the expression engine for evaluation
            expr.setX(x);
            // evaluate and pull scalar value - currently does not work
            //TODO: fix eval and then use actual call
            SeVec3d val=0.0;//expr.evaluate();
            double y=val[0];
            // transform from logical to device coordinate
            int j=(y-ymin)/(ymax-ymin)*h;
            // store to the buffer
            if(j>=0 && j<h)
                buffer[i+j*w]='#';
        }
    }

    // draw the graph from the buffer
    for(int j=h-1;j>=0;j--){
        for(int i=0;i<w;i++){
            std::cout<<buffer[i+j*w];
        }
        std::cout<<std::endl;
    }

    return 0;
}
