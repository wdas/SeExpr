/*
 Copyright Disney Enterprises, Inc.  All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License
 and the following modification to it: Section 6 Trademarks.
 deleted and replaced with:

 6. Trademarks. This License does not grant permission to use the
 trade names, trademarks, service marks, or product names of the
 Licensor and its affiliates, except as required for reproducing
 the content of the NOTICE file.

 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
*/

#include "ExprNode.h"
#include "ExprFuncStandard.h"

namespace SeExpr2 {

ExprType ExprFuncStandard::prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnvBuilder& envBuilder) const {
    if (_funcType < VEC) {
        // scalar argumented functions returning scalars
        //   use promote protocol...

        bool error = false;
        int nonOneDim = 1;  // defaults to 1, if another is seen record!
        bool multiInvoke = !scalarWanted;
        ExprType retType;
        for (int c = 0; c < node->numChildren(); c++) {
            ExprType childType = node->child(c)->prep(scalarWanted, envBuilder);
            int childDim = childType.dim();
            node->child(c)->checkIsFP(childType, error);
            retType.setLifetime(childType);
            if (childDim != 1) {
                if (nonOneDim != 1 && childDim != nonOneDim) multiInvoke = false;
                nonOneDim = childDim;
            }
        }
        if (error)
            return retType.Error();
        else if (multiInvoke && nonOneDim != 1)
            return retType.FP(nonOneDim);
        return retType.FP(1);
    } else {
        // vector argumented functions
        bool error = false;
        ExprType retType;
        for (int c = 0; c < node->numChildren(); c++) {
            ExprType childType = node->child(c)->prep(scalarWanted, envBuilder);
            int childDim = childType.dim();
            node->child(c)->checkIsFP(childType, error);
            node->child(c)->checkCondition(childDim == 1 || childDim == 3, "Expected float or FP[3]", error);
            retType.setLifetime(childType);
        }
        if (error)
            return retType.Error();
        else if (scalarWanted || _funcType < VECVEC)
            return retType.FP(1);
        else
            return retType.FP(3);
    }
}

int Func0Op(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[1]] = ((ExprFuncStandard::Func0*)(c[opData[0]]))();
    return 1;
}
int Func1Op(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[2]] = ((ExprFuncStandard::Func1*)(c[opData[0]]))(fp[opData[1]]);
    return 1;
}
int Func2Op(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[3]] = ((ExprFuncStandard::Func2*)(c[opData[0]]))(fp[opData[1]], fp[opData[2]]);
    return 1;
}
int Func3Op(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[4]] = ((ExprFuncStandard::Func3*)(c[opData[0]]))(fp[opData[1]], fp[opData[2]], fp[opData[3]]);
    return 1;
}
int Func4Op(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[5]] =
        ((ExprFuncStandard::Func4*)(c[opData[0]]))(fp[opData[1]], fp[opData[2]], fp[opData[3]], fp[opData[4]]);
    return 1;
}
int Func5Op(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[6]] = ((ExprFuncStandard::Func5*)(c[opData[0]]))(
        fp[opData[1]], fp[opData[2]], fp[opData[3]], fp[opData[4]], fp[opData[5]]);
    return 1;
}
int Func6Op(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[7]] = ((ExprFuncStandard::Func6*)(c[opData[0]]))(
        fp[opData[1]], fp[opData[2]], fp[opData[3]], fp[opData[4]], fp[opData[5]], fp[opData[6]]);
    return 1;
}
int FuncNOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    int n = opData[1];
    double* vals = static_cast<double*>(alloca(n * sizeof(double)));
    for (int k = 0; k < n; k++) vals[k] = fp[opData[k + 2]];
    double* out = &fp[opData[n + 2]];
    *out = ((ExprFuncStandard::Funcn*)(c[opData[0]]))(n, vals);
    return 1;
}
int Func1VOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[2]] = ((ExprFuncStandard::Func1v*)(c[opData[0]]))(Vec3d::copy(&fp[opData[1]]));
    return 1;
}
int Func2VOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    fp[opData[3]] =
        ((ExprFuncStandard::Func2v*)(c[opData[0]]))(Vec3d::copy(&fp[opData[1]]), Vec3d::copy(&fp[opData[2]]));
    return 1;
}
int Func1VVOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    Vec3d v = ((ExprFuncStandard::Func1vv*)(c[opData[0]]))(Vec3d::copy(&fp[opData[1]]));
    double* out = &fp[opData[2]];
    for (int k = 0; k < 3; k++) out[k] = v[k];
    return 1;
}
int Func2VVOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    Vec3d v = ((ExprFuncStandard::Func2vv*)(c[opData[0]]))(Vec3d::copy(&fp[opData[1]]), Vec3d::copy(&fp[opData[2]]));
    double* out = &fp[opData[3]];
    for (int k = 0; k < 3; k++) out[k] = v[k];
    return 1;
}
int FuncNVOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    int n = opData[1];
    Vec3d* vals = static_cast<Vec3d*>(alloca(n * sizeof(Vec3d)));
    for (int k = 0; k < n; k++) new (vals + k) Vec3d(Vec3dRef(&fp[opData[k + 2]]));  // placement new!
    double* out = &fp[opData[n + 2]];
    *out = ((ExprFuncStandard::Funcnv*)(c[opData[0]]))(n, vals);
    return 1;
}
int FuncNVVOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    int n = opData[1];
    Vec3d* vals = static_cast<Vec3d*>(alloca(n * sizeof(Vec3d)));
    for (int k = 0; k < n; k++) new (vals + k) Vec3d(Vec3dRef(&fp[opData[k + 2]]));  // placement new!
    double* out = &fp[opData[n + 2]];
    Vec3d val = ((ExprFuncStandard::Funcnvv*)(c[opData[0]]))(n, vals);
    for (int k = 0; k < 3; k++) out[k] = val[k];
    return 1;
}

