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
#include "Interpreter.h"
#include "VarBlock.h"
#include <iostream>
#include <cstdio>
#include <dlfcn.h>

// TODO: optimize to write to location directly on a CondNode
namespace SeExpr2 {

void Interpreter::eval(VarBlock* block, bool debug) {
    if (block) {
        static_assert(sizeof(char*) == sizeof(size_t), "Expect to fit size_t in char*");
        s[0] = reinterpret_cast<char*>(block->data());
        s[1] = reinterpret_cast<char*>(block->indirectIndex);
    }
    double* fp = &d[0];
    char** str = &s[0];
    int pc = _pcStart;
    int end = ops.size();
    while (pc < end) {
        if (debug) {
            std::cerr << "Running op at " << pc << std::endl;
            print(pc);
        }
        const std::pair<OpF, int>& op = ops[pc];
        int* opCurr = &opData[0] + op.second;
        pc += op.first(opCurr, fp, str, callStack);
    }
}

void Interpreter::print(int pc) const {
    std::cerr << "---- ops     ----------------------" << std::endl;
    for (size_t i = 0; i < ops.size(); i++) {
        Dl_info info;
        const char* name = "";
        if (dladdr((void*)ops[i].first, &info)) name = info.dli_sname;
        fprintf(stderr, "%s %s %p (", pc == (int)i ? "-->" : "   ", name, ops[i].first);
        int nextGuy = (i == ops.size() - 1 ? opData.size() : ops[i + 1].second);
        for (int k = ops[i].second; k < nextGuy; k++) {
            fprintf(stderr, " %d", opData[k]);
        }
        fprintf(stderr, ")\n");
    }
    std::cerr << "---- opdata  ----------------------" << std::endl;
    for (size_t k = 0; k < opData.size(); k++) {
        std::cerr << "opData[" << k << "]= " << opData[k] << std::endl;
        ;
    }
    std::cerr << "----- fp --------------------------" << std::endl;
    for (size_t k = 0; k < d.size(); k++) {
        std::cerr << "fp[" << k << "]= " << d[k] << std::endl;
        ;
    }
    std::cerr << "---- str     ----------------------" << std::endl;
    std::cerr << "s[0] reserved for datablock = " << reinterpret_cast<size_t>(s[0]) << std::endl;
    std::cerr << "s[1] is indirectIndex = " << reinterpret_cast<size_t>(s[1]) << std::endl;
    for (size_t k = 2; k < s.size(); k++) {
        std::cerr << "s[" << k << "]= 0x" << s[k];
        if (s[k]) std::cerr << " '" << s[k][0] << s[k][1] << s[k][2] << s[k][3] << "...'";
        std::cerr << std::endl;
    }
}

// template Interpreter::OpF* getTemplatizedOp<Promote<1> >(int);
// template Interpreter::OpF* getTemplatizedOp<Promote<2> >(int);
// template Interpreter::OpF* getTemplatizedOp<Promote<3> >(int);

//! Return the function f encapsulated in class T for the dynamic i converted to a static d. (partial application of
// template using c)
template <char c, template <char c1, int d> class T>
static Interpreter::OpF getTemplatizedOp2(int i) {
    switch (i) {
        case 1:
            return T<c, 1>::f;
        case 2:
            return T<c, 2>::f;
        case 3:
            return T<c, 3>::f;
        case 4:
            return T<c, 4>::f;
        case 5:
            return T<c, 5>::f;
        case 6:
            return T<c, 6>::f;
        case 7:
            return T<c, 7>::f;
        case 8:
            return T<c, 8>::f;
        case 9:
            return T<c, 9>::f;
        case 10:
            return T<c, 10>::f;
        case 11:
            return T<c, 11>::f;
        case 12:
            return T<c, 12>::f;
        case 13:
            return T<c, 13>::f;
        case 14:
            return T<c, 14>::f;
        case 15:
            return T<c, 15>::f;
        case 16:
            return T<c, 16>::f;
        default:
            assert(false && "Invalid dynamic parameter (not supported template)");
            break;
    }
    return 0;
}

namespace {

//! Computes a binary op of vector dimension d
template <char op, int d>
struct BinaryOp {
    static double niceMod(double a, double b) {
        if (b == 0) return 0;
        return a - floor(a / b) * b;
    }

    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        double* in1 = fp + opData[0];
        double* in2 = fp + opData[1];
        double* out = fp + opData[2];

