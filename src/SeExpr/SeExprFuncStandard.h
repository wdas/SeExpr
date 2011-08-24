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
#ifndef _SeExprFuncStandard_h_
#define _SeExprFuncStandard_h_

#include <SeVec3d.h>
#include "SeExprFuncX.h"
class SeExprFuncStandard:public SeExprFuncX
{
public:
    enum FuncType {
	NONE=0, 
	// scalar args and result
	FUNC0, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5, FUNC6, FUNCN,
	// vector args, scalar result
	VEC, FUNC1V=VEC, FUNC2V, FUNCNV,
	// vector args and result
	VECVEC, FUNC1VV=VECVEC, FUNC2VV, FUNCNVV,
    };

    typedef double Func0();
    typedef double Func1(double);
    typedef double Func2(double, double);
    typedef double Func3(double, double, double);
    typedef double Func4(double, double, double, double);
    typedef double Func5(double, double, double, double, double);
    typedef double Func6(double, double, double, double, double, double);
    typedef double Func1v(const SeVec3d&);
    typedef double Func2v(const SeVec3d&, const SeVec3d&);
    typedef SeVec3d Func1vv(const SeVec3d&);
    typedef SeVec3d Func2vv(const SeVec3d&, const SeVec3d&);
    typedef double Funcn(int n, double* params);
    typedef double Funcnv(int n, const SeVec3d* params);
    typedef SeVec3d Funcnvv(int n, const SeVec3d* params);


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
    SeExprFuncStandard(FuncType funcType,void* f)
        : SeExprFuncX(true), _funcType(funcType), _func(f)
    {}
#if 0
    //! User defined function with prototype double f(double)
    SeExprFunc(Func1* f)
        : _type(FUNC1), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype double f(double,double)
    SeExprFunc(Func2* f)
        : _type(FUNC2), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with prototype double f(double,double,double)
    SeExprFunc(Func3* f)
        : _type(FUNC3), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(3), _maxargs(3)
    {};
    //! User defined function with prototype double f(double,double,double,double)
    SeExprFunc(Func4* f)
        : _type(FUNC4), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(4), _maxargs(4)
    {};
    //! User defined function with prototype double f(double,double,double,double,double)
    SeExprFunc(Func5* f)
        : _type(FUNC5), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(5), _maxargs(5)
    {};
    //! User defined function with prototype double f(double,double,double,double,double,double)
    SeExprFunc(Func6* f)
        : _type(FUNC6), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(6), _maxargs(6)
    {};
    //! User defined function with prototype double f(vector)
    SeExprFunc(Func1v* f)
        : _type(FUNC1V), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype double f(vector,vector)
    SeExprFunc(Func2v* f)
        : _type(FUNC2V), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with prototype vector f(vector)
    SeExprFunc(Func1vv* f)
        : _type(FUNC1VV), _retType(SeExprType().FP(3).Varying()), _scalar(false), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype vector f(vector,vector)
    SeExprFunc(Func2vv* f)
        : _type(FUNC2VV), _retType(SeExprType().FP(3).Varying()), _scalar(false), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with arbitrary number of arguments double f(double,...)
    SeExprFunc(Funcn* f, int min, int max)
	: _type(FUNCN), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(min), _maxargs(max)
    {};
    //! User defined function with arbitrary number of arguments double f(vector,...)
    SeExprFunc(Funcnv* f, int min, int max)
	: _type(FUNCNV), _retType(SeExprType().FP(1).Varying()), _scalar(true), _func((void*)f), _minargs(min), _maxargs(max)
    {};
    //! User defined function with arbitrary number of arguments vector f(vector,...)
    SeExprFunc(Funcnvv* f, int min, int max)
	: _type(FUNCNVV), _retType(SeExprType().FP(3).Varying()), _scalar(false), _func((void*)f), _minargs(min), _maxargs(max)
    {};
#endif

public:
    SeExprFuncStandard()
        :SeExprFuncX(true)
    {}


    virtual SeExprType prep(SeExprFuncNode* node, bool scalarWanted, SeExprVarEnv & env) const;
    virtual int buildInterpreter(const SeExprFuncNode* node,SeInterpreter* interpreter) const;


private:
    FuncType   _funcType;
    void* _func; // blind func style


};

#endif