int ExprFuncStandard::buildInterpreter(const ExprFuncNode* node, Interpreter* interpreter) const {
    std::vector<int> argOps;
    for (int c = 0; c < node->numChildren(); c++) {
        int op = node->child(c)->buildInterpreter(interpreter);
        argOps.push_back(op);
    }
    int retOp = -1;

    int funcPtrLoc = interpreter->allocPtr();
    interpreter->s[funcPtrLoc] = (char*)_func;

    Interpreter::OpF op = 0;
    switch (_funcType) {
        case FUNC0:
            op = Func0Op;
            break;
        case FUNC1:
            op = Func1Op;
            break;
        case FUNC2:
            op = Func2Op;
            break;
        case FUNC3:
            op = Func3Op;
            break;
        case FUNC4:
            op = Func4Op;
            break;
        case FUNC5:
            op = Func5Op;
            break;
        case FUNC6:
            op = Func6Op;
            break;
        case FUNCN:
            op = FuncNOp;
            break;
        case FUNC1V:
            op = Func1VOp;
            break;
        case FUNC2V:
            op = Func2VOp;
            break;
        case FUNCNV:
            op = FuncNVOp;
            break;
        case FUNC1VV:
            op = Func1VVOp;
            break;
        case FUNC2VV:
            op = Func2VVOp;
            break;
        case FUNCNVV:
            op = FuncNVVOp;
            break;
        default:
            assert(false);
    }

    if (_funcType < VEC) {
        retOp = interpreter->allocFP(node->type().dim());
        for (int k = 0; k < node->type().dim(); k++) {
            interpreter->addOp(op);
            interpreter->addOperand(funcPtrLoc);
            if (_funcType == FUNCN) interpreter->addOperand(argOps.size());
            for (size_t c = 0; c < argOps.size(); c++) {
                if (node->child(c)->type().isFP(1))
                    interpreter->addOperand(argOps[c]);
                else
                    interpreter->addOperand(argOps[c] + k);
            }
            interpreter->addOperand(retOp + k);
            interpreter->endOp();
        }
    } else {
        // do any promotions that are necessary
        for (size_t c = 0; c < argOps.size(); c++)
            if (node->child(c)->type().dim() == 1) {
                int promotedArgOp = interpreter->allocFP(3);
                interpreter->addOp(Promote<3>::f);
                interpreter->addOperand(argOps[c]);
                interpreter->addOperand(promotedArgOp);
                interpreter->endOp();
                argOps[c] = promotedArgOp;
            }
        retOp = interpreter->allocFP(_funcType >= VECVEC ? 3 : 1);

        interpreter->addOp(op);
        interpreter->addOperand(funcPtrLoc);
        if (_funcType == FUNCNV || _funcType == FUNCNVV) interpreter->addOperand(argOps.size());
        for (size_t c = 0; c < argOps.size(); c++) {
            interpreter->addOperand(argOps[c]);
        }
        interpreter->addOperand(retOp);
        interpreter->endOp();
    }
    if (Expression::debugging) {
        std::cerr << "Interpreter dump" << std::endl;
        interpreter->print();
    }
    return retOp;
}
}