        for (int k = 0; k < d; k++) {
            switch (op) {
                case '+':
                    *out = (*in1) + (*in2);
                    break;
                case '-':
                    *out = (*in1) - (*in2);
                    break;
                case '*':
                    *out = (*in1) * (*in2);
                    break;
                case '/':
                    *out = (*in1) / (*in2);
                    break;
                case '%':
                    *out = niceMod(*in1, *in2);
                    break;
                case '^':
                    *out = pow(*in1, *in2);
                    break;
                // these only make sense with d==1
                case '<':
                    *out = (*in1) < (*in2);
                    break;
                case '>':
                    *out = (*in1) > (*in2);
                    break;
                case 'l':
                    *out = (*in1) <= (*in2);
                    break;
                case 'g':
                    *out = (*in1) >= (*in2);
                    break;
                case '&':
                    *out = (*in1) && (*in2);
                    break;
                case '|':
                    *out = (*in1) || (*in2);
                    break;
                default:
                    assert(false);
            }
            in1++;
            in2++;
            out++;
        }
        return 1;
    }
};

/// Computes a unary op on a FP[d]
template <char op, int d>
struct UnaryOp {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        double* in = fp + opData[0];
        double* out = fp + opData[1];
        for (int k = 0; k < d; k++) {
            switch (op) {
                case '-':
                    *out = -(*in);
                    break;
                case '~':
                    *out = 1 - (*in);
                    break;
                case '!':
                    *out = !*in;
                    break;
                default:
                    assert(false);
            }
            in++;
            out++;
        }
        return 1;
    }
};

//! Subscripts
template <int d>
struct Subscript {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        int tuple = opData[0];
        int subscript = int(fp[opData[1]]);
        int out = opData[2];
        if (subscript >= d || subscript < 0)
            fp[out] = 0;
        else
            fp[out] = fp[tuple + subscript];
        return 1;
    }
};

//! build a vector tuple from a bunch of numbers
template <int d>
struct Tuple {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        int out = opData[d];
        for (int k = 0; k < d; k++) {
            fp[out + k] = fp[opData[k]];
        }
        return 1;
    }
};

//! Assign a floating point to another (NOTE: if src and dest have different dimensions, use Promote)
template <int d>
struct AssignOp {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        int in = opData[0];
        int out = opData[1];
        for (int k = 0; k < d; k++) {
            fp[out + k] = fp[in + k];
        }
        return 1;
    }
};

//! Assigns a string from one position to another
struct AssignStrOp {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        int in = opData[0];
        int out = opData[1];
        c[out] = c[in];
        return 1;
    }
};

//! Jumps relative to current executing pc if cond is true
struct CondJmpRelativeIfFalse {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        bool cond = (bool)fp[opData[0]];
        if (!cond)
            return opData[1];
        else
            return 1;
    }
};

//! Jumps relative to current executing pc if cond is true
struct CondJmpRelativeIfTrue {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        bool cond = (bool)fp[opData[0]];
        if (cond)
            return opData[1];
        else
            return 1;
    }
};

//! Jumps relative to current executing pc unconditionally
struct JmpRelative {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) { return opData[0]; }
};

//! Evaluates an external variable
struct EvalVar {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        ExprVarRef* ref = reinterpret_cast<ExprVarRef*>(c[opData[0]]);
        ref->eval(fp + opData[1]);  // ,c+opData[1]);
        return 1;
    }
};

