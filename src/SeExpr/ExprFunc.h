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
#ifndef ExprFunc_h
#define ExprFunc_h

#include "Vec.h"
#include <vector>

#include "ExprType.h"
#include "ExprEnv.h"
#include "ExprFuncX.h"
#include "ExprFuncStandard.h"

namespace SeExpr2 {

//! Function Definition, used in parse tree and func table.
/** This class in a static setting manages all builtin functions defined by
    SeExpr internally or through the use of shared object plugins.  These can be queried
    by name for documentation.

    Users can create their own custom functions by creating one of these with the appropriate
    argument template. Any function that doesn't work within the given templates
    can be written using a ExprFuncX template instead

    Note: If you use the convenience prototypes instead of ExprFuncX, the
    user defined function will be assumed to be thread safe. If you have a
    thread unsafe function be sure to use ExprFuncX and call the base constructor
    with false.
*/
class ExprFunc {
    static void initInternal();  // call to define built-in funcs and load standard plugins
  public:
    //! call to define built-in funcs and load standard plugins
    /** In addition to initializing all builtins, this loads all plugins given in a
        a colon delimited SE_EXPR_PLUGINS environment variable **/
    static void init();
    //! cleanup all functions
    static void cleanup();
    //! load all plugins in a given path
    static void loadPlugins(const char* path);
    //! load a given plugin
    static void loadPlugin(const char* path);

    /* A pointer to the define func is passed to the init method of
       expression plugins.  This should be called instead of calling
       the static method directly so that the plugin will work if the
       expression library is statically linked. */
    static void define(const char* name, ExprFunc f, const char* docString);
    static void define(const char* name, ExprFunc f);
    typedef void (*Define)(const char* name, ExprFunc f);
    typedef void (*Define3)(const char* name, ExprFunc f, const char* docString);

    //! Lookup a builtin function by name
    static const ExprFunc* lookup(const std::string& name);

    //! Get a list of registered builtin and DSO generated functions
    static void getFunctionNames(std::vector<std::string>& names);

    //! Get doc string for a specific function
    static std::string getDocString(const char* functionName);

    //! Get the total size estimate of all plugins
    static size_t sizeInBytes();

    //! Dump statistics
    static Statistics statistics();

    // bool isScalar() const { return _scalar; };

    ExprFunc() : _func(0), _minargs(0), _maxargs(0) {};

    //! User defined function with custom argument parsing
    ExprFunc(ExprFuncX& f, int min = 1, int max = 1) : _func(&f), _minargs(min), _maxargs(max) {};

    ExprFunc(ExprFuncStandard::Func0* f)
        : _standardFunc(ExprFuncStandard::FUNC0, (void*)f), _func(0), _minargs(0), _maxargs(0) {}
    ExprFunc(ExprFuncStandard::Func1* f)
        : _standardFunc(ExprFuncStandard::FUNC1, (void*)f), _func(0), _minargs(1), _maxargs(1) {}
    ExprFunc(ExprFuncStandard::Func2* f)
        : _standardFunc(ExprFuncStandard::FUNC2, (void*)f), _func(0), _minargs(2), _maxargs(2) {}
    ExprFunc(ExprFuncStandard::Func3* f)
        : _standardFunc(ExprFuncStandard::FUNC3, (void*)f), _func(0), _minargs(3), _maxargs(3) {}
    ExprFunc(ExprFuncStandard::Func4* f)
        : _standardFunc(ExprFuncStandard::FUNC4, (void*)f), _func(0), _minargs(4), _maxargs(4) {}
    ExprFunc(ExprFuncStandard::Func5* f)
        : _standardFunc(ExprFuncStandard::FUNC5, (void*)f), _func(0), _minargs(5), _maxargs(5) {}
    ExprFunc(ExprFuncStandard::Func6* f)
        : _standardFunc(ExprFuncStandard::FUNC6, (void*)f), _func(0), _minargs(6), _maxargs(6) {}
    ExprFunc(ExprFuncStandard::Funcn* f, int minArgs, int maxArgs)
        : _standardFunc(ExprFuncStandard::FUNCN, (void*)f), _func(0), _minargs(minArgs), _maxargs(maxArgs) {}
    ExprFunc(ExprFuncStandard::Func1v* f)
        : _standardFunc(ExprFuncStandard::FUNC1V, (void*)f), _func(0), _minargs(1), _maxargs(1) {}
    ExprFunc(ExprFuncStandard::Func2v* f)
        : _standardFunc(ExprFuncStandard::FUNC2V, (void*)f), _func(0), _minargs(2), _maxargs(2) {}
    ExprFunc(ExprFuncStandard::Funcnv* f, int minArgs, int maxArgs)
        : _standardFunc(ExprFuncStandard::FUNCNV, (void*)f), _func(0), _minargs(minArgs), _maxargs(maxArgs) {}
    ExprFunc(ExprFuncStandard::Func1vv* f)
        : _standardFunc(ExprFuncStandard::FUNC1VV, (void*)f), _func(0), _minargs(1), _maxargs(1) {}
    ExprFunc(ExprFuncStandard::Func2vv* f)
        : _standardFunc(ExprFuncStandard::FUNC2VV, (void*)f), _func(0), _minargs(2), _maxargs(2) {}
    ExprFunc(ExprFuncStandard::Funcnvv* f)
        : _standardFunc(ExprFuncStandard::FUNC1VV, (void*)f), _func(0), _minargs(1), _maxargs(1) {}
    ExprFunc(ExprFuncStandard::Funcnvv* f, int minArgs, int maxArgs)
        : _standardFunc(ExprFuncStandard::FUNCNVV, (void*)f), _func(0), _minargs(minArgs), _maxargs(maxArgs) {}

    //! return the minimum number of acceptable arguments
    int minArgs() const { return _minargs; }
    //! return the maximum number of acceptable arguments
    int maxArgs() const { return _maxargs; }
    //! return pointer to the funcx
    const ExprFuncX* funcx() const { return _func ? _func : &_standardFunc; }

  private:
    ExprFuncStandard _standardFunc;
    ExprFuncX* _func;
    int _minargs;
    int _maxargs;
    static std::vector<void*> dynlib;
};
}

#endif
