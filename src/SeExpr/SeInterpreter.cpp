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
#include "SeExprNode.h"
#include "SeInterpreter.h"
#include <iostream>

void SeInterpreter::eval()
{
    double* fp=&d[0];
    char** str=&s[0];
    int pc=0;
    int end=ops.size();
    while(pc<end){
        std::cerr<<"Running op at "<<pc<<std::endl;
        const std::pair<OpF,int>& op=ops[pc];
        int* opCurr=&opData[0]+op.second;
        pc+=op.first(opCurr,fp,str);
    }
}

void SeInterpreter::print()
{
    std::cerr<<"ops "<<std::endl;
    for(size_t i=0;i<ops.size();i++){
        std::cerr<<ops[i].first<<" (";
        int nextGuy=(i==ops.size()-1 ? opData.size() : ops[i+1].second);
                
        for(int k=ops[i].second;k<nextGuy;k++){
            std::cerr<<" "<<opData[k];
        }
        std::cerr<<")"<<std::endl;
    }
    std::cerr<<"opdata "<<std::endl;
    for(size_t k=0;k<opData.size();k++){
        std::cerr<<"opData["<<k<<"]= "<<opData[k]<<std::endl;;
    }
    std::cerr<<"fp "<<std::endl;
    for(size_t k=0;k<d.size();k++){
        std::cerr<<"fp["<<k<<"]= "<<d[k]<<std::endl;;
    }
}



//! Return the function f encapsulated in class T for the dynamic i converted to a static d.
template<template<int d> class T>
static SeInterpreter::OpF getTemplatizedOp(int i)
{
    switch(i){
        case 1: return T<1>::f;
        case 2: return T<2>::f;
        case 3: return T<3>::f;
        case 4: return T<4>::f;
        case 5: return T<5>::f;
        case 6: return T<6>::f;
        case 7: return T<7>::f;
        case 8: return T<8>::f;
        case 9: return T<9>::f;
        case 10: return T<10>::f;
        case 11: return T<11>::f;
        case 12: return T<12>::f;
        case 13: return T<13>::f;
        case 14: return T<14>::f;
        case 15: return T<15>::f;
        case 16: return T<16>::f;
        default: assert(false && "Invalid dynamic parameter (not supported template)");break;
    }
}

template<char c,template<char c1,int d> class T>
static SeInterpreter::OpF getTemplatizedOp2(int i)
{
    switch(i){
        case 1: return T<c,1>::f;
        case 2: return T<c,2>::f;
        case 3: return T<c,3>::f;
        case 4: return T<c,4>::f;
        case 5: return T<c,5>::f;
        case 6: return T<c,6>::f;
        case 7: return T<c,7>::f;
        case 8: return T<c,8>::f;
        case 9: return T<c,9>::f;
        case 10: return T<c,10>::f;
        case 11: return T<c,11>::f;
        case 12: return T<c,12>::f;
        case 13: return T<c,13>::f;
        case 14: return T<c,14>::f;
        case 15: return T<c,15>::f;
        case 16: return T<c,16>::f;
        default: assert(false && "Invalid dynamic parameter (not supported template)");break;
    }
}

//! Computes a binary op of vector dimension d
template<char op,int d>
struct BinaryOp
{
    static int f(int* opData,double* fp,char** c)
    {
        double* in1=fp+opData[0];
        double* in2=fp+opData[1];
        double* out=fp+opData[2];

        for(int k=0;k<d;k++){
            switch(op){
                case '+': *out=(*in1)+(*in2); break;
                case '-': *out=(*in1)-(*in2); break;
                case '*': *out=(*in1)*(*in2); break;
                case '/': *out=(*in1)/(*in2); break;
                case '%': *out=fmod(*in1,*in2); break;
                case '^': *out=pow(*in1,*in2); break;
                default: assert(false);
            }
            in1++;in2++;out++;
        }
        return 1;
    }
};
   
/// Computes a unary op on a FP[d]
template<char op,int d>
struct UnaryOp{
    static int f(int* opData,double* fp,char** c)
    {
        double* in=fp+opData[0];
        double* out=fp+opData[1];
        for(int k=0;k<d;k++){
            switch(op){
                case '-': *out=-(*in);break;
                case '~': *out=1-(*in);break;
                case '!': *out=!*in;break;
                default: assert(false);
            }
            std::cerr<<"In "<<*in<<" out "<<*out<<std::endl;
            in++;out++;
        }
        return 1;
    }
};

//! Promotes a FP[1] to FP[d]
template<int d>
struct Promote{
    static int f(int* opData,double* fp,char** c)
    {
        int posIn=opData[0];
        int posOut=opData[1];
        for(int k=posOut;k<posOut+d;k++) fp[k]=fp[posIn];
        return 1;
    }
};

//! Subscripts 
template<int d>
struct Subscript{
    static int f(int* opData,double* fp,char** c)
    {
        int tuple=opData[0];
        int subscript=int(fp[opData[1]]);
        int out=opData[2];
        if(subscript>=d || subscript <0) fp[out]=0;
        else fp[out]=fp[tuple+subscript];
        return 1;
    }
};

//! build a vector tuple from a bunch of numbers
template<int d>
struct Tuple{
    static int f(int* opData,double* fp,char** c)
    {
        int out=opData[d];
        std::cerr<<"we have d "<<d<<std::endl;
        std::cerr<<"we have out "<<out<<std::endl;
        std::cerr<<"we have out "<<fp[opData[0]]<<std::endl;
        std::cerr<<"we have out "<<fp[opData[1]]<<std::endl;
        for(int k=0;k<d;k++){
            fp[out+k]=fp[opData[k]];
        }
        return 1;
    }
};

