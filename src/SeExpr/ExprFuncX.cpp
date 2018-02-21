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
#include <cstdio>
#include <stdexcept>

#include "ExprFunc.h"
#include "ExprFuncX.h"
#include "ExprNode.h"
#include "Interpreter.h"
#include "VarBlock.h"

namespace SeExpr2 {
int ExprFuncSimple::EvalOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    const ExprFuncNode* node = reinterpret_cast<const ExprFuncNode*>(c[opData[0]]);
    ExprFuncSimple* simple =
        const_cast<ExprFuncSimple*>(reinterpret_cast<const ExprFuncSimple*>(node->func()->funcx()));
    ArgHandle args(opData, fp, c, callStack, c[0]);
    simple->eval(args);
    return 1;
}

int ExprFuncSimple::EvalClosureOp(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    const ExprFuncNode* node = reinterpret_cast<const ExprFuncNode*>(c[opData[0]]);

    ExprFuncX** funcs = (ExprFuncX**)c[0];
    const auto* funcSymbol = static_cast<const VarBlockCreator::FuncSymbol*>(node->func()->funcx());
    int offset = funcSymbol->offset();
    ExprFuncSimple* simple = reinterpret_cast<ExprFuncSimple*>(funcs[offset]);
    ArgHandle args(opData, fp, c, callStack, c[0]);

    if (!args.data) {
        args.data = simple->evalConstant(node, args);
    }

    simple->eval(args);
    return 1;
}

int ExprFuncSimple::buildInterpreter(const ExprFuncNode* node, Interpreter* interpreter) const {
    std::vector<int> operands;
    for (int c = 0; c < node->numChildren(); c++) {
        int operand = node->child(c)->buildInterpreter(interpreter);
#if 0
        // debug
        std::cerr<<"we are "<<node->promote(c)<<" "<<c<<std::endl;
#endif
        if (node->promote(c) != 0) {
            interpreter->addOp(getTemplatizedOp<Promote>(node->promote(c)));
            int promotedOperand = interpreter->allocFP(node->promote(c));
            interpreter->addOperand(operand);
            interpreter->addOperand(promotedOperand);
            operand = promotedOperand;
            interpreter->endOp();
        }
        operands.push_back(operand);
    }
    int outoperand = -1;
    int nargsData = interpreter->allocFP(1);
    interpreter->d[nargsData] = node->numChildren();
    if (node->type().isFP())
        outoperand = interpreter->allocFP(node->type().dim());
    else if (node->type().isString())
        outoperand = interpreter->allocPtr();
    else
        assert(false);

    int ptrDataLoc = interpreter->allocPtr();
    const auto* funcSymbol = dynamic_cast<const VarBlockCreator::FuncSymbol*>(node->func()->funcx());
    bool isLateBoundClosure = (bool)funcSymbol;
    if (isLateBoundClosure) {
        interpreter->addOp(EvalClosureOp);
    } else {
        interpreter->addOp(EvalOp);
    }
    int ptrLoc = interpreter->allocPtr();
    interpreter->s[ptrLoc] = (char*)node;
    interpreter->addOperand(ptrLoc);
    interpreter->addOperand(ptrDataLoc);
    interpreter->addOperand(outoperand);
    interpreter->addOperand(nargsData);
    for (size_t c = 0; c < operands.size(); c++) {
        interpreter->addOperand(operands[c]);
    }
    interpreter->endOp(false);  // do not eval because the function may not be evaluatable!

    // call into interpreter eval
    int pc = interpreter->nextPC() - 1;
    int* opCurr = (&interpreter->opData[0]) + interpreter->ops[pc].second;

    ArgHandle args(opCurr, &interpreter->d[0], &interpreter->s[0], interpreter->callStack, nullptr);
    if (!isLateBoundClosure) interpreter->s[ptrDataLoc] = reinterpret_cast<char*>(evalConstant(node, args));

    return outoperand;
}

ExprType ExprFuncSimple::genericPrep(ExprFuncNode* node,
                                     bool scalarWanted,
                                     ExprVarEnvBuilder& env,
                                     const ExprFuncDeclaration& decl) {
    assert(node);

    int nargs = node->numChildren();
    if (nargs < decl.minArgs || nargs > decl.maxArgs) {
        std::stringstream msg;
        msg << "Wrong number of arguments, should be " << decl.minArgs << " to " << decl.maxArgs;
        node->addError(msg.str());
        return SeExpr2::ExprType().Error();
    }
    for (int i = 0; i < nargs; ++i) {
        if (!node->checkArg(i, decl.types[i], env)) {
            return SeExpr2::ExprType().Error();
        }
    }

    return decl.types.back();
}
}

extern "C" {
//            allocate int[4+number of args];
//            allocate char*[2];
//            allocate double[1+ sizeof(ret) + sizeof(args)]
//
//            int[0]= c , 0
//            int[1]= c , 1
//            int[2]= f,  0
//            int[3]= f,  8
//
//            int[4]= f, 8
//            int[5]= f, 9
//
//
//                    double[0] = 0
//                    double[1] = 0
//                    double[2] = 0
//                    double[3] = 0
// opData indexes either into f or into c.
// opdata[0] points to ExprFuncSimple instance
// opdata[1] points to the data generated by evalConstant
// opdata[2] points to return value
// opdata[3] points to number of args
// opdata[4] points to beginning of arguments in
void SeExpr2LLVMEvalCustomFunction(int* opDataArg,
                                   double* fpArg,
                                   char** strArg,
                                   void** funcdata,
                                   const SeExpr2::ExprFuncNode* node,
                                   double** varBlockData) {
    SeExpr2::ExprFuncSimple* funcSimple =
        const_cast<SeExpr2::ExprFuncSimple*>((const SeExpr2::ExprFuncSimple*)node->func()->funcx());

    strArg[0] = reinterpret_cast<char*>(funcSimple);

    std::vector<int> callStack;
    SeExpr2::ExprFuncSimple::ArgHandle handle(opDataArg, fpArg, strArg, callStack, (const char*)varBlockData);
    if (!*funcdata) {
        handle.data = funcSimple->evalConstant(node, handle);
        *funcdata = reinterpret_cast<void*>(handle.data);
    } else {
        handle.data = reinterpret_cast<SeExpr2::ExprFuncNode::Data*>(*funcdata);
    }

    funcSimple->eval(handle);
}
}
