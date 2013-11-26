/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/
#include <RslPlugin.h>
#include <rx.h>
#include <map>
#include <list>
#include <pthread.h>
#include <SeVec3d.h>
#include <SeExpression.h>
#include <SeExprFunc.h>
#include <RixInterfaces.h>
#include <cstdlib>
#include <cstring>
#include "immutable_hash_map.h"

namespace{
    RixTokenStorage* tokenizer;
    inline const char* tokenize(const char* str)
    {
	const char* result = tokenizer->GetToken(str);
	return result;
    }

    int SeTokenize(RslContext* /*ctx*/, int /*argc*/, const RslArg* argv[])
    {
        RslStringIter result(argv[0]);
        const char* str = *RslStringIter(argv[1]);
        *result = (RtString) tokenize(str);
        return 0;
    }

    struct SeRmanVarMap {
        immutable_hash_map<const char*, int> indexMap;
	std::vector<int> groupStarts;
	std::vector<int> groupCounts;
	std::vector<int> groupIndices;

	SeRmanVarMap(int nvars, const char** varnames, int* varindices, int* groupstarts)
	    : indexMap(varnames, varindices, nvars),
	      groupStarts(groupstarts, groupstarts + nvars),
	      groupCounts(nvars),
	      groupIndices(nvars)
	{
	    int groupIndex = 0, i, j;
	    for (i = 0; i < nvars; ) {
		// determine size of current group
		for (j = i+1; j < nvars; j++) {
		    if (groupStarts[j] != groupStarts[i]) break;
		}
		int groupCount = j - i;
		// assign group size and index to group members
		for (; i < j; i++) {
		    groupCounts[i] = groupCount;
		    groupIndices[i] = groupIndex;
		}
		groupIndex++;
	    }

            for(int i=0;i<indexMap._keys.size();i++){
                const char* name=indexMap._keys[i]?indexMap._keys[i]:"";
            }
	}
    };
    typedef std::map<const char*, SeRmanVarMap*> SeRmanVarMapMap;
    typedef std::map<const char*, int> SeRmanExprMap;
    typedef std::vector<int> SeVarBinding;

    class SeRmanExpr;


    //! Store per thread caches of expressions and the variable bindings
    struct ThreadData {

        // rix message interface
        RixMessages *msgs;
        
	// all variable maps accessed by this thread
	SeRmanVarMapMap varmaps;

	SeRmanVarMap& getVarMap(const char* varMapHandle)
	{
	    SeRmanVarMap*& varmap = varmaps[varMapHandle];
	    if (!varmap) {
		// parse var list and make a new varmap
		char* varlist = strdup(varMapHandle);
		std::vector<const char*> varnames;
		std::vector<int> groupStarts;

		// parse each var group (separated by spaces)
		char* varlist_end = 0;
		char* vargroup = strtok_r(varlist, " ", &varlist_end);
		do {
		    // parse vars within var group (separated by commas)
		    int groupStart = varnames.size();
		    char* vargroup_end = 0;
		    char* var = strtok_r(vargroup, ",", &vargroup_end);
		    do {
			varnames.push_back(tokenize(var));
			groupStarts.push_back(groupStart);
		    } while (var = strtok_r(0, ",", &vargroup_end));
		} while (vargroup = strtok_r(0, " ", &varlist_end));

		// build new varmap
		int nvars = varnames.size();
		int* varindices = (int*) alloca(sizeof(int)*nvars);
		for (int i = 0; i < nvars; i++){
                    varindices[i] = i;
                }
		varmap = new SeRmanVarMap(nvars, &varnames[0], &varindices[0], &groupStarts[0]);
		free(varlist);
	    }
	    return *varmap;
	}

        void ptError (char* fmt, ...) {
            static char strbuf[1024];
            va_list ap;
            va_start(ap, fmt);
            vsprintf(strbuf, fmt, ap); // TODO: this should use vsnprintf
            msgs->Error("%s", strbuf);
            va_end(ap);
        }
        
