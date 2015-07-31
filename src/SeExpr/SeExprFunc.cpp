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
#include <string>
#include <map>
#include <stdlib.h>

#ifndef SEEXPR_WIN32
#include <dlfcn.h>
#include <dirent.h>
#endif



#include "SeExpression.h"
#include "SeExprFunc.h"
#include "SeExprNode.h"
#include "SeExprBuiltins.h"

#include "SeMutex.h"

namespace {
    // FuncTable - table of pre-defined functions
    class FuncTable {
    public:
        FuncTable()
            :_inited(false)
        {}

        ~FuncTable(){
            funcmap.clear();
#ifdef SEEXPR_WIN32
#else
            for(size_t i=0;i<dynamicLibraries.size();i++){
                dlclose(dynamicLibraries[i]);
            }
#endif
        }

	void define(const char* name, SeExprFunc f,const char* docString=0) {
            if(docString) funcmap[name] = FuncMapItem(std::string(docString),f); 
            else funcmap[name] = FuncMapItem(name,f); 
        }

	const SeExprFunc* lookup(const std::string& name)
	{
	    FuncMap::iterator iter;
	    if ((iter = funcmap.find(name)) != funcmap.end())
		return &iter->second.second;
	    return 0;
	}
        void initIfNeeded();
	void initBuiltins();

        void getFunctionNames(std::vector<std::string>& names)
        {
            for(FuncMap::iterator i=funcmap.begin();i!=funcmap.end();++i)
                names.push_back(i->first);
        }

        std::string getDocString(const char* functionName)
        {
            FuncMap::iterator i=funcmap.find(functionName);
            if(i==funcmap.end()) return "";
            else return i->second.first;
        }

        void addLibraryReference(void* lib)
        {
            dynamicLibraries.push_back(lib);
        }
	
    private:
        bool _inited;
        typedef std::pair<std::string,SeExprFunc> FuncMapItem;
	typedef std::map<std::string,FuncMapItem> FuncMap;
        std::vector<void*> dynamicLibraries;
	FuncMap funcmap;
    };

    FuncTable Functions;

inline static void 
defineInternal(const char* name,SeExprFunc f)
{
    // THIS FUNCTION IS NOT THREAD SAFE, it assumes you have a mutex from callee
    // ALSO YOU MUST BE VERY CAREFUL NOT TO CALL ANYTHING THAT TRIES TO REACQUIRE MUTEX!
    Functions.define(name,f);
}

inline static void 
defineInternal3(const char* name,SeExprFunc f,const char* docString)
{
    // THIS FUNCTION IS NOT THREAD SAFE, it assumes you have a mutex from callee
    // ALSO YOU MUST BE VERY CAREFUL NOT TO CALL ANYTHING THAT TRIES TO REACQUIRE MUTEX!
    Functions.define(name,f,docString);
}

void FuncTable::initIfNeeded(){
    // THIS FUNCTION IS NOT THREAD SAFE, it assumes you have a mutex from callee
    // ALSO YOU MUST BE VERY CAREFUL NOT TO CALL ANYTHING THAT TRIES TO REACQUIRE MUTEX!
    if(_inited) return;
    _inited=true;
    
    // TODO: make thread safe
    SeExpr::defineBuiltins(defineInternal,defineInternal3);
    const char* path = getenv("SE_EXPR_PLUGINS");
    if (path) SeExprFunc::loadPlugins(path);
    
}

} // namespace


bool SeExprFuncX::prep(SeExprFuncNode* node, bool wantVec)
{
    /* call base node prep by default:
       this passes wantVec to all the children and sets isVec true if any
       child is a vec */
    return node->SeExprNode::prep(wantVec);
}


static SeExprInternal::Mutex mutex;

void SeExprFunc::init()
{
    SeExprInternal::AutoMutex locker(mutex);
    Functions.initIfNeeded();
}

const SeExprFunc*
SeExprFunc::lookup(const std::string& name)
{
    mutex.lock();
    Functions.initIfNeeded();
    const SeExprFunc* ret=Functions.lookup(name);
    mutex.unlock();
    return ret;
}


void
SeExprFunc::define(const char* name, SeExprFunc f)
{
    mutex.lock();
    Functions.initIfNeeded();
    defineInternal(name,f);
    mutex.unlock();
}

void
SeExprFunc::define(const char* name, SeExprFunc f,const char* docString)
{
    mutex.lock();
    Functions.initIfNeeded();
    defineInternal3(name,f,docString);
    mutex.unlock();
}

void 
SeExprFunc::getFunctionNames(std::vector<std::string>& names)
{
    mutex.lock();
    Functions.initIfNeeded();
    Functions.getFunctionNames(names);
    mutex.unlock();
}

std::string
SeExprFunc::getDocString(const char* functionName)
{
    mutex.lock();
    Functions.initIfNeeded();
    std::string ret=Functions.getDocString(functionName);
    mutex.unlock();
    return ret;
}

#ifndef SEEXPR_WIN32

#if defined(__APPLE__) && !defined(__MAC_10_9)
static int MatchPluginName(struct dirent* dir)
#else
static int MatchPluginName(const struct dirent* dir)
#endif
{
    const char* name = dir->d_name;
    // return true if name matches SeExpr*.so
    return 
	!strncmp(name, "SeExpr", 6) &&
	!strcmp(name + strlen(name) - 3, ".so");
}
#endif


void
SeExprFunc::loadPlugins(const char* path)
{
#ifdef SEEXPR_WIN32

#else
    // first split path into individual entries
    char* pathdup = strdup(path);
    char* state = 0;
    char* entry = strtok_r(pathdup, ":", &state);
    while (entry) {
	// if entry ends with ".so", load directly
	if ((!strcmp(entry+strlen(entry)-3, ".so")))
	    loadPlugin(entry);
	else {
	    // assume it's a dir - search it for plugins
	    struct dirent** matches = 0;
	    int numMatches = scandir(entry, &matches, MatchPluginName, alphasort);
	    for (int i = 0; i < numMatches; i++) {
		std::string fullpath = entry; fullpath += "/"; 
		fullpath += matches[i]->d_name;
		loadPlugin(fullpath.c_str());
			free(matches[i]);
	    }
	    if (matches) free(matches);
	}

	entry = strtok_r(0, ":", &state);
    }
    free(pathdup);
#endif
}

void
SeExprFunc::loadPlugin(const char* path)
{
#ifdef SEEXPR_WIN32
    std::cerr<<"SeExpr: warning Plugins are not supported on windows currently"<<std::endl;
#else
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
	std::cerr << "Error reading expression plugin: " << path << std::endl;
	const char* err = dlerror();
	if (err) std::cerr << err << std::endl;
	return;
    }
    typedef void (*initfn_v1) (SeExprFunc::Define);
    initfn_v1 init_v1 = (initfn_v1) dlsym(handle, "SeExprPluginInit");
    typedef void (*initfn_v2) (SeExprFunc::Define3);
    initfn_v2 init_v2 = (initfn_v2) dlsym(handle, "SeExprPluginInitV2");

    if(init_v2){
        init_v2(defineInternal3);
        Functions.addLibraryReference(handle);
    }else if(init_v1){
        init_v1(defineInternal);
        Functions.addLibraryReference(handle);
    }else{
	std::cerr << "Error reading expression plugin: " << path << std::endl;
	std::cerr << "No function named SeExprPluginInit or SeExprPluginInitV2 found" << std::endl;
	dlclose(handle);
	return;
    }
#endif
}

