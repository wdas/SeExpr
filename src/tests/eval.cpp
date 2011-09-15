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
struct Evaluator
{
    std::vector<double> d;
    std::vector<char*> s;
    std::vector<int> opData;
    
    typedef int(*OpF)(int*,double*,char**);

    std::vector<std::pair<OpF,int> > ops;

    int allocFP(int n)
    {int ret=d.size();for(int k=0;k<n;k++) d.push_back(0);return ret;}

    int allocPtr()
    {int ret=s.size();s.push_back(0);return ret;}

    void eval(){
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

    void print(){
        std::cerr<<"ops "<<std::endl;
        for(int i=0;i<ops.size();i++){
            std::cerr<<ops[i].first<<" (";
            int nextGuy=(i==ops.size()-1 ? opData.size() : ops[i+1].second);
                
            for(int k=ops[i].second;k<nextGuy;k++){
                std::cerr<<" "<<opData[k];
            }
            std::cerr<<")"<<std::endl;
        }
        std::cerr<<"opdata "<<std::endl;
        for(int k=0;k<opData.size();k++){
            std::cerr<<"opData["<<k<<"]= "<<opData[k]<<std::endl;;
        }
        std::cerr<<"fp "<<std::endl;
        for(int k=0;k<d.size();k++){
            std::cerr<<"fp["<<k<<"]= "<<d[k]<<std::endl;;
        }
    }
};



//! Return the function f encapsulated in class T for the dynamic i converted to a static d.
template<template<int d> class T>
static Evaluator::OpF getTemplatizedOp(int i)
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
static Evaluator::OpF getTemplatizedOp2(int i)
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
                default: break;
            }
            in1++;in2++;out++;
        }
        return 1;
    }
};
   

template<char op,int d>
struct UnaryOp{
    static int f(int* opData,double* fp,char** c)
    {
        double* in=fp+opData[0];
        double* out=fp+opData[1];
        for(int k=0;k<d;k++){
            switch(op){
                case '-': *out=-(*in);
                case '~': *out=1-(*in);
                case '!': *out=!*in;
            }
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


#define DEBUG(x) \
    std::cerr<<"Builder: "<<x<<std::endl;


class EvalBuilder:public SeExpr::Examiner<true>
{
public:
    typedef std::map<const SeExprNode*,int> NodeToLoc;
    NodeToLoc location;
    typedef std::map<const SeExprVarRef*,int> VarToLoc;
    VarToLoc varLocation;
    Evaluator eval;

    int getLoc(const SeExprNode* node)
    {
        NodeToLoc::iterator i=location.find(node);
        if(i==location.end()){
            int loc=0;
            if(node->type().isString()) loc=eval.allocPtr();
            else if(node->type().isFP()) loc=eval.allocFP(node->type().dim());
            else assert(false); //  This should not be possible by type check
            location[node]=loc;
            return loc;
        }
        return i->second;
    }


    bool examine(const SeExprNode*) {return true;}

    void post(const SeExprNode* examinee)
    {
        if(const SeExprNumNode* num=dynamic_cast<const SeExprNumNode*>(examinee)){
            DEBUG("num");
            int loc=getLoc(num);
            eval.d[loc]=num->value();
            location[num]=loc;
        }else if(const SeExprVecNode* node=dynamic_cast<const SeExprVecNode*>(examinee)){
            eval.ops.push_back(std::make_pair(getTemplatizedOp<Tuple>(node->numChildren()),eval.opData.size()));
            for(int k=0;k<node->numChildren();k++){
                const SeExprNode* child=node->child(k);
                eval.opData.push_back(getLoc(child));
            }
            eval.opData.push_back(getLoc(node));
        }else if(const SeExprBinaryOpNode* node=dynamic_cast<const SeExprBinaryOpNode*>(examinee)){
            DEBUG("binop");
            const SeExprNode* child0=node->child(0),*child1=node->child(1);
            int dim0=child0->type().dim(),dim1=child1->type().dim(),dimout=node->type().dim(); 
            int op0=getLoc(child0);
            int op1=getLoc(child1);
            if(dimout>1){
                if(dim0 != dimout){
                    eval.ops.push_back(std::make_pair(getTemplatizedOp<Promote>(dimout),eval.opData.size()));
                    int promoteOp0=eval.allocFP(dimout);
                    eval.opData.push_back(op0);
                    eval.opData.push_back(promoteOp0);
                    op0=promoteOp0;
                }
                if(dim1 != dimout){
                    eval.ops.push_back(std::make_pair(getTemplatizedOp<Promote>(dimout),eval.opData.size()));
                    int promoteOp1=eval.allocFP(dimout);
                    eval.opData.push_back(op1);
                    eval.opData.push_back(promoteOp1);
                    op1=promoteOp1;
                }
            }

            switch(node->_op){
                case '+': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'+',BinaryOp>(dimout),eval.opData.size()));break;
                case '-': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'-',BinaryOp>(dimout),eval.opData.size()));break;
                case '*': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'*',BinaryOp>(dimout),eval.opData.size()));break;
                case '/': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'/',BinaryOp>(dimout),eval.opData.size()));break;
                case '^': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'^',BinaryOp>(dimout),eval.opData.size()));break;
                case '%': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'%',BinaryOp>(dimout),eval.opData.size()));break;
                default: assert(false);
            }
            int op2=getLoc(node);
            eval.opData.push_back(op0);
            eval.opData.push_back(op1);
            eval.opData.push_back(op2);
        }else if(const SeExprUnaryOpNode* node=dynamic_cast<const SeExprUnaryOpNode*>(examinee)){
            DEBUG("unaryop");
            const SeExprNode* child0=node->child(0);
            int dim0=child0->type().dim(),dimout=node->type().dim(); 
            int op0=getLoc(child0);
            int op1=getLoc(node);
            switch(node->_op){
                case '-': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'-',UnaryOp>(dimout),eval.opData.size()));break;
                case '~': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'~',UnaryOp>(dimout),eval.opData.size()));break;
                case '!': eval.ops.push_back(std::make_pair(getTemplatizedOp2<'!',UnaryOp>(dimout),eval.opData.size()));break;
                default: assert(false);
            }
            eval.opData.push_back(op0);
            eval.opData.push_back(op1);
        }else if(const SeExprAssignNode* node=dynamic_cast<const SeExprAssignNode*>(examinee)){
            //std::cerr<<"storing for var "<<node->var()<<std::endl;
            //varLocation[node->var()]=getLoc(node->child(0)); // just point to other node
        }else if(const SeExprVarNode* node=dynamic_cast<const SeExprVarNode*>(examinee)){
            //std::cerr<<"looking for "<<node->var()<<std::endl;
            VarToLoc::const_iterator it=varLocation.find(node->var());
            if(it==varLocation.end()){
                assert(false);
            }else{
                location[node]=it->second;
            }
        }
    }

    void reset()
    {}
};