        void ptWarn (char* fmt, ...) {
            static char strbuf[1024];
            va_list ap;
            va_start(ap, fmt);
            vsprintf(strbuf, fmt, ap); // TODO: this should use vsnprintf
            msgs->Warning("%s", strbuf);
            va_end(ap);
        }

	SeRmanExprMap exprmap;
	std::vector<SeRmanExpr*> exprs;

	RtColor* varValues; // value of every var at current grid point
	float* Ci;	    // current value of Ci

	ThreadData()
	{
	    msgs = (RixMessages*)
		RxGetRixContext()->GetRixInterface(k_RixMessages);
	    exprs.push_back(0); // dummy entry; index 0 means uninitialized
	}
    };

    //! Gets the per thread specific data from the rsl evaluation context
    ThreadData& getThreadData(RslContext* ctx)
    {
        ThreadData* td = (ThreadData*) ctx->GetThreadData();
        if (!td) {
            td = new ThreadData;
            ctx->SetThreadData(td);
        }
        return *td;
    }

    //! A variable holding a grid's worth of values
    class SeRmanVar : public SeExprVarRef
    {
     public:
	SeRmanVar(ThreadData& td) : td(td), index(0) {}
	virtual bool isVec() { return 1; } // treat all vars as vectors
	void setIndex(int i) { index = i; }
	virtual void eval(const SeExprVarNode* node, SeVec3d& result)
	{
	    RtColor& c = td.varValues[index];
            result = c;
	}
     private:
	ThreadData& td;
	int index;
    };

    //! Accesses a given RiAttrubte, automatically. I.e. to do RiAttribute("user","foo",...) you would just access $user::foo
    class AttrVar : public SeExprVectorVarRef
    {
        std::string name;
        SeVec3d value;
     public:

	AttrVar() 
            :name(""),value(0.)
        {}

	AttrVar(const std::string& nameIn) 
            :value(0.)
        {
            //change "::" to ":" (needed :: for parsing SE).
            size_t pos=nameIn.find("::");
            name=nameIn.substr(0, pos)+nameIn.substr(pos+1,nameIn.size()-(pos-1));
        }

        std::string getName(){return name;}

        void doLookup()
        {
            // make sure have enough space to hold result
            float fbuf16[16];

            RxInfoType_t rxType; // = RxInfoColor;
            int count;            
            int statusOpt, statusAttr;

            //try option first then attribute
            statusOpt = RxOption (name.c_str(), fbuf16, sizeof (fbuf16), 
                               &rxType, &count);
            statusAttr = RxAttribute (name.c_str(), fbuf16, sizeof (fbuf16), 
                                      &rxType, &count);

            if (statusAttr != 0 && statusOpt != 0) {
                // no matches, go with default of 0
                value.setValue(0.0, 0.0, 0.0);
                return;
            }

            // found something
            switch (rxType) 
            {
            case RxInfoFloat:
                // promote float to color grey scale
                value.setValue (fbuf16[0], fbuf16[0], fbuf16[0]);
                break;
            case RxInfoColor:
            case RxInfoNormal:
            case RxInfoVector:
            case RxInfoPoint:
                // any of the tuples will do for color
                value.setValue (fbuf16[0], fbuf16[1], fbuf16[2]);
                break;
            default:
                // not an expected match
                //stderr << "SeRmanExpr: Unexpected type for Option/Attribute" << name.c_str() << rxType <<".  Only Float or Color allowed.";
                break;
            }
        }

        // Implement the interface of SeExprVarRef
	virtual void eval(const SeExprVarNode* node, SeVec3d& result)
	{result = value;}

    };


    //! The expression parsing/evaluator class. Derives from standard SeExpr
    class SeRmanExpr : public SeExpression {
     public:
	SeRmanExpr(const std::string &expr, ThreadData& td)
	    : SeExpression(expr), _td(td),  _boundVarMap(-1) {}

