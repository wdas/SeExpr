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
#ifndef VarBlock_h
#define VarBlock_h

#include "Expression.h"
#include "ExprType.h"
#include "Vec.h"


namespace SeExpr2 {

class ExprNode;
class ExprVarNode;
class ExprFunc;
class Expression;
class Interpreter;

class ExprVarBlockCreator;

/// A thread local evaluation context. Just allocate and fill in with data.
class ExprVarBlock{
private:
	/// Allocate an exprVarBlock
    ExprVarBlock(int size)
    {
    	posix_memalign((void**)&_data,32,size*sizeof(double));
    }
public:
	friend class ExprVarBlockCreator;

	/// Move semantics is the only allowed way to change the structure
    ExprVarBlock(ExprVarBlock&& other)
    {
    	_data=other._data;
    	other._data=nullptr;
    }

    /// Free aligned data
    ~ExprVarBlock(){
    	free(_data);_data=nullptr;
    }

    /// Don't allow copying and operator='ing'
    ExprVarBlock(const ExprVarBlock&)=delete;
    ExprVarBlock& operator=(const ExprVarBlock&)=delete;


    /// Get vector proxy object (DO NOT HOLD THESE BETWEEN registerVariable or clone calls)
    template<int d> Vec<double,d,true> FP(uint32_t variableOffset){
        assert(variableOffset+d<data.size());
        return Vec<double,d,true>(&_data[variableOffset]);
    }

    /// Get raw data (used to call evalFP() on the expression)
    double* fpData(){return _data;}
private:
	double* _data;
};

class ExprVarBlockCreator{
public:
    class Ref:public ExprVarRef{
        uint32_t _offset;
    public:
    	int offset() const {return _offset;}
        Ref(const ExprType& type,int offset):ExprVarRef(type),_offset(offset){}
        void eval(double*) override {assert(false);}
        void eval(const char**) override {assert(false);}
    };

    /// Register a variable and return a handle
    int registerVariable(const std::string& name,const ExprType type){
        if(_vars.find(name) != _vars.end()){
            throw std::runtime_error("Already registered a variable named "+name);
        }else{
        	int offset=_nextOffset;
            _nextOffset+=std::max(4,type.dim());
            _vars.insert(std::make_pair(name,Ref(type,offset)));
            return offset;
        }
    }
    
    /// Get an evaluation handle (one needed per thread)
    ExprVarBlock evaluator(){
        return ExprVarBlock(_nextOffset);
    }
    
    /// Resolve the variable using anything in the data block (call from resolveVar in Expr subclass)
    ExprVarRef* resolveVar(const std::string& name) {
        auto it=_vars.find(name);
        if(it != _vars.end()) return &it->second;
        return nullptr;
    }
private:
    int _nextOffset=0;
    std::map<std::string,Ref> _vars;
};


} // namespace

#endif