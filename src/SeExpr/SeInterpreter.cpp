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
#include <cstdio>

// TODO: optimize to write to location directly on a CondNode

void SeInterpreter::eval()
{
    double* fp=&d[0];
    char** str=&s[0];
    int pc=0;
    int end=ops.size();
    while(pc<end){
//        std::cerr<<"Running op at "<<pc<<std::endl;
        const std::pair<OpF,int>& op=ops[pc];
        int* opCurr=&opData[0]+op.second;
        pc+=op.first(opCurr,fp,str);
    }
}

void SeInterpreter::print()
{
    std::cerr<<"---- ops     ----------------------"<<std::endl;
    for(size_t i=0;i<ops.size();i++){
        fprintf(stderr,"0x%08x (",ops[i].first);
        int nextGuy=(i==ops.size()-1 ? opData.size() : ops[i+1].second);
                
        for(int k=ops[i].second;k<nextGuy;k++){
            fprintf(stderr," %d",opData[k]);
        }
        fprintf(stderr,")\n");
    }
    std::cerr<<"---- opdata  ----------------------"<<std::endl;
    for(size_t k=0;k<opData.size();k++){
        std::cerr<<"opData["<<k<<"]= "<<opData[k]<<std::endl;;
    }
    std::cerr<<"----- fp --------------------------"<<std::endl;
    for(size_t k=0;k<d.size();k++){
        std::cerr<<"fp["<<k<<"]= "<<d[k]<<std::endl;;
    }
    std::cerr<<"---- str     ----------------------"<<std::endl;
    for(size_t k=0;k<s.size();k++){
        std::cerr<<"s["<<k<<"]= 0x"<<s[k]<<" '"<<s[k][0]<<s[k][1]<<s[k][2]<<s[k][3]<<"'..."<<std::endl;;
    }

}



//template SeInterpreter::OpF* getTemplatizedOp<Promote<1> >(int);
//template SeInterpreter::OpF* getTemplatizedOp<Promote<2> >(int);
//template SeInterpreter::OpF* getTemplatizedOp<Promote<3> >(int);

//! Return the function f encapsulated in class T for the dynamic i converted to a static d. (partial application of template using c)
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
    return 0;
}

namespace{

//! Computes a binary op of vector dimension d
template<char op,int d>
struct BinaryOp
{
    static double niceMod(double a, double b){
        if (b == 0) return 0;
        return a - floor(a/b)*b;
    }
    
