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
#ifndef MAKEDEPEND
#include <iostream>
#include <math.h>
#include <stack>
#include <algorithm>
#include <sstream>
#endif

#include "ExprNode.h"
#include "ExprParser.h"
#include "ExprFunc.h"
#include "Expression.h"
#include "ExprType.h"
#include "ExprEnv.h"
#include "Platform.h"

#include <cstdio>
#include <typeinfo>
#include <ExprWalker.h>

#ifdef SEEXPR_ENABLE_LLVM
#include <llvm/ADT/ArrayRef.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/PassManager.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/NoFolder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Utils/Cloning.h>
using namespace llvm;
#endif


namespace SeExpr2 {

#ifdef SEEXPR_ENABLE_LLVM
Value* promoteToDim(Value *val, unsigned dim, IRBuilder<> &Builder);

class LLVMEvaluator{
    // TODO: this seems needlessly complex, let's fix it
    // TODO: let the dev code allocate memory?
    // FP is the native function for this expression.
    template<class T>
    class LLVMEvaluationContext{
    private:
        typedef void (*FunctionPtr)(T*);
        FunctionPtr functionPtr;
        T* resultData;
    public:
        LLVMEvaluationContext(const LLVMEvaluationContext&)=delete;
        LLVMEvaluationContext& operator=(const LLVMEvaluationContext&)=delete;

        LLVMEvaluationContext()
        :functionPtr(nullptr),resultData(nullptr)
        {}
        void init(void* fp,int dim){
            reset();
            functionPtr=reinterpret_cast<FunctionPtr>(fp);
            resultData=new T[dim];
        }
        void reset(){
            if(resultData) delete[] resultData;
            functionPtr=0;
            resultData=0;
        }
        const T* operator()()
        {
            assert(functionPtr && resultData);
            functionPtr(resultData);
            return resultData;
        }
    };
    std::unique_ptr<LLVMEvaluationContext<double>> _llvmEvalFP;
    std::unique_ptr<LLVMEvaluationContext<char*>> _llvmEvalStr;



    std::unique_ptr<llvm::LLVMContext> _llvmContext;
    std::unique_ptr<llvm::ExecutionEngine> TheExecutionEngine;
public:

    LLVMEvaluator()
    {}

    void reset(){
        _llvmEvalFP.reset(nullptr);
        _llvmEvalStr.reset(nullptr);
    }

    const char* evalStr(){
        return *(*_llvmEvalStr)();
    }

    const double* evalFP(){
        return (*_llvmEvalFP)();
    }

    void debugPrint(){
        //TheModule->dump(); TODO!
    }

    void prepLLVM(ExprNode* parseTree,ExprType desiredReturnType)  {
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();

        std::string uniqueName = getUniqueName();

        // create Module
        _llvmContext.reset(new LLVMContext());
        Module* TheModule = new Module(uniqueName+"_module", *_llvmContext);

        // Create the JIT.  This takes ownership of the module.
        std::string ErrStr;
        TheExecutionEngine.reset(
            EngineBuilder(TheModule).setErrorStr(&ErrStr)
            .setUseMCJIT(true)
            .setOptLevel(CodeGenOpt::Default)
            .create());

        if (!TheExecutionEngine) {
            fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
            exit(1);
        }

        // create function and entry BB
        bool desireFP = desiredReturnType.isFP();
        Type *ParamTys[1] = {desireFP?Type::getDoublePtrTy(*_llvmContext):
                    PointerType::getUnqual(Type::getInt8PtrTy(*_llvmContext))};
        FunctionType *FT = FunctionType::get(Type::getVoidTy(*_llvmContext), ParamTys, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, uniqueName+"_func", TheModule);
        BasicBlock *BB = BasicBlock::Create(*_llvmContext, "entry", F);
        IRBuilder<> Builder(BB);

        // codegen
        Value *lastVal = parseTree->codegen(Builder);

        // return values through parameter.
        Value *firstArg = &*F->arg_begin();
        unsigned dim = (unsigned)desiredReturnType.dim();
        if(desireFP) {
            if(dim > 1) {
                Value *newLastVal = promoteToDim(lastVal, dim, Builder);
                assert(newLastVal->getType()->getVectorNumElements() == dim);
                for(unsigned i = 0; i < dim; ++i) {
                    Value *idx = ConstantInt::get(Type::getInt32Ty(*_llvmContext), i);
                    Value *val = Builder.CreateExtractElement(newLastVal, idx);
                    Value *ptr = Builder.CreateInBoundsGEP(firstArg, idx);
                    Builder.CreateStore(val, ptr);
                }
            } else if(dim == 1) {
                Value *ptr = Builder.CreateConstInBoundsGEP1_32(firstArg, 0);
                Builder.CreateStore(lastVal, ptr);
            } else {assert(false && "error. dim of FP is less than 1.");}
        } else {
            Builder.CreateStore(lastVal, firstArg);
        }

        Builder.CreateRetVoid();

        verifyModule(*TheModule);
        //TheModule->dump();

        TheExecutionEngine->finalizeObject();
        void* fp=TheExecutionEngine->getPointerToFunction(F);
        if(desireFP){
            _llvmEvalFP.reset(new LLVMEvaluationContext<double>);
            _llvmEvalFP->init(fp,dim); 
        }else{
            _llvmEvalStr.reset(new LLVMEvaluationContext<char*>);
            _llvmEvalStr->init(fp,dim);   
        }
    }