	virtual SeExprVarRef* resolveVar(const std::string& name) const
	{
            if (name.find("::") != std::string::npos) {
                int i, size;
                for (i = 0, size = _attrrefs.size(); i < size; i++)
                    //AttrVar attr =  _attrrefs[i];
                    if (name == _attrrefs[i]->getName())
                        return _attrrefs[i];

                // didn't match so make new
                AttrVar* attrVar = new AttrVar(/*td,*/ name);
                _attrrefs.push_back(attrVar);
                return attrVar;
            }

	    const char* token = tokenize(name.c_str());
	    for (int i = 0, size = _varrefs.size(); i < size; i++)
		if (_varnames[i] == token) return _varrefs[i];
	    SeRmanVar* var = new SeRmanVar(_td);
	    _varnames.push_back(token);
	    _varrefs.push_back(var);
	    return var;
	}

	SeVarBinding* bindVars(const char* varMapHandle)
	{
	    SeVarBinding*& binding = _bindings[varMapHandle];
	    if (!binding) {
		binding = new SeVarBinding;

		// find varmap
		SeRmanVarMap& varmap = _td.getVarMap(varMapHandle);
		// bind varmap to expression
		int nvars = _varnames.size();
		binding->resize(nvars);
		for (int i = 0; i < nvars; i++) {
		    const char* name = _varnames[i];
		    int index = varmap.indexMap[name];
		    if (!index) {
                        //for(int i=0;i<varmap.indexMap._keys.size();i++){
                        //    std::cerr<<"key "<<i<<std::endl;
                        //    const char* name=indexMap._keys[i]?indexMap._keys[i]:"";
                        //    std::cerr<<" we have key "<<name<<" and val "<<indexMap._values[i]<<std::endl;
                        //}
                        char msg[] = "SeRmanExpr error: undefined variable \"$%s\"";
                        _td.ptError(msg, name);
		    }
		    (*binding)[i] = index;
		}
	    }
	    _bindstack.push_back(binding);
	    return binding;
	}

        void lookupAttrs()
        {
            int nattrs = _attrrefs.size();

            // fill in attrs
            for (int i = 0; i < nattrs; i++) {
                _attrrefs[i]->doLookup();
            }
        }

	void setVarIndices()
	{
	    // set the varref indices to the currently bound varmap
	    // note: we can't do this during bind because expression evals may be nested
	    SeVarBinding* binding = _bindstack.back();
	    if (binding) {
		for (int i = 0, size = binding->size(); i < size; i++)
		    _varrefs[i]->setIndex((*binding)[i]);
	    }
	}

	void unbindVars()
	{
	    _bindstack.pop_back();
	}

     private:
	mutable std::vector<const char*> _varnames;         // ordered, unique list of var names
	mutable std::vector<SeRmanVar*> _varrefs;           // var refs corresponding to _varnames
        mutable std::vector<AttrVar*> _attrrefs;
	mutable std::map<const char*, SeVarBinding*> _bindings; // bindings for each varmap
	mutable std::vector<SeVarBinding*> _bindstack;           // stack of active bindings
	ThreadData& _td;
	int _boundVarMap;
    };

    void init(RixContext* ctx)
    {
        tokenizer = (RixTokenStorage*) ctx->GetRixInterface(k_RixGlobalTokenData);

        // temporarily unset the expr plugins path
        char* plugins_ptr = getenv("SE_EXPR_PLUGINS");
        std::string plugins;
        if (plugins_ptr) {
            plugins = plugins_ptr;
            unsetenv("SE_EXPR_PLUGINS");
        }

        // and init the plugins explicitly (they're statically linked)
        SeExprFunc::init();
        //DefineSeExprPlugin (SeExprFunc::define);

        // restore environment
        if (plugins_ptr)
            setenv("SE_EXPR_PLUGINS", plugins.c_str(), 1);
    }