//! Evaluates an external variable using a variable block
template <int dim>
struct EvalVarBlock {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        if (c[0]) {
            double* basePointer = reinterpret_cast<double*>(c[0]) + opData[0];
            double* destPointer = fp + opData[1];
            for (int i = 0; i < dim; i++) destPointer[i] = basePointer[i];
        }
        return 1;
    }
};

//! Evaluates an external variable using a variable block
template <char uniform, int dim>
struct EvalVarBlockIndirect {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        if (c[0]) {
            int stride = opData[2];
            int outputVarBlockOffset = opData[0];
            int destIndex = opData[1];
            size_t indirectIndex = reinterpret_cast<size_t>(c[1]);
            double* basePointer =
                reinterpret_cast<double**>(c[0])[outputVarBlockOffset] + (uniform ? 0 : (stride * indirectIndex));
            double* destPointer = fp + destIndex;
            for (int i = 0; i < dim; i++) destPointer[i] = basePointer[i];
        } else {
            // TODO: this happens in initial evaluation!
            // std::cerr<<"Did not get data block"<<std::endl;
            // assert(false && "Did not get data block");
        }
        return 1;
    }
};

template <char op, int d>
struct CompareEqOp {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        bool result = true;
        double* in0 = fp + opData[0];
        double* in1 = fp + opData[1];
        double* out = fp + opData[2];
        for (int k = 0; k < d; k++) {
            switch (op) {
                case '=':
                    result &= (*in0) == (*in1);
                    break;
                case '!':
                    result &= (*in0) != (*in1);
                    break;
                default:
                    assert(false);
            }
            in0++;
            in1++;
        }
        *out = result;
        return 1;
    }
};

template <char op>
struct CompareEqOp<op, 3> {
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        bool eq = fp[opData[0]] == fp[opData[1]] && fp[opData[0] + 1] == fp[opData[1] + 1] &&
                  fp[opData[0] + 2] == fp[opData[1] + 2];
        if (op == '=') fp[opData[2]] = eq;
        if (op == '!') fp[opData[2]] = !eq;
        return 1;
    }
};

template <char op, int d>
struct StrCompareEqOp {
    // TODO: this should rely on tokenization and not use strcmp
    static int f(int* opData, double* fp, char** c, std::vector<int>& callStack) {
        switch (op) {
            case '=':
                fp[opData[2]] = strcmp(c[opData[0]], c[opData[1]]) == 0;
                break;
            case '!':
                fp[opData[2]] = strcmp(c[opData[0]], c[opData[1]]) == 0;
                break;
        }
        return 1;
    }
};
}

namespace {
int ProcedureReturn(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    int newPC = callStack.back();
    callStack.pop_back();
    return newPC - opData[0];
}
}

namespace {
int ProcedureCall(int* opData, double* fp, char** c, std::vector<int>& callStack) {
    callStack.push_back(opData[0]);
    return opData[1];
}
}

int ExprLocalFunctionNode::buildInterpreter(Interpreter* interpreter) const {
    _procedurePC = interpreter->nextPC();
    int lastOperand = 0;
    for (int c = 0; c < numChildren(); c++) lastOperand = child(c)->buildInterpreter(interpreter);
    int basePC = interpreter->nextPC();
    ;
    interpreter->addOp(ProcedureReturn);
    // int endPC =
    interpreter->addOperand(basePC);
    interpreter->endOp(false);
    _returnedDataOp = lastOperand;

    return 0;
}