class Expr:public SeExpression
{
public:
    void walk()
    {
        EvalBuilder builder;
        SeExpr::ConstWalker walker(&builder);
        if(isValid() && _parseTree){
            walker.walk(_parseTree);
            builder.eval.eval();
            builder.eval.print();
        }
    }
};


int main(int argc,char* argv[])
{
    Expr expr;
    expr.setExpr(argv[1]);
    if(!expr.isValid()){
        std::cerr<<"parse error "<<expr.parseError()<<std::endl;
    }

#if 0
    Evaluator eval;
    int const1=eval.allocFP(2);eval.d[const1]=5;eval.d[const1+1]=1;
    int const3=eval.allocFP(1);eval.d[const3]=7;
    // promote result
    int promote1=eval.allocFP(2);
    int add1=eval.allocFP(2);

    // setup promote
    eval.ops.push_back(std::make_pair(promote<2>,eval.opData.size()));
    eval.opData.push_back(const3); // 3posIn
    eval.opData.push_back(promote1); // posOut

    // setup add
    eval.ops.push_back(std::make_pair(binaryOp<2,'*'>,eval.opData.size()));
    eval.opData.push_back(const1);
    eval.opData.push_back(promote1);
    eval.opData.push_back(add1);

    // subscript
    int subscriptConst=eval.allocFP(1);
    int final=eval.allocFP(1);
    eval.d[subscriptConst]=1;

    eval.ops.push_back(std::make_pair(subscript<2>,eval.opData.size()));
    eval.opData.push_back(add1);
    eval.opData.push_back(subscriptConst);
    eval.opData.push_back(final);

    for(int i=1;i<=10000000;i++){
        eval.eval();
    }
    for(int i=0;i<eval.d.size();i++) std::cerr<<"d["<<i<<"]="<<eval.d[i]<<std::endl;

#endif    
    std::cerr<<"fud"<<std::endl;
    expr.walk();

    return 0;
}




