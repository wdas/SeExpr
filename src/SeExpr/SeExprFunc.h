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
#ifndef SeExprFunc_h
#define SeExprFunc_h

#include "SeVec3d.h"
#include <vector>

#include "SeExprType.h"
#include "SeExprEnv.h"

class SeExpression;
class SeExprFuncNode;

//! Extension function spec, used for complicated argument custom functions.
/** Provides the ability to handle all argument type checking and processing manually.
    Derive from this class and then make your own SeExprFunc that takes this object.
    This is necessary if you need string arguments or you have variable numbers of
    arguments.  See SeExprBuiltins.h for some examples */
class SeExprFuncX {
public:
    //! Create an SeExprFuncX. If the functions and type checking you implement
    //! is thread safe your derived class should call this with true. If not,
    //! then false.  If you mark a function as thread unsafe,  and it is used
    //! in an expression then bool SeExpression::isThreadSafe() will return false
    //! and the controlling software should not attempt to run multiple threads
    //! of an expression.
    SeExprFuncX(const bool threadSafe)
        :_threadSafe(threadSafe)
    {}

    /** prep the expression by doing all type checking argument checking, etc. */
    virtual SeExprType prep(SeExprFuncNode* node, bool scalarWanted, SeExprVarEnv & env);

    virtual bool       isScalar() const = 0;
    virtual SeExprType retType () const = 0;

    /** evaluate the expression. the given node is where in the parse tree
        the evaluation is for */
    virtual void eval(const SeExprFuncNode* node, SeVec3d& result) const = 0;
    virtual ~SeExprFuncX(){}

    bool isThreadSafe() const {return _threadSafe;}

 protected:
    bool _isScalar;

private:
    bool _threadSafe;
};

//! Function Definition, used in parse tree and func table.  
/** This class in a static setting manages all builtin functions defined by
    SeExpr internally or through the use of shared object plugins.  These can be queried
    by name for documentation.

    Users can create their own custom functions by creating one of these with the appropriate 
    argument template. Any function that doesn't work within the given templates
    can be written using a SeExprFuncX template instead
    
    Note: If you use the convenience prototypes instead of SeExprFuncX, the
    user defined function will be assumed to be thread safe. If you have a
    thread unsafe function be sure to use SeExprFuncX and call the base constructor
    with false.
*/
class SeExprFunc {
    static void initInternal(); // call to define built-in funcs and load standard plugins
public:
    //! call to define built-in funcs and load standard plugins
    /** In addition to initializing all builtins, this loads all plugins given in a
        a colon delimited SE_EXPR_PLUGINS environment variable **/
    static void init(); 
    //! load all plugins in a given path
    static void loadPlugins(const char* path);
    //! load a given plugin
    static void loadPlugin(const char* path);

    /* A pointer to the define func is passed to the init method of
       expression plugins.  This should be called instead of calling
       the static method directly so that the plugin will work if the
       expression library is statically linked. */
    static void define(const char* name, SeExprFunc f,const char* docString);
    static void define(const char* name, SeExprFunc f);
    typedef void (*Define) (const char* name, SeExprFunc f);
    typedef void (*Define3) (const char* name, SeExprFunc f,const char* docString);

    //! Lookup a builtin function by name
    static const SeExprFunc* lookup(const std::string& name);

    //! Get a list of registered builtin and DSO generated functions
    static void getFunctionNames(std::vector<std::string>& names);

    //! Get doc string for a specific function
    static std::string getDocString(const char* functionName);

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

    enum FuncType {
	NONE=0, 
	// scalar args and result
	FUNC0, FUNC1, FUNC2, FUNC3, FUNC4, FUNC5, FUNC6, FUNCN,
	// vector args, scalar result
	VEC, FUNC1V=VEC, FUNC2V, FUNCNV,
	// vector args and result
	VECVEC, FUNC1VV=VECVEC, FUNC2VV, FUNCNVV,
	// extension type
	FUNCX
    };

    bool isScalar() const { return _scalar; };

    SeExprFunc()
        : _type(NONE), _retType(SeExprType::NoneType_varying()), _scalar(false), _func(0), _minargs(0), _maxargs(0)
    {};
    //! No argument function
    SeExprFunc(Func0* f)
        : _type(FUNC0), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(0), _maxargs(0)
    {};
    //! User defined function with prototype double f(double)
    SeExprFunc(Func1* f)
        : _type(FUNC1), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype double f(double,double)
    SeExprFunc(Func2* f)
        : _type(FUNC2), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with prototype double f(double,double,double)
    SeExprFunc(Func3* f)
        : _type(FUNC3), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(3), _maxargs(3)
    {};
    //! User defined function with prototype double f(double,double,double,double)
    SeExprFunc(Func4* f)
        : _type(FUNC4), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(4), _maxargs(4)
    {};
    //! User defined function with prototype double f(double,double,double,double,double)
    SeExprFunc(Func5* f)
        : _type(FUNC5), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(5), _maxargs(5)
    {};
    //! User defined function with prototype double f(double,double,double,double,double,double)
    SeExprFunc(Func6* f)
        : _type(FUNC6), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(6), _maxargs(6)
    {};
    //! User defined function with prototype double f(vector)
    SeExprFunc(Func1v* f)
        : _type(FUNC1V), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype double f(vector,vector)
    SeExprFunc(Func2v* f)
        : _type(FUNC2V), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with prototype vector f(vector)
    SeExprFunc(Func1vv* f)
        : _type(FUNC1VV), _retType(SeExprType::FPNType_varying(3)), _scalar(false), _func((void*)f), _minargs(1), _maxargs(1)
    {};
    //! User defined function with prototype vector f(vector,vector)
    SeExprFunc(Func2vv* f)
        : _type(FUNC2VV), _retType(SeExprType::FPNType_varying(3)), _scalar(false), _func((void*)f), _minargs(2), _maxargs(2)
    {};
    //! User defined function with arbitrary number of arguments double f(double,...)
    SeExprFunc(Funcn* f, int min, int max)
	: _type(FUNCN), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(min), _maxargs(max)
    {};
    //! User defined function with arbitrary number of arguments double f(vector,...)
    SeExprFunc(Funcnv* f, int min, int max)
	: _type(FUNCNV), _retType(SeExprType::FP1Type_varying()), _scalar(true), _func((void*)f), _minargs(min), _maxargs(max)
    {};
    //! User defined function with arbitrary number of arguments vector f(vector,...)
    SeExprFunc(Funcnvv* f, int min, int max)
	: _type(FUNCNVV), _retType(SeExprType::FPNType_varying(3)), _scalar(false), _func((void*)f), _minargs(min), _maxargs(max)
    {};
    //! User defined function with custom argument parsing
    SeExprFunc(SeExprFuncX& f, int min=1, int max=1)
	: _type(FUNCX), _retType(f.retType()), _scalar(f.isScalar()), _func((void*)&f), _minargs(min), _maxargs(max)
    {};

    inline SeExprType retType() const { return _retType; };
    int type() const { return _type; }
    int minArgs() const { return _minargs; }
    int maxArgs() const { return _maxargs; }
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
    SeExprFuncX* funcx() const { return (SeExprFuncX*)_func; }

private:
    FuncType   _type;
    SeExprType _retType;
    bool       _scalar;
    void*      _func;
    int        _minargs;
    int        _maxargs;
};

#endif