int ExprLocalFunctionNode::buildInterpreterForCall(const ExprFuncNode* callerNode, Interpreter* interpreter) const {
    std::vector<int> operands;
    for (int c = 0; c < callerNode->numChildren(); c++) {
        const ExprNode* child = callerNode->child(c);
        // evaluate the argument
        int operand = callerNode->child(c)->buildInterpreter(interpreter);
        if (child->type().isFP()) {
            if (callerNode->promote(c) != 0) {
                // promote the argument to the needed type
                interpreter->addOp(getTemplatizedOp<Promote>(callerNode->promote(c)));
                // int promotedOperand=interpreter->allocFP(callerNode->promote(c));
                interpreter->addOperand(operand);
                interpreter->addOperand(prototype()->interpreterOps(c));
                interpreter->endOp();
            } else {
                interpreter->addOp(getTemplatizedOp<AssignOp>(child->type().dim()));
                interpreter->addOperand(operand);
                interpreter->addOperand(prototype()->interpreterOps(c));
                interpreter->endOp();
            }
        } else {
            // TODO: string
            assert(false);
        }
        operands.push_back(operand);
    }
    int outoperand = -1;
    if (callerNode->type().isFP())
        outoperand = interpreter->allocFP(callerNode->type().dim());
    else if (callerNode->type().isString())
        outoperand = interpreter->allocPtr();
    else
        assert(false);

    int basePC = interpreter->nextPC();
    interpreter->addOp(ProcedureCall);
    int returnAddress = interpreter->addOperand(0);
    interpreter->addOperand(_procedurePC - basePC);
    interpreter->endOp(false);
    // set return address
    interpreter->opData[returnAddress] = interpreter->nextPC();

    // TODO: copy result back and string
    interpreter->addOp(getTemplatizedOp<AssignOp>(callerNode->type().dim()));
    interpreter->addOperand(_returnedDataOp);
    interpreter->addOperand(outoperand);
    interpreter->endOp();

    return outoperand;
}

int ExprNode::buildInterpreter(Interpreter* interpreter) const {
    for (int c = 0; c < numChildren(); c++) child(c)->buildInterpreter(interpreter);
    return -1;
}

int ExprNumNode::buildInterpreter(Interpreter* interpreter) const {
    int loc = interpreter->allocFP(1);
    interpreter->d[loc] = value();
    return loc;
}

int ExprStrNode::buildInterpreter(Interpreter* interpreter) const {
    int loc = interpreter->allocPtr();
    interpreter->s[loc] = const_cast<char*>(_str.c_str());
    return loc;
}

int ExprVecNode::buildInterpreter(Interpreter* interpreter) const {
    std::vector<int> locs;
    for (int k = 0; k < numChildren(); k++) {
        const ExprNode* c = child(k);
        locs.push_back(c->buildInterpreter(interpreter));
    }
    interpreter->addOp(getTemplatizedOp<Tuple>(numChildren()));
    for (int k = 0; k < numChildren(); k++) interpreter->addOperand(locs[k]);
    int loc = interpreter->allocFP(numChildren());
    interpreter->addOperand(loc);
    interpreter->endOp();
    return loc;
}

int ExprBinaryOpNode::buildInterpreter(Interpreter* interpreter) const {
    const ExprNode* child0 = child(0), *child1 = child(1);
    int dim0 = child0->type().dim(), dim1 = child1->type().dim(), dimout = type().dim();
    int op0 = child0->buildInterpreter(interpreter);
    int op1 = child1->buildInterpreter(interpreter);
    if (dimout > 1) {
        if (dim0 != dimout) {
            interpreter->addOp(getTemplatizedOp<Promote>(dimout));
            int promoteOp0 = interpreter->allocFP(dimout);
            interpreter->addOperand(op0);
            interpreter->addOperand(promoteOp0);
            op0 = promoteOp0;
            interpreter->endOp();
        }
        if (dim1 != dimout) {
            interpreter->addOp(getTemplatizedOp<Promote>(dimout));
            int promoteOp1 = interpreter->allocFP(dimout);
            interpreter->addOperand(op1);
            interpreter->addOperand(promoteOp1);
            op1 = promoteOp1;
            interpreter->endOp();
        }
    }

    switch (_op) {
        case '+':
            interpreter->addOp(getTemplatizedOp2<'+', BinaryOp>(dimout));
            break;
        case '-':
            interpreter->addOp(getTemplatizedOp2<'-', BinaryOp>(dimout));
            break;
        case '*':
            interpreter->addOp(getTemplatizedOp2<'*', BinaryOp>(dimout));
            break;
        case '/':
            interpreter->addOp(getTemplatizedOp2<'/', BinaryOp>(dimout));
            break;
        case '^':
            interpreter->addOp(getTemplatizedOp2<'^', BinaryOp>(dimout));
            break;
        case '%':
            interpreter->addOp(getTemplatizedOp2<'%', BinaryOp>(dimout));
            break;
        default:
            assert(false);
    }
    int op2 = interpreter->allocFP(dimout);
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->addOperand(op2);
    interpreter->endOp();

    return op2;
}

