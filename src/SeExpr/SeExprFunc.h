#ifndef SeExprFunc_h
#define SeExprFunc_h

#include "SeVec3d.h"
#include <vector>

#include "SeExprType.h"
#include "SeExprEnv.h"
#include "SeExprFuncX.h"
#include "SeExprFuncStandard.h"

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

    //bool isScalar() const { return _scalar; };

    SeExprFunc()
        : _func(0), _minargs(0), _maxargs(0)
    {};

    //! User defined function with custom argument parsing
    SeExprFunc(SeExprFuncX& f, int min=1, int max=1)
	: _func(&f), _minargs(min), _maxargs(max)
    {};

    SeExprFunc(SeExprFuncStandard::Func0* f)
        :_standardFunc(SeExprFuncStandard::FUNC0,(void*)f),_func(0),_minargs(0),_maxargs(0) {}
    SeExprFunc(SeExprFuncStandard::Func1* f)
        :_standardFunc(SeExprFuncStandard::FUNC1,(void*)f),_func(0),_minargs(1),_maxargs(1) {}
    SeExprFunc(SeExprFuncStandard::Func2* f)
        :_standardFunc(SeExprFuncStandard::FUNC2,(void*)f),_func(0),_minargs(2),_maxargs(2) {}
    SeExprFunc(SeExprFuncStandard::Func3* f)
        :_standardFunc(SeExprFuncStandard::FUNC3,(void*)f),_func(0),_minargs(3),_maxargs(3) {}
    SeExprFunc(SeExprFuncStandard::Func4* f)
        :_standardFunc(SeExprFuncStandard::FUNC4,(void*)f),_func(0),_minargs(4),_maxargs(4) {}
    SeExprFunc(SeExprFuncStandard::Func5* f)
        :_standardFunc(SeExprFuncStandard::FUNC5,(void*)f),_func(0),_minargs(5),_maxargs(5) {}
    SeExprFunc(SeExprFuncStandard::Func6* f)
        :_standardFunc(SeExprFuncStandard::FUNC6,(void*)f),_func(0),_minargs(6),_maxargs(6) {}
    SeExprFunc(SeExprFuncStandard::Funcn* f,int minArgs,int maxArgs)
        :_standardFunc(SeExprFuncStandard::FUNCN,(void*)f),_func(0),_minargs(minArgs),_maxargs(maxArgs) {}
    SeExprFunc(SeExprFuncStandard::Func1v* f)
        :_standardFunc(SeExprFuncStandard::FUNC1V,(void*)f),_func(0),_minargs(1),_maxargs(1) {}
    SeExprFunc(SeExprFuncStandard::Func2v* f)
        :_standardFunc(SeExprFuncStandard::FUNC2V,(void*)f),_func(0),_minargs(2),_maxargs(2) {}
    SeExprFunc(SeExprFuncStandard::Funcnv* f,int minArgs,int maxArgs)
        :_standardFunc(SeExprFuncStandard::FUNCNV,(void*)f),_func(0),_minargs(minArgs),_maxargs(maxArgs) {}
    SeExprFunc(SeExprFuncStandard::Func1vv* f)
        :_standardFunc(SeExprFuncStandard::FUNC1VV,(void*)f),_func(0),_minargs(1),_maxargs(1) {}
    SeExprFunc(SeExprFuncStandard::Func2vv* f)
        :_standardFunc(SeExprFuncStandard::FUNC2VV,(void*)f),_func(0),_minargs(2),_maxargs(2) {}
    SeExprFunc(SeExprFuncStandard::Funcnvv* f)
        :_standardFunc(SeExprFuncStandard::FUNC1VV,(void*)f),_func(0),_minargs(1),_maxargs(1) {}
    SeExprFunc(SeExprFuncStandard::Funcnvv* f,int minArgs,int maxArgs)
        :_standardFunc(SeExprFuncStandard::FUNCNVV,(void*)f),_func(0),_minargs(minArgs),_maxargs(maxArgs) {}

    //! return the minimum number of acceptable arguments
    int minArgs() const { return _minargs; }
    //! return the maximum number of acceptable arguments
    int maxArgs() const { return _maxargs; }
    //! return pointer to the funcx
    const SeExprFuncX* funcx() const { return _func ? _func : &_standardFunc; }

private:
    SeExprFuncStandard _standardFunc;
    SeExprFuncX*      _func;
    int        _minargs;
    int        _maxargs;
};

#endif
