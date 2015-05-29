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

#include "ExprNode.h"
#include "ExprFuncStandard.h"

namespace SeExpr2 {

#ifdef SEEXPR_DEBUG
static const bool debugMode=true;
#else
static const bool debugMode=false;
#endif

ExprType ExprFuncStandard::prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnv & env) const
{
    if(_funcType<VEC){
        // scalar argumented functions returning scalars
        //   use promote protocol...

        bool error=false;
        int nonOneDim=1; // defaults to 1, if another is seen record!
        bool multiInvoke=!scalarWanted;
        ExprType retType;
        for(int c=0;c<node->numChildren();c++){
            ExprType childType = node->child(c)->prep(scalarWanted, env);
            int childDim=childType.dim();
            node->child(c)->checkIsFP(childType,error);
            retType.setLifetime(childType);
            if(childDim != 1){
                if(nonOneDim!=1 && childDim!=nonOneDim) multiInvoke=false;
                nonOneDim=childDim;
            }
        }
        if(error) return retType.Error();
        else if(multiInvoke && nonOneDim!=1) return retType.FP(nonOneDim);
        return retType.FP(1);
    }else{
        // vector argumented functions
        bool error=false;
        ExprType retType;
        for(int c=0;c<node->numChildren();c++){
            ExprType childType = node->child(c)->prep(scalarWanted, env);
            int childDim=childType.dim();
            node->child(c)->checkIsFP(childType,error);
            node->child(c)->checkCondition(childDim == 1 || childDim == 3,"Expected float or FP[3]",error);
            retType.setLifetime(childType);
        }
        if(error) return retType.Error();
        else if(scalarWanted || _funcType<VECVEC) return retType.FP(1);
        else return retType.FP(3);
    }
}

int Func0Op(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[1]]=((ExprFuncStandard::Func0*)(c[opData[0]]))();
    return 1;
}
int Func1Op(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[2]]=((ExprFuncStandard::Func1*)(c[opData[0]]))(fp[opData[1]]);
    return 1;
}
int Func2Op(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[3]]=((ExprFuncStandard::Func2*)(c[opData[0]]))(fp[opData[1]],fp[opData[2]]);
    return 1;
}
int Func3Op(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[4]]=((ExprFuncStandard::Func3*)(c[opData[0]]))(fp[opData[1]],fp[opData[2]],fp[opData[3]]);
    return 1;
}
int Func4Op(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[5]]=((ExprFuncStandard::Func4*)(c[opData[0]]))(fp[opData[1]],fp[opData[2]],fp[opData[3]],fp[opData[4]]);
    return 1;
}
int Func5Op(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[6]]=((ExprFuncStandard::Func5*)(c[opData[0]]))(fp[opData[1]],fp[opData[2]],fp[opData[3]],fp[opData[4]],fp[opData[5]]);
    return 1;
}
int Func6Op(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[7]]=((ExprFuncStandard::Func6*)(c[opData[0]]))(fp[opData[1]],fp[opData[2]],fp[opData[3]],fp[opData[4]],fp[opData[5]],fp[opData[6]]);
    return 1;
}
int FuncNOp(int* opData,double* fp,char** c,std::vector<int>& callStack){
    int n=opData[1];
    double *vals=static_cast<double*>(alloca(n*sizeof(double)));
    for(int k=0;k<n;k++) vals[k]=fp[opData[k+2]];
    double* out=&fp[opData[n+2]];
    *out=((ExprFuncStandard::Funcn*)(c[opData[0]]))(n,vals);
    return 1;
}
int Func1VOp(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[2]]=((ExprFuncStandard::Func1v*)(c[opData[0]]))(Vec3d(&fp[opData[1]]));
    return 1;
}
int Func2VOp(int* opData,double* fp,char** c,std::vector<int>& callStack){
    fp[opData[3]]=((ExprFuncStandard::Func2v*)(c[opData[0]]))(Vec3d(&fp[opData[1]]),Vec3d(&fp[opData[2]]));
    return 1;
}
int Func1VVOp(int* opData,double* fp,char** c,std::vector<int>& callStack){
    Vec3d v=((ExprFuncStandard::Func1vv*)(c[opData[0]]))(Vec3d(&fp[opData[1]]));
    double* out=&fp[opData[2]];
    for(int k=0;k<3;k++) out[k]=v[k];
    return 1;
}
int Func2VVOp(int* opData,double* fp,char** c,std::vector<int>& callStack){
    Vec3d v=((ExprFuncStandard::Func2vv*)(c[opData[0]]))(Vec3d(&fp[opData[1]]),Vec3d(&fp[opData[2]]));
    double* out=&fp[opData[3]];
    for(int k=0;k<3;k++) out[k]=v[k];
    return 1;
}
int FuncNVOp(int* opData,double* fp,char** c,std::vector<int>& callStack){
    int n=opData[1];
    Vec3d *vals=static_cast<Vec3d*>(alloca(n*sizeof(Vec3d)));
    for(int k=0;k<n;k++) new (vals[k]) Vec3d(&fp[opData[k+2]]); // placement new!
    double* out=&fp[opData[n+2]];
    *out=((ExprFuncStandard::Funcnv*)(c[opData[0]]))(n,vals);
    return 1;
}
int FuncNVVOp(int* opData,double* fp,char** c,std::vector<int>& callStack){
    int n=opData[1];
    Vec3d *vals=static_cast<Vec3d*>(alloca(n*sizeof(Vec3d)));
    for(int k=0;k<n;k++) new (vals[k]) Vec3d(&fp[opData[k+2]]); // placement new!
    double* out=&fp[opData[n+2]];
    Vec3d val=((ExprFuncStandard::Funcnvv*)(c[opData[0]]))(n,vals);
    for(int k=0;k<3;k++) out[k]=val[k];
    return 1;
}