int ExprUnaryOpNode::buildInterpreter(Interpreter* interpreter) const {
    const ExprNode* child0 = child(0);
    int dimout = type().dim();
    int op0 = child0->buildInterpreter(interpreter);

    switch (_op) {
        case '-':
            interpreter->addOp(getTemplatizedOp2<'-', UnaryOp>(dimout));
            break;
        case '~':
            interpreter->addOp(getTemplatizedOp2<'~', UnaryOp>(dimout));
            break;
        case '!':
            interpreter->addOp(getTemplatizedOp2<'!', UnaryOp>(dimout));
            break;
        default:
            assert(false);
    }
    int op1 = interpreter->allocFP(dimout);
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->endOp();

    return op1;
}

int ExprSubscriptNode::buildInterpreter(Interpreter* interpreter) const {
    const ExprNode* child0 = child(0), *child1 = child(1);
    int dimin = child0->type().dim();
    int op0 = child0->buildInterpreter(interpreter);
    int op1 = child1->buildInterpreter(interpreter);
    int op2 = interpreter->allocFP(1);

    interpreter->addOp(getTemplatizedOp<Subscript>(dimin));
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->addOperand(op2);
    interpreter->endOp();
    return op2;
}

int ExprVarNode::buildInterpreter(Interpreter* interpreter) const {
    if (const ExprLocalVar* var = _localVar) {
        // if (const ExprLocalVar* phi = var->getPhi()) var = phi;
        Interpreter::VarToLoc::iterator i = interpreter->varToLoc.find(var);
        if (i != interpreter->varToLoc.end())
            return i->second;
        else
            throw std::runtime_error("Unallocated variable encountered.");
    } else if (const ExprVarRef* var = _var) {
        ExprType type = var->type();
        int destLoc = -1;
        if (type.isFP()) {
            int dim = type.dim();
            destLoc = interpreter->allocFP(dim);
        } else
            destLoc = interpreter->allocPtr();
        if (const auto* blockVarRef = dynamic_cast<const VarBlockCreator::Ref*>(var)) {
            // TODO: handle strings
            if (blockVarRef->type().isLifetimeUniform())
                interpreter->addOp(getTemplatizedOp2<1, EvalVarBlockIndirect>(type.dim()));
            else
                interpreter->addOp(getTemplatizedOp2<0, EvalVarBlockIndirect>(type.dim()));
            interpreter->addOperand(blockVarRef->offset());
            interpreter->addOperand(destLoc);
            interpreter->addOperand(blockVarRef->stride());
            interpreter->endOp();
        } else {
            int varRefLoc = interpreter->allocPtr();
            interpreter->addOp(EvalVar::f);
            interpreter->s[varRefLoc] = const_cast<char*>(reinterpret_cast<const char*>(var));
            interpreter->addOperand(varRefLoc);
            interpreter->addOperand(destLoc);
            interpreter->endOp();
        }
        return destLoc;
    }
    return -1;
}

int ExprLocalVar::buildInterpreter(Interpreter* interpreter) const {
    return interpreter->varToLoc[this] =
               _type.isFP() ? interpreter->allocFP(_type.dim()) : _type.isString() ? interpreter->allocPtr() : -1;
}