    static int f(int* opData,double* fp,char** c){
        double* in1=fp+opData[0];
        double* in2=fp+opData[1];
        double* out=fp+opData[2];

        for(int k=0;k<d;k++){
            switch(op){
                case '+': *out=(*in1)+(*in2); break;
                case '-': *out=(*in1)-(*in2); break;
                case '*': *out=(*in1)*(*in2); break;
                case '/': *out=(*in1)/(*in2); break;
                case '%': *out=niceMod(*in1,*in2); break;
                case '^': *out=pow(*in1,*in2); break;
                // these only make sense with d==1
                case '<': *out=(*in1) < (*in2);break;
                case '>': *out=(*in1) > (*in2);break;
                case 'l': *out=(*in1) <= (*in2);break;
                case 'g': *out=(*in1) >= (*in2);break;
                case '&': *out=(*in1) && (*in2);break;
                case '|': *out=(*in1) || (*in2);break;
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

//! Assigns a string from one position to another
struct AssignStrOp{
    static int f(int* opData,double* fp,char** c)
    {
        int in=opData[0];
        int out=opData[1];
        c[out]=c[in];
        return 1;
    }
};

//! Jumps relative to current executing pc if cond is true
struct CondJmpRelative{
    static int f(int* opData,double* fp,char** c){
        bool cond=(bool)fp[opData[0]];
        if(!cond) return opData[1];
        else return 1;
    }
};


//! Jumps relative to current executing pc unconditionally
struct JmpRelative{
    static int f(int* opData,double* fp,char** c){
        return opData[0];
    }
};

//! Evaluates an external variable
struct EvalVar{
    static int f(int* opData,double* fp,char** c){
        SeExprVarRef* ref=reinterpret_cast<SeExprVarRef*>(c[opData[0]]);
        ref->eval(fp+opData[1]); // ,c+opData[1]);
        return 1;
    }
};

template<char op,int d>
struct CompareEqOp{
    static int f(int* opData,double* fp,char** c){
        bool result=true;
        double* in0=fp+opData[0];
        double* in1=fp+opData[1];
        double* out=fp+opData[2];
        for(int k=0;k<d;k++){
            switch(op){ 
                case '=': result &= (*in0) == (*in1);break;
                case '!': result &= (*in0) != (*in1);break;
                default:assert(false);
            }
            in0++;in1++;
        }
        *out=result;
        return 1;
    }
};

template<char op>
struct CompareEqOp<op,3>{
    static int f(int* opData,double* fp,char** c){
        bool eq=fp[opData[0]]==fp[opData[1]] && 
            fp[opData[0]+1]==fp[opData[1]+1] && 
            fp[opData[0]+2]==fp[opData[1]+2];
        if(op=='=') fp[opData[2]]=eq;
        if(op=='!') fp[opData[2]]=!eq;
        return 1;
    }
};



template<char op,int d>
struct StrCompareEqOp{
    // TODO: this should rely on tokenization and not use strcmp
    static int f(int* opData,double* fp,char** c){
        switch(op){
            case '=': fp[opData[2]] = strcmp(c[opData[0]],c[opData[1]])==0;break;
            case '!': fp[opData[2]] = strcmp(c[opData[0]],c[opData[1]])==0;break;
        }
        return 1;
    }
};

}

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

int SeExprStrNode::buildInterpreter(SeInterpreter *interpreter) const
{
    int loc=interpreter->allocPtr();
    interpreter->s[loc]=const_cast<char*>(_str.c_str());
    return loc;
}

int SeExprVecNode::buildInterpreter(SeInterpreter* interpreter) const
{
    std::vector<int> locs;
    for(int k=0;k<numChildren();k++){
        const SeExprNode* c=child(k);
        locs.push_back(c->buildInterpreter(interpreter));
    }
    interpreter->addOp(getTemplatizedOp<Tuple>(numChildren()));
    for(int k=0;k<numChildren();k++) interpreter->addOperand(locs[k]);
    int loc=interpreter->allocFP(numChildren());
    interpreter->addOperand(loc);
    interpreter->endOp();
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
            interpreter->endOp();
        }
        if(dim1 != dimout){
            interpreter->addOp(getTemplatizedOp<Promote>(dimout));
            int promoteOp1=interpreter->allocFP(dimout);
            interpreter->addOperand(op1);
            interpreter->addOperand(promoteOp1);
            op1=promoteOp1;
            interpreter->endOp();
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
    interpreter->endOp();

    return op2;
}

int SeExprUnaryOpNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprNode* child0=child(0);
    int dimout=type().dim(); 
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
    interpreter->endOp();
    
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
    interpreter->endOp();
    return op2;
}

int SeExprVarNode::buildInterpreter(SeInterpreter* interpreter) const
{
    if(const SeExprLocalVar* var=_localVar){
        if(const SeExprLocalVar* phi=var->getPhi()) var=phi;
        SeInterpreter::VarToLoc::iterator i=interpreter->varToLoc.find(var);
        if(i!=interpreter->varToLoc.end()) return i->second;
        assert(false);
    }else if(const SeExprVarRef* var=_var){
        SeExprType type=var->type();
        int varRefLoc=interpreter->allocPtr();
        int destLoc=-1;
        if(type.isFP()){
            int dim=type.dim();
            destLoc=interpreter->allocFP(dim);
        }else destLoc=interpreter->allocPtr();

        interpreter->addOp(EvalVar::f);
        interpreter->s[varRefLoc]=const_cast<char*>(reinterpret_cast<const char*>(var));
        interpreter->addOperand(varRefLoc);
        interpreter->addOperand(destLoc);
        interpreter->endOp();
        return destLoc;

    }
    return -1;
}

int SeExprAssignNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprLocalVar* var=_localVar;
    if(const SeExprLocalVar* phi=var->getPhi()) var=phi;
    SeInterpreter::VarToLoc::iterator i=interpreter->varToLoc.find(var);

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
        interpreter->endOp();

        return loc;
    }else if(child0Type.isString()){
        int loc=-1;
        if(i==interpreter->varToLoc.end()) loc=interpreter->varToLoc[var]=interpreter->allocPtr();
        else loc=i->second;
        
        int op0=child(0)->buildInterpreter(interpreter);
        interpreter->addOp(AssignStrOp::f);
        interpreter->addOperand(op0);
        interpreter->addOperand(loc);
        interpreter->endOp();
        return loc;
        
    }

