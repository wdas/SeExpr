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
#ifndef _ExprFuncStandard_h_
#define _ExprFuncStandard_h_

#include "Vec.h"
#include "ExprFuncX.h"

namespace SeExpr2 {

class ExprFuncStandard : public ExprFuncX {
  public:
    enum FuncType {
        NONE = 0,
        // scalar args and result
        FUNC0,
        FUNC1,
        FUNC2,
        FUNC3,
        FUNC4,
        FUNC5,
        FUNC6,
        FUNCN,
        // vector args, scalar result
        VEC,
        FUNC1V = VEC,
        FUNC2V,
        FUNCNV,
        // vector args and result
        VECVEC,
        FUNC1VV = VECVEC,
        FUNC2VV,
        FUNCNVV
    };

    typedef double Func0();
    typedef double Func1(double);
    typedef double Func2(double, double);
    typedef double Func3(double, double, double);
    typedef double Func4(double, double, double, double);
    typedef double Func5(double, double, double, double, double);
    typedef double Func6(double, double, double, double, double, double);
    typedef double Func1v(const Vec3d&);
    typedef double Func2v(const Vec3d&, const Vec3d&);
    typedef Vec3d Func1vv(const Vec3d&);
    typedef Vec3d Func2vv(const Vec3d&, const Vec3d&);
    typedef double Funcn(int n, double* params);
    typedef double Funcnv(int n, const Vec3d* params);
    typedef Vec3d Funcnvv(int n, const Vec3d* params);

#if 0
    Func0* func0() const { return (Func0*)_func; }
    Func1* func1() const { return (Func1*)_func; }
    Func2* func2() const { return (Func2*)_func; }
    Func3* func3() const { return (Func3*)_func; }
    Func4* func4() const { return (Func4*)_func; }
    Func5* func5() const { return (Func5*)_func; }
    Func6* func6() const { return (Func6*)_func; }
    Func1v* func1v() const { return (Func1v*)_func; }
    Func2v* func2v() const { return (Func2v*)_func; }
    Func1vv* func1vv() const { return (Func1vv*)_func; }
    Func2vv* func2vv() const { return (Func2vv*)_func; }
    Funcn* funcn() const { return (Funcn*)_func; }
    Funcnv* funcnv() const { return (Funcnv*)_func; }
    Funcnvv* funcnvv() const { return (Funcnvv*)_func; }
#endif

    //! No argument function
    ExprFuncStandard(FuncType funcType, void* f) : ExprFuncX(true), _funcType(funcType), _func(f) {}
#if 0
    //! User defined function with prototype double f(double)
    ExprFunc(Func1* f)
        : _type(FUNC1), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype double f(double,double)
    ExprFunc(Func2* f)
        : _type(FUNC2), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with prototype double f(double,double,double)
    ExprFunc(Func3* f)
        : _type(FUNC3), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(3), _maxargs(3)
    {};
    //! User defined function with prototype double f(double,double,double,double)
    ExprFunc(Func4* f)
        : _type(FUNC4), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(4), _maxargs(4)
    {};
    //! User defined function with prototype double f(double,double,double,double,double)
    ExprFunc(Func5* f)
        : _type(FUNC5), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(5), _maxargs(5)
    {};
    //! User defined function with prototype double f(double,double,double,double,double,double)
    ExprFunc(Func6* f)
        : _type(FUNC6), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(6), _maxargs(6)
    {};
    //! User defined function with prototype double f(vector)
    ExprFunc(Func1v* f)
        : _type(FUNC1V), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype double f(vector,vector)
    ExprFunc(Func2v* f)
        : _type(FUNC2V), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with prototype vector f(vector)
    ExprFunc(Func1vv* f)
        : _type(FUNC1VV), _retType(ExprType().FP(3).Varying()), _scalar(false), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype vector f(vector,vector)
    ExprFunc(Func2vv* f)
        : _type(FUNC2VV), _retType(ExprType().FP(3).Varying()), _scalar(false), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with arbitrary number of arguments double f(double,...)
    ExprFunc(Funcn* f, int min, int max)
	: _type(FUNCN), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(min), _maxargs(max)
    {};
    //! User defined function with arbitrary number of arguments double f(vector,...)
    ExprFunc(Funcnv* f, int min, int max)
	: _type(FUNCNV), _retType(ExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(min), _maxargs(max)
    {};
    //! User defined function with arbitrary number of arguments vector f(vector,...)
    ExprFunc(Funcnvv* f, int min, int max)
	: _type(FUNCNVV), _retType(ExprType().FP(3).Varying()), _scalar(false), _func((void*)f), _minargs(min), _maxargs(max)
    {};
#endif

  public:
    ExprFuncStandard() : ExprFuncX(true) {}

    virtual ExprType prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnvBuilder& envBuilder) const;
    virtual int buildInterpreter(const ExprFuncNode* node, Interpreter* interpreter) const;
    void* getFuncPointer() const { return _func; }
    FuncType getFuncType() const { return _funcType; }

  private:
    FuncType _funcType;
    void* _func;  // blind func style
};
}

#endif