    int SeExprBind(RslContext* ctx, int argc, const RslArg* argv[])
    {
        RslFloatIter result(argv[0]);
        const char* exprstr = *RslStringIter(argv[1]);
        const char* varmapHandle = *RslStringIter(argv[2]);

        if (!exprstr[0]) {
            // expression is blank - just return null handle
            *result = 0;
	    argv[3]->GetResizer()->Resize(0);
            return 0;
        }

        // see if we have this expr already
        ThreadData& td = getThreadData(ctx);
        int& index = td.exprmap[exprstr];
        if (!index) {
            // parse expr (parser is not reentrant - use mutex)
            static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
            pthread_mutex_lock(&mutex);
            SeRmanExpr* expr = new SeRmanExpr(exprstr, td);
            bool valid = expr->isValid(); // triggers parse
            pthread_mutex_unlock(&mutex);
            if (!valid) {
                char msg[] = "SeRmanExpr error: %s";
                td.ptError(msg, expr->parseError().c_str());
                index = 0;
            }
            else index = td.exprs.size();
            // store expr object whether parse succeeded or not (so we don't parse again)
            td.exprs.push_back(expr);
        }

        *result = index;
        if (index) {
            // bind vars only if we have a valid expr
            SeRmanExpr* expr = td.exprs[index];
            expr->lookupAttrs();
            SeVarBinding& binding = *expr->bindVars(varmapHandle);
            int nvars = binding.size();
	    argv[3]->GetResizer()->Resize(nvars);
#if RSL_PLUGIN_VERSION >= 6
            RslFloatArrayIter varIndices(argv[3]);
#else
            float* varIndices = *RslFloatArrayIter(argv[3]);
#endif
            for (int i = 0; i < nvars; i++) {
                varIndices[i] = binding[i];
            }
        }
        else {
	    argv[3]->GetResizer()->Resize(0);
        }
        return 0;
    }

    int SeExprEval(RslContext* ctx, int argc, const RslArg* argv[])
    {
        int index = int(*RslFloatIter(argv[1]));
        RslColorArrayIter varValuesIter=argv[2];
        RslColorIter CiIter=argv[3];

        int numVals = argv[3]->NumValues();

        if (!index) {
            for (int i = 0; i < numVals; i++, CiIter++, varValuesIter++) {
                float* Ci = *CiIter;
                Ci[0] = Ci[1] = Ci[2] = 0;
            }
            return 0;
        }

        ThreadData& td = getThreadData(ctx);

        SeRmanExpr& expr = *td.exprs[index];
        expr.setVarIndices();

        bool isThreadSafe = expr.isThreadSafe();

        for (int i = 0; i < numVals; i++, CiIter++, varValuesIter++) {
            td.varValues = &varValuesIter[0];
            float* Ci = td.Ci = *CiIter;

            // expression evaluator is reentrant but functions may not be
            static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
            if (!isThreadSafe)
                pthread_mutex_lock(&mutex);

            SeVec3d v = expr.evaluate();

            if (!isThreadSafe)
                pthread_mutex_unlock(&mutex);


            if (!isfinite(v[0]) || !isfinite(v[1]) || !isfinite(v[2])) {
                char msg[] = "Shader Expression: %s: resulted in NAN. Setting val to 1";
                td.ptWarn (msg, expr.getExpr().c_str());
                v[0] = v[1] = v[2] = 1;
	    }

	    Ci[0] = v[0];
	    Ci[1] = v[1];
	    Ci[2] = v[2];
        }

        expr.unbindVars();
        return 0;
    }
}

extern "C" {
    static RslFunction funcs[] = {
	// SeTokenize(inputStr) -> tokenized string
        {"string SeTokenize(string)", SeTokenize, NULL, NULL },

	// SeExprBind(exprStr, varmap, varIndices[]) -> exprHandle
        {"float SeExprBind(string, string, output uniform float[])", SeExprBind, NULL, NULL },

	// SeExprEval(exprHandle, varValues, CsVal)
        {"void SeExprEval(uniform float, color[], output color)", SeExprEval, NULL, NULL },
        NULL
    };

    RslFunctionTable RslPublicFunctions(funcs, init);
}