int ExprAssignNode::buildInterpreter(Interpreter* interpreter) const {
    int loc = _localVar->buildInterpreter(interpreter);
    assert(loc != -1 && "Invalid type found");

    ExprType child0Type = child(0)->type();
    int op0 = child(0)->buildInterpreter(interpreter);
    if (child0Type.isFP()) {
        interpreter->addOp(getTemplatizedOp<AssignOp>(child0Type.dim()));
    } else if (child0Type.isString()) {
        interpreter->addOp(AssignStrOp::f);
    } else {
        assert(false && "Invalid desired assign type");
        return -1;
    }
    interpreter->addOperand(op0);
    interpreter->addOperand(loc);
    interpreter->endOp();
    return loc;
}

void copyVarToPromotedPosition(Interpreter* interpreter, ExprLocalVar* varSource, ExprLocalVar* varDest) {
    if (varDest->type().isFP()) {
        int destDim = varDest->type().dim();
        if (destDim != varSource->type().dim()) {
            assert(varSource->type().dim() == 1);
            interpreter->addOp(getTemplatizedOp<Promote>(destDim));
        } else {
            interpreter->addOp(getTemplatizedOp<AssignOp>(destDim));
        }
        interpreter->addOperand(interpreter->varToLoc[varSource]);
        interpreter->addOperand(interpreter->varToLoc[varDest]);
        interpreter->endOp();
    } else if (varDest->type().isString()) {
        interpreter->addOp(AssignStrOp::f);
        interpreter->addOperand(interpreter->varToLoc[varSource]);
        interpreter->addOperand(interpreter->varToLoc[varDest]);
        interpreter->endOp();
    } else {
        assert(false && "failed to promote invalid type");
    }
}

int ExprIfThenElseNode::buildInterpreter(Interpreter* interpreter) const {
    int condop = child(0)->buildInterpreter(interpreter);
    int basePC = interpreter->nextPC();

    const auto& merges = _varEnv->merge(_varEnvMergeIndex);
    // Allocate spots for all the join variables
    // they are before in the sequence of operands, but it doesn't matter
    // NOTE: at this point the variables thenVar and elseVar have not been codegen'd
    for (auto& it : merges) {
        ExprLocalVarPhi* finalVar = it.second;
        if (finalVar->valid()) {
            finalVar->buildInterpreter(interpreter);
        }
    }

    // Setup the conditional jump
    interpreter->addOp(CondJmpRelativeIfFalse::f);
    interpreter->addOperand(condop);
    int destFalse = interpreter->addOperand(0);
    interpreter->endOp();

    // Then block (build interpreter and copy variables out then jump to end)
    child(1)->buildInterpreter(interpreter);
    for (auto& it : merges) {
        ExprLocalVarPhi* finalVar = it.second;
        if (finalVar->valid()) {
            copyVarToPromotedPosition(interpreter, finalVar->_thenVar, finalVar);
        }
    }
    interpreter->addOp(JmpRelative::f);
    int destEnd = interpreter->addOperand(0);
    interpreter->endOp();

    // Else block (build interpreter, copy variables out and then we're at end)
    int child2PC = interpreter->nextPC();
    child(2)->buildInterpreter(interpreter);
    for (auto& it : merges) {
        ExprLocalVarPhi* finalVar = it.second;
        if (finalVar->valid()) {
            copyVarToPromotedPosition(interpreter, finalVar->_elseVar, finalVar);
        }
    }

    // Patch the jump addresses in the conditional
    interpreter->opData[destFalse] = child2PC - basePC;
    interpreter->opData[destEnd] = interpreter->nextPC() - (child2PC - 1);

    return -1;
}