//! build a vector tuple from a bunch of numbers
template<int d>
struct AssignOp{
    static int f(int* opData,double* fp,char** c)
    {
        int in=opData[0];
        int out=opData[1];
        for(int k=0;k<d;k++){
            fp[out+k]=fp[in+k];
        }
        return 1;
    }
};

int SeExprNode::buildInterpreter(SeInterpreter* interpreter) const
{
    for(int c=0;c<numChildren();c++) child(c)->buildInterpreter(interpreter);
    return -1;
}

int SeExprNumNode::buildInterpreter(SeInterpreter* interpreter) const
{
    int loc=interpreter->allocFP(1);
    interpreter->d[loc]=value();
    return loc;
}

int SeExprVecNode::buildInterpreter(SeInterpreter* interpreter) const
{
    interpreter->addOp(getTemplatizedOp<Tuple>(numChildren()));
    for(int k=0;k<numChildren();k++){
        const SeExprNode* c=child(k);
        int loc=c->buildInterpreter(interpreter);
        interpreter->addOperand(loc);
    }
    int loc=interpreter->allocFP(numChildren());
    interpreter->addOperand(loc);
    return loc;
}

int SeExprBinaryOpNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprNode* child0=child(0),*child1=child(1);
    int dim0=child0->type().dim(),dim1=child1->type().dim(),dimout=type().dim(); 
    int op0=child0->buildInterpreter(interpreter);
    int op1=child1->buildInterpreter(interpreter);
    if(dimout>1){
        if(dim0 != dimout){
            interpreter->addOp(getTemplatizedOp<Promote>(dimout));
            int promoteOp0=interpreter->allocFP(dimout);
            interpreter->addOperand(op0);
            interpreter->addOperand(promoteOp0);
            op0=promoteOp0;
        }
        if(dim1 != dimout){
            interpreter->addOp(getTemplatizedOp<Promote>(dimout));
            int promoteOp1=interpreter->allocFP(dimout);
            interpreter->addOperand(op1);
            interpreter->addOperand(promoteOp1);
            op1=promoteOp1;
        }
    }

    switch(_op){
        case '+': interpreter->addOp(getTemplatizedOp2<'+',BinaryOp>(dimout));break;
        case '-': interpreter->addOp(getTemplatizedOp2<'-',BinaryOp>(dimout));break;
        case '*': interpreter->addOp(getTemplatizedOp2<'*',BinaryOp>(dimout));break;
        case '/': interpreter->addOp(getTemplatizedOp2<'/',BinaryOp>(dimout));break;
        case '^': interpreter->addOp(getTemplatizedOp2<'^',BinaryOp>(dimout));break;
        case '%': interpreter->addOp(getTemplatizedOp2<'%',BinaryOp>(dimout));break;
        default: assert(false);
    }
    int op2=interpreter->allocFP(dimout);
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->addOperand(op2);

    return op2;
}

int SeExprUnaryOpNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprNode* child0=child(0);
    int dim0=child0->type().dim(),dimout=type().dim(); 
    int op0=child0->buildInterpreter(interpreter);

    switch(_op){
        case '-': interpreter->addOp(getTemplatizedOp2<'-',UnaryOp>(dimout));break;
        case '~': interpreter->addOp(getTemplatizedOp2<'~',UnaryOp>(dimout));break;
        case '!': interpreter->addOp(getTemplatizedOp2<'!',UnaryOp>(dimout));break;
        default: assert(false);
    }
    int op1=interpreter->allocFP(dimout);
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    
    return op1;
}

int SeExprSubscriptNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprNode* child0=child(0),*child1=child(1);
    int dimin=child0->type().dim();
    int op0=child0->buildInterpreter(interpreter);
    int op1=child1->buildInterpreter(interpreter);
    int op2=interpreter->allocFP(1);

    interpreter->addOp(getTemplatizedOp<Subscript>(dimin));
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->addOperand(op2);
    return op2;
}

int SeExprVarNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprLocalVar* var=_localVar;
    if(const SeExprLocalVar* phi=var->getPhi()) var=phi;
    SeInterpreter::VarToLoc::iterator i=interpreter->varToLoc.find(_localVar);
    if(i!=interpreter->varToLoc.end()) return i->second;
    assert(false);
    return -1;
}

int SeExprAssignNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprLocalVar* var=_localVar;
    if(const SeExprLocalVar* phi=var->getPhi()) var=phi;
    SeInterpreter::VarToLoc::iterator i=interpreter->varToLoc.find(_localVar);

    SeExprType child0Type=child(0)->type();
    if(child0Type.isFP()){
        int dimout=child0Type.dim();
        int loc=-1;
        if(i==interpreter->varToLoc.end()) loc=interpreter->varToLoc[var]=interpreter->allocFP(dimout);
        else loc=i->second;
        
        int op0=child(0)->buildInterpreter(interpreter);
        interpreter->addOp(getTemplatizedOp<AssignOp>(dimout));
        interpreter->addOperand(op0);
        interpreter->addOperand(loc);
        return loc;
    }else if(child0Type.isString()){
        // TODO: do this4
    }

    assert(false);
    return -1;
}
