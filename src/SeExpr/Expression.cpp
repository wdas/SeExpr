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

#include "Evaluator.h"

namespace SeExpr2 {

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
}

Expression::~Expression()
{
    reset();
    delete _llvmEvaluator;
}

void Expression::debugPrintInterpreter() const {
    if(_interpreter){
        _interpreter->print();
        std::cerr<<"return slot "<<_returnSlot<<std::endl;
    }
}

void Expression::debugPrintLLVM() const {
    _llvmEvaluator->debugPrint();
}

void Expression::debugPrintParseTree() const {
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
     delete _llvmEvaluator;_llvmEvaluator=new LLVMEvaluator();
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
            debugPrintParseTree();
#           endif
#           ifdef SEEXPR_PERFORMANCE
            PrintTiming timer("v2 interpreter build time: ");
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
        } else { // useLLVM
#           ifdef SEEXPR_DEBUG
            debugPrintParseTree();
#           endif
#           ifdef SEEXPR_PERFORMANCE
            PrintTiming timer("v2 llvm codegen time: ");
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

#           ifdef SEEXPR_PERFORMANCE
            PrintTiming timer("v2 interpreter eval time: ");
#           endif
            _interpreter->eval();
            return &_interpreter->d[_returnSlot];

        } else { // useLLVM

#           ifdef SEEXPR_PERFORMANCE
            PrintTiming timer("v2 llvm eval time: ");
#           endif
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

#           ifdef SEEXPR_PERFORMANCE
            PrintTiming timer("v2 interpreter eval time: ");
#           endif
            _interpreter->eval();
            return _interpreter->s[_returnSlot];

        } else { // useLLVM

#           ifdef SEEXPR_PERFORMANCE
            PrintTiming timer("v2 llvm eval time: ");
#           endif
            _llvmEvaluator->evalStr();
        }
    }

    return 0;
}

} // end namespace SeExpr2/