int ExprCompareNode::buildInterpreter(Interpreter* interpreter) const {
    const ExprNode* child0 = child(0), *child1 = child(1);
    assert(type().dim() == 1 && type().isFP());

    if (_op == '&' || _op == '|') {
        // Handle short circuiting

        // allocate output
        int op2 = interpreter->allocFP(1);
        // unconditionally evaluate first argument
        int op0 = child0->buildInterpreter(interpreter);
        // conditional to check if that argument could continue
        int basePC = (interpreter->nextPC());
        interpreter->addOp(_op == '&' ? CondJmpRelativeIfFalse::f : CondJmpRelativeIfTrue::f);
        interpreter->addOperand(op0);
        int destFalse = interpreter->addOperand(0);
        interpreter->endOp();
        // this is the no-branch case (op1=true for & and op0=false for |), so eval op1
        int op1 = child1->buildInterpreter(interpreter);
        // combine with &
        interpreter->addOp(_op == '&' ? getTemplatizedOp2<'&', BinaryOp>(1) : getTemplatizedOp2<'|', BinaryOp>(1));
        interpreter->addOperand(op0);
        interpreter->addOperand(op1);
        interpreter->addOperand(op2);
        interpreter->endOp();
        interpreter->addOp(JmpRelative::f);
        int destEnd = interpreter->addOperand(0);
        interpreter->endOp();
        // this is the branch case (op1=false for & and op0=true for |) so no eval of op1 required
        // just copy from the op0's value
        int falseConditionPC = interpreter->nextPC();
        interpreter->addOp(AssignOp<1>::f);
        interpreter->addOperand(op0);
        interpreter->addOperand(op2);
        interpreter->endOp();

        // fix PC relative jump addressses
        interpreter->opData[destFalse] = falseConditionPC - basePC;
        interpreter->opData[destEnd] = interpreter->nextPC() - (falseConditionPC - 1);

        return op2;

    } else {
        // Noraml case, always have to evaluatee everything
        int op0 = child0->buildInterpreter(interpreter);
        int op1 = child1->buildInterpreter(interpreter);
        switch (_op) {
            case '<':
                interpreter->addOp(getTemplatizedOp2<'<', BinaryOp>(1));
                break;
            case '>':
                interpreter->addOp(getTemplatizedOp2<'>', BinaryOp>(1));
                break;
            case 'l':
                interpreter->addOp(getTemplatizedOp2<'l', BinaryOp>(1));
                break;
            case 'g':
                interpreter->addOp(getTemplatizedOp2<'g', BinaryOp>(1));
                break;
            case '&':
                assert(false);  // interpreter->addOp(getTemplatizedOp2<'&',BinaryOp>(1));break;
            case '|':
                assert(false);  // interpreter->addOp(getTemplatizedOp2<'|',BinaryOp>(1));break;
            default:
                assert(false);
        }
        int op2 = interpreter->allocFP(1);
        interpreter->addOperand(op0);
        interpreter->addOperand(op1);
        interpreter->addOperand(op2);
        interpreter->endOp();
        return op2;
    }
}

int ExprPrototypeNode::buildInterpreter(Interpreter* interpreter) const {
    // set up parents
    _interpreterOps.clear();
    for (int c = 0; c < numChildren(); c++) {
        if (const ExprVarNode* childVarNode = dynamic_cast<const ExprVarNode*>(child(c))) {
            ExprType childType = childVarNode->type();
            if (childType.isFP()) {
                int operand = interpreter->allocFP(childType.dim());
                _interpreterOps.push_back(operand);
                interpreter->varToLoc[childVarNode->localVar()] = operand;
            }
        } else {
            assert(false);
        }
        child(c)->buildInterpreter(interpreter);

        // make sure we have a slot in our global activation record for the variables!
    }
    return 0;
}