int ExprFuncStandard::buildInterpreter(const ExprFuncNode* node,Interpreter* interpreter) const
{
    std::vector<int> argOps;
    for(int c=0;c<node->numChildren();c++){
        int op=node->child(c)->buildInterpreter(interpreter);
        argOps.push_back(op);
    }
    int retOp=-1;

    int funcPtrLoc=interpreter->allocPtr();
    interpreter->s[funcPtrLoc]=(char*)_func;

    Interpreter::OpF op=0;
    switch(_funcType){
        case FUNC0: op=Func0Op;break;
        case FUNC1: op=Func1Op;break;
        case FUNC2: op=Func2Op;break;
        case FUNC3: op=Func3Op;break;
        case FUNC4: op=Func4Op;break;
        case FUNC5: op=Func5Op;break;
        case FUNC6: op=Func6Op;break;
        case FUNCN: op=FuncNOp;break;
        case FUNC1V: op=Func1VOp;break;
        case FUNC2V: op=Func2VOp;break;
        case FUNCNV: op=FuncNVOp;break;
        case FUNC1VV: op=Func1VVOp;break;
        case FUNC2VV: op=Func2VVOp;break;
        case FUNCNVV: op=FuncNVVOp;break;
        default:assert(false);
    }

    if(debugMode)
        std::cerr<<"we have arg ops "<<argOps.size()<<std::endl;
    if(_funcType<VEC){
        retOp=interpreter->allocFP(node->type().dim());
        for(int k=0;k<node->type().dim();k++){
            interpreter->addOp(op);
            interpreter->addOperand(funcPtrLoc);
            if(_funcType==FUNCN)
                interpreter->addOperand(argOps.size());
            for(size_t c=0;c<argOps.size();c++){
                if(node->child(c)->type().isFP(1)) interpreter->addOperand(argOps[c]);
                else interpreter->addOperand(argOps[c]+k);
            }
            interpreter->addOperand(retOp+k);
            interpreter->endOp();
        }
    }else{
        // do any promotions that are necessary
        for(size_t c=0;c<argOps.size();c++) if(node->child(c)->type().dim()==1){
                int promotedArgOp=interpreter->allocFP(3);
                interpreter->addOp(Promote<3>::f);
                interpreter->addOperand(argOps[c]);
                interpreter->addOperand(promotedArgOp);
                interpreter->endOp();
                argOps[c]=promotedArgOp;
            }
        retOp=interpreter->allocFP(_funcType >= VECVEC ? 3 : 1);

        interpreter->addOp(op);
        interpreter->addOperand(funcPtrLoc);
        if(_funcType==FUNCNV || _funcType==FUNCNVV)
            interpreter->addOperand(argOps.size());
        for(size_t c=0;c<argOps.size();c++){
            interpreter->addOperand(argOps[c]);
        }
        interpreter->addOperand(retOp);
        interpreter->endOp();
    }
    if(debugMode)
        interpreter->print();
    return retOp;
}

}