    assert(false);
    return -1;
}

int SeExprIfThenElseNode::buildInterpreter(SeInterpreter *interpreter) const
{
    int condop=child(0)->buildInterpreter(interpreter);
    int basePC=interpreter->nextPC();
    
    interpreter->addOp(CondJmpRelative::f);
    interpreter->addOperand(condop);
    int destFalse=interpreter->addOperand(0);
    interpreter->endOp();

    child(1)->buildInterpreter(interpreter);

    interpreter->addOp(JmpRelative::f);
    int destEnd=interpreter->addOperand(0);
    interpreter->endOp();

    int child2PC=interpreter->nextPC();

    child(2)->buildInterpreter(interpreter);
    
    interpreter->opData[destFalse]=child2PC-basePC;
    interpreter->opData[destEnd]=interpreter->nextPC()-(child2PC-1);

    return -1;
}

int SeExprCompareNode::buildInterpreter(SeInterpreter *interpreter) const
{
    const SeExprNode* child0=child(0),*child1=child(1);
    assert(type().dim()==1 && type().isFP());
    int op0=child0->buildInterpreter(interpreter);
    int op1=child1->buildInterpreter(interpreter);
    switch(_op){
        case '<': interpreter->addOp(getTemplatizedOp2<'<',BinaryOp>(1));break;
        case '>': interpreter->addOp(getTemplatizedOp2<'>',BinaryOp>(1));break;
        case 'l': interpreter->addOp(getTemplatizedOp2<'l',BinaryOp>(1));break;
        case 'g': interpreter->addOp(getTemplatizedOp2<'g',BinaryOp>(1));break;
        case '&': interpreter->addOp(getTemplatizedOp2<'&',BinaryOp>(1));break;
        case '|': interpreter->addOp(getTemplatizedOp2<'|',BinaryOp>(1));break;
        default: assert(false);
    }
    int op2=interpreter->allocFP(1);
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->addOperand(op2);
    interpreter->endOp();
    return op2;
}

int SeExprCompareEqNode::buildInterpreter(SeInterpreter* interpreter) const
{
    const SeExprNode* child0=child(0),*child1=child(1);
    int op0=child0->buildInterpreter(interpreter);
    int op1=child1->buildInterpreter(interpreter);

    if(child0->type().isFP()){
        int dim0=child0->type().dim(),dim1=child1->type().dim();
        int dimCompare=std::max(dim0,dim1);
        if(dimCompare>1){
            if(dim0 == 1){
                interpreter->addOp(getTemplatizedOp<Promote>(dim1));
                int promotedOp0=interpreter->allocFP(dim1);
                interpreter->addOperand(op0);
                interpreter->addOperand(promotedOp0);
                op0=promotedOp0;
            }
            if(dim1 == 1){
                interpreter->addOp(getTemplatizedOp<Promote>(dim0));
                int promotedOp1=interpreter->allocFP(dim0);
                interpreter->addOperand(op1);
                interpreter->addOperand(promotedOp1);
                op1=promotedOp1;
            }
        }
        interpreter->addOp(getTemplatizedOp2<'=',CompareEqOp>(dimCompare));
    }else if(child0->type().isString())
        interpreter->addOp(getTemplatizedOp2<'=',StrCompareEqOp>(1));
    else assert(false);
    int op2=interpreter->allocFP(1);
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->addOperand(op2);
    interpreter->endOp();
    return op2;
}

int SeExprCondNode::buildInterpreter(SeInterpreter *interpreter) const
{
    int opOut=-1;
    // TODO: handle strings!
    int dimout=type().dim();

    // conditional
    int condOp=child(0)->buildInterpreter(interpreter);
    int basePC=(interpreter->nextPC());
    interpreter->addOp(CondJmpRelative::f);
    interpreter->addOperand(condOp);
    int destFalse=interpreter->addOperand(0);
    interpreter->endOp();

    // true way of working
    int op1=child(1)->buildInterpreter(interpreter);
    if(type().isFP())interpreter->addOp(getTemplatizedOp<AssignOp>(dimout));
    else if(type().isString()) interpreter->addOp(AssignStrOp::f);
    else assert(false);
    interpreter->addOperand(op1);
    int dataOutTrue=interpreter->addOperand(-1);
    // jump past false way of working
    interpreter->addOp(JmpRelative::f);
    int destEnd=interpreter->addOperand(0);
    interpreter->endOp();

    // record start of false condition
    int child2PC=interpreter->nextPC();

    // false way of working
    int op2=child(2)->buildInterpreter(interpreter);
    if(type().isFP())interpreter->addOp(getTemplatizedOp<AssignOp>(dimout));
    else if(type().isString()) interpreter->addOp(AssignStrOp::f);
    else assert(false);
    interpreter->addOperand(op2);
    int dataOutFalse=interpreter->addOperand(-1);
    interpreter->endOp();

    // patch up relative jumps
    interpreter->opData[destFalse]=child2PC-basePC;
    interpreter->opData[destEnd]=interpreter->nextPC()-(child2PC-1);

    // allocate output
    if(type().isFP()) opOut=interpreter->allocFP(type().dim());
    else if(type().isString()) opOut=interpreter->allocPtr();
    else assert(false);
        
    // patch outputs on assigns in each condition
    interpreter->opData[dataOutTrue]=opOut;
    interpreter->opData[dataOutFalse]=opOut;

    return opOut;
}

int SeExprBlockNode::buildInterpreter(SeInterpreter *interpreter) const
{
    assert(numChildren()==2);
    child(0)->buildInterpreter(interpreter);
    return child(1)->buildInterpreter(interpreter);
}

int SeExprModuleNode::buildInterpreter(SeInterpreter *interpreter) const
{
    int lastIdx=0;
    for(int c=0;c<numChildren();c++)
        lastIdx=child(c)->buildInterpreter(interpreter);
    return lastIdx;
}