int ExprCompareEqNode::buildInterpreter(Interpreter* interpreter) const {
    const ExprNode* child0 = child(0), *child1 = child(1);
    int op0 = child0->buildInterpreter(interpreter);
    int op1 = child1->buildInterpreter(interpreter);

    if (child0->type().isFP()) {
        int dim0 = child0->type().dim(), dim1 = child1->type().dim();
        int dimCompare = std::max(dim0, dim1);
        if (dimCompare > 1) {
            if (dim0 == 1) {
                interpreter->addOp(getTemplatizedOp<Promote>(dim1));
                int promotedOp0 = interpreter->allocFP(dim1);
                interpreter->addOperand(op0);
                interpreter->addOperand(promotedOp0);
                interpreter->endOp();
                op0 = promotedOp0;
            }
            if (dim1 == 1) {
                interpreter->addOp(getTemplatizedOp<Promote>(dim0));
                int promotedOp1 = interpreter->allocFP(dim0);
                interpreter->addOperand(op1);
                interpreter->addOperand(promotedOp1);
                interpreter->endOp();
                op1 = promotedOp1;
            }
        }
        if (_op == '=')
            interpreter->addOp(getTemplatizedOp2<'=', CompareEqOp>(dimCompare));
        else if (_op == '!')
            interpreter->addOp(getTemplatizedOp2<'!', CompareEqOp>(dimCompare));
        else
            assert(false && "Invalid operation");
    } else if (child0->type().isString()) {
        if (_op == '=')
            interpreter->addOp(getTemplatizedOp2<'=', StrCompareEqOp>(1));
        else if (_op == '!')
            interpreter->addOp(getTemplatizedOp2<'!', StrCompareEqOp>(1));
        else
            assert(false && "Invalid operation");
    } else
        assert(false && "Invalid type for comparison");
    int op2 = interpreter->allocFP(1);
    interpreter->addOperand(op0);
    interpreter->addOperand(op1);
    interpreter->addOperand(op2);
    interpreter->endOp();
    return op2;
}

int ExprCondNode::buildInterpreter(Interpreter* interpreter) const {
    int opOut = -1;
    // TODO: handle strings!
    int dimout = type().dim();

    // conditional
    int condOp = child(0)->buildInterpreter(interpreter);
    int basePC = (interpreter->nextPC());
    interpreter->addOp(CondJmpRelativeIfFalse::f);
    interpreter->addOperand(condOp);
    int destFalse = interpreter->addOperand(0);
    interpreter->endOp();

    // true way of working
    int op1 = child(1)->buildInterpreter(interpreter);
    if (type().isFP())
        interpreter->addOp(getTemplatizedOp<AssignOp>(dimout));
    else if (type().isString())
        interpreter->addOp(AssignStrOp::f);
    else
        assert(false);
    interpreter->addOperand(op1);
    int dataOutTrue = interpreter->addOperand(-1);
    interpreter->endOp(false);

    // jump past false way of working
    interpreter->addOp(JmpRelative::f);
    int destEnd = interpreter->addOperand(0);
    interpreter->endOp();

    // record start of false condition
    int child2PC = interpreter->nextPC();

    // false way of working
    int op2 = child(2)->buildInterpreter(interpreter);
    if (type().isFP())
        interpreter->addOp(getTemplatizedOp<AssignOp>(dimout));
    else if (type().isString())
        interpreter->addOp(AssignStrOp::f);
    else
        assert(false);
    interpreter->addOperand(op2);
    int dataOutFalse = interpreter->addOperand(-1);
    interpreter->endOp(false);

    // patch up relative jumps
    interpreter->opData[destFalse] = child2PC - basePC;
    interpreter->opData[destEnd] = interpreter->nextPC() - (child2PC - 1);

    // allocate output
    if (type().isFP())
        opOut = interpreter->allocFP(type().dim());
    else if (type().isString())
        opOut = interpreter->allocPtr();
    else
        assert(false);

    // patch outputs on assigns in each condition
    interpreter->opData[dataOutTrue] = opOut;
    interpreter->opData[dataOutFalse] = opOut;

    return opOut;
}

int ExprBlockNode::buildInterpreter(Interpreter* interpreter) const {
    assert(numChildren() == 2);
    child(0)->buildInterpreter(interpreter);
    return child(1)->buildInterpreter(interpreter);
}

int ExprModuleNode::buildInterpreter(Interpreter* interpreter) const {
    int lastIdx = 0;
    for (int c = 0; c < numChildren(); c++) {
        if (c == numChildren() - 1) interpreter->setPCStart(interpreter->nextPC());
        lastIdx = child(c)->buildInterpreter(interpreter);
    }
    return lastIdx;
}
}