    std::string getUniqueName() const {
        std::ostringstream o;
        o << std::setbase(16) << (uint64_t)(this);
        return ("_" + o.str());
    }
};

#else
class LLVMEvaluator{
public:
    const char* evalStr(){assert("LLVM is not enabled in build" && false);}
    const double* evalFP(){assert("LLVM is not enabled in build" && false);}
    void prepLLVM(ExprNode* parseTree,ExprType desiredReturnType)  {
        assert("LLVM is not enabled in build" && false);
    }
    void debugPrint(){}
};
#endif

#ifdef SEEXPR_DEBUG
static const bool debugMode=true;
#else
static const bool debugMode=false;
#endif

class TypePrintExaminer : public SeExpr2::Examiner<true> {
public:
    virtual bool examine(const SeExpr2::ExprNode* examinee);
    virtual void reset  ()                           {};
};

bool
TypePrintExaminer::examine(const ExprNode* examinee)
{
    const ExprNode* curr=examinee;
    int depth=0;
    char buf[1024];
    while(curr != 0) {depth++;curr=curr->parent();}
    sprintf(buf,"%*s",depth*2," ");
    std::cout <<buf<<"'"<<examinee->toString()<<"' "<<typeid(*examinee).name()
              <<" type=" << examinee->type().toString() << std::endl;

    return true;
};

#ifdef SEEXPR_ENABLE_LLVM


// TODO: add proper attributes for functions
// TODO: figure out where to store result
// codegen'd function use heap to store return value,
// pass pointer to memory back to caller.
// no need to allocate memory in user program to call this.
#endif

#ifdef SEEXPR_ENABLE_LLVM
    EvaluationStrategy defaultEvaluationStrategy = UseLLVM;
#else
    EvaluationStrategy defaultEvaluationStrategy = UseInterpreter;
#endif

Expression::Expression(EvaluationStrategy evaluationStrategy)
    : _wantVec(true), _expression(""), _evaluationStrategy(evaluationStrategy), _context(&Context::global()), _desiredReturnType(ExprType().FP(3).Varying()), _varEnv(0), _parseTree(0), _isValid(0), _parsed(0), _prepped(0), _interpreter(0),
    _llvmEvaluator(new LLVMEvaluator())
{
    ExprFunc::init();
}

Expression::Expression( const std::string &e, const ExprType & type, EvaluationStrategy evaluationStrategy, const Context& context)
    : _wantVec(true), _expression(e), _evaluationStrategy(evaluationStrategy), _context(&context), _desiredReturnType(type), _varEnv(0),  _parseTree(0), _isValid(0), _parsed(0), _prepped(0), _interpreter(0),
    _llvmEvaluator(new LLVMEvaluator())
{
    ExprFunc::init();
#ifdef SEEXPR_ENABLE_LLVM
    std::cerr << "default is LLVM\n";
#endif
}

Expression::~Expression()
{
    reset();
    delete _llvmEvaluator;
}

void Expression::debugPrintInterpreter(){
    if(_interpreter){
        _interpreter->print();
        std::cerr<<"return slot "<<_returnSlot<<std::endl;
    }
}

void Expression::debugPrintLLVM(){
    _llvmEvaluator->debugPrint();
}

void Expression::debugPrintParseTree(){
    if(_parseTree){
        // print the parse tree
        std::cerr<<"Parse tree desired type "<<_desiredReturnType.toString()<<" actual "<<_parseTree->type().toString()<<std::endl;
        TypePrintExaminer _examiner;
        SeExpr2::ConstWalker  _walker(&_examiner);
        _walker.walk(_parseTree);
    }
}

void Expression::reset()
{
     delete _parseTree;_parseTree=0;
     delete _varEnv;_varEnv=0;
     if(_evaluationStrategy == UseInterpreter) {
         delete _interpreter;
         _interpreter=0;
     }
    _isValid = 0;
    _parsed = 0;
    _prepped = 0;
    _parseError = "";
    _vars.clear();
    _funcs.clear();
    //_localVars.clear();
    _errors.clear();
    _threadUnsafeFunctionCalls.clear();
    _comments.clear();

#ifdef SEEXPR_ENABLE_LLVM
    _llvmEvaluator->reset();
#endif
}

void Expression::setContext(const Context& context)
{
    reset();
    _context = &context;
}

void Expression::setDesiredReturnType(const ExprType & type)
{
    reset();
    _desiredReturnType=type;
}

void Expression::setExpr(const std::string& e)
{
    if(_expression != "")
        reset();
    _expression = e;
}

bool Expression::syntaxOK() const
{
    parseIfNeeded();
    return _isValid;
}

bool Expression::isConstant() const
{
    parseIfNeeded();
    return returnType().isLifetimeConstant();
}

bool Expression::usesVar(const std::string& name) const
{
    parseIfNeeded();
    return _vars.find(name) != _vars.end();
}

bool Expression::usesFunc(const std::string& name) const
{
    parseIfNeeded();
    return _funcs.find(name) != _funcs.end();
}

void
Expression::parse() const
{
    if (_parsed) return;
    _parsed = true;
    int tempStartPos,tempEndPos;
    ExprParse(_parseTree,
        _parseError, tempStartPos, tempEndPos, 
        _comments, this, _expression.c_str(), _wantVec);
    if(!_parseTree){
        addError(_parseError,tempStartPos,tempEndPos);
    }
}

void Expression::prep() const {
    if (_prepped) return;
    _prepped = true;
    parseIfNeeded();
    _varEnv=new ExprVarEnv;

    bool error=false;

    if(!_parseTree){
        // parse error
        error=true;
    }else if (!_parseTree->prep(_desiredReturnType.isFP(1), *_varEnv).isValid()) {
        // prep error
        error=true;
    }else if(!ExprType::valuesCompatible(_parseTree->type(),_desiredReturnType)){
        // incompatible type error
        error=true;
        _parseTree->addError("Expression generated type "
            +_parseTree->type().toString()+" incompatible with desired type "
            +_desiredReturnType.toString());
    }else{
        _isValid=true;

        if(_evaluationStrategy == UseInterpreter) {
#           ifdef SEEXPR_DEBUG
            PrintTiming timer("interpreter build time: ");
#           endif
            _interpreter=new Interpreter;
            _returnSlot=_parseTree->buildInterpreter(_interpreter);
            if(_desiredReturnType.isFP()){
                int dimWanted=_desiredReturnType.dim();
                int dimHave=_parseTree->type().dim();
                if(dimWanted>dimHave){
                    _interpreter->addOp(getTemplatizedOp<Promote>(dimWanted));
                    int finalOp=_interpreter->allocFP(dimWanted);
                    _interpreter->addOperand(_returnSlot);
                    _interpreter->addOperand(finalOp);
                    _returnSlot=finalOp;
                    _interpreter->endOp();
                }
            }
        } else {
#           ifdef SEEXPR_DEBUG
            PrintTiming timer("llvm codegen time: ");
#           endif
            _llvmEvaluator->prepLLVM(_parseTree,_desiredReturnType);
        }

        // TODO: need promote
        _returnType=_parseTree->type();
    }

    if(error){
        _isValid=false;
        _returnType=ExprType().Error();

        // build line lookup table
        std::vector<int> lines;
        const char* start=_expression.c_str();
        const char* p=_expression.c_str();
        while(*p!=0){
            if(*p=='\n') lines.push_back(p-start);
            p++;
        }
        lines.push_back(p-start);

        std::stringstream sstream;
        for(unsigned int i=0;i<_errors.size();i++){
            int* bound=std::lower_bound(&*lines.begin(),&*lines.end(),_errors[i].startPos);
            int line=bound-&*lines.begin()+1;
            //int column=_errors[i].startPos-lines[line-1];
            sstream<<"  Line "<<line<<": "<<_errors[i].error<<std::endl;
        }
        _parseError=std::string(sstream.str());
    }

    if(debugMode) std::cerr<<"ending with isValid "<<_isValid<<std::endl;
}


bool
Expression::isVec() const
{
    prepIfNeeded();
    return _isValid ? _parseTree->isVec() : _wantVec;
}

const ExprType &
Expression::returnType() const
{
    prepIfNeeded();
    return _returnType;
}

const double* Expression::evalFP() const
{
    prepIfNeeded();

    if (_isValid) {
        if(_evaluationStrategy == UseInterpreter) {
            _interpreter->eval();
            return &_interpreter->d[_returnSlot];
        } else {
            return _llvmEvaluator->evalFP();
        }
    }

    return SeExpr2::Vec3d(0,0,0);
}

const char* Expression::evalStr() const
{
    prepIfNeeded();

    if (_isValid) {
        if(_evaluationStrategy == UseInterpreter) {
            _interpreter->eval();
            return _interpreter->s[_returnSlot];
        } else {
            _llvmEvaluator->evalStr();
        }
    }

    return 0;
}

} // end namespace SeExpr2/
