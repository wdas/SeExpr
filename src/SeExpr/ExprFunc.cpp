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
#include <string>
#include <map>
#include <stdlib.h>
#include <iostream>
#ifndef SEEXPR_WIN32
#include <dlfcn.h>
#include <dirent.h>
#endif

#include "Expression.h"
#include "ExprFunc.h"
#include "ExprNode.h"
#include "ExprBuiltins.h"

#include "Mutex.h"

namespace {
// FuncTable - table of pre-defined functions
class FuncTable {
  public:
    void define(const char* name, SeExpr2::ExprFunc f, const char* docString = 0)
    {
        if (docString)
            funcmap[name] = FuncMapItem(std::string(docString), f);
        else
            funcmap[name] = FuncMapItem(name, f);
    }

    const SeExpr2::ExprFunc* lookup(const std::string& name)
    {
        FuncMap::iterator iter;
        if ((iter = funcmap.find(name)) != funcmap.end())
            return &iter->second.second;
        return 0;
    }
    void initBuiltins();

    void getFunctionNames(std::vector<std::string>& names)
    {
        for (FuncMap::iterator i = funcmap.begin(); i != funcmap.end(); ++i)
            names.push_back(i->first);
    }

    std::string getDocString(const char* functionName)
    {
        FuncMap::iterator i = funcmap.find(functionName);
        if (i == funcmap.end())
            return "";
        else
            return i->second.first;
    }

    size_t sizeInBytes() const
    {
        size_t totalSize = 0;
        for (FuncMap::const_iterator it = funcmap.begin(); it != funcmap.end(); ++it) {
            totalSize += it->first.size() + sizeof(FuncMapItem);
            const SeExpr2::ExprFunc& function = it->second.second;
            if (const SeExpr2::ExprFuncX* funcx = function.funcx()) {
                totalSize += funcx->sizeInBytes();
            }
        }
        return totalSize;
    }

    SeExpr2::Statistics statistics() const
    {
        SeExpr2::Statistics statisticsDump;
        size_t totalSize = 0;
        for (FuncMap::const_iterator it = funcmap.begin(); it != funcmap.end(); ++it) {
            totalSize += it->first.size() + sizeof(FuncMapItem);
            const SeExpr2::ExprFunc& function = it->second.second;
            if (const SeExpr2::ExprFuncX* funcx = function.funcx()) {
                funcx->statistics(statisticsDump);
            }
        }
        return statisticsDump;
    }

  private:
    typedef std::pair<std::string, SeExpr2::ExprFunc> FuncMapItem;
    typedef std::map<std::string, FuncMapItem> FuncMap;
    FuncMap funcmap;
};

FuncTable* Functions = 0;
}

// ExprType ExprFuncX::prep(ExprFuncNode* node, bool scalarWanted, ExprVarEnv & env) const
//{
//    /* call base node prep by default:
//       this passes wantVec to all the children and sets isVec true if any
//       child is a vec */
//    /* TODO: check that this is correct behavior */
//    return node->ExprNode::prep(scalarWanted, env);
//}

namespace SeExpr2 {

std::vector<void*> ExprFunc::dynlib;

static SeExprInternal2::Mutex mutex;

void ExprFunc::init()
{
    SeExprInternal2::AutoMutex locker(mutex);
    initInternal();
}

void ExprFunc::cleanup()
{
    SeExprInternal2::AutoMutex locker(mutex);
    delete Functions;
    Functions = nullptr;
#ifdef SEEXPR_WIN32
#else
    for (size_t i = 0; i < dynlib.size(); i++) {
        dlclose(dynlib[i]);
    }
#endif
}

const ExprFunc* ExprFunc::lookup(const std::string& name)
{
    mutex.lock();
    if (!Functions)
        initInternal();
    const ExprFunc* ret = Functions->lookup(name);
    mutex.unlock();
    return ret;
}

inline static void defineInternal(const char* name, ExprFunc f)
{
    // THIS FUNCTION IS NOT THREAD SAFE, it assumes you have a mutex from callee
    // ALSO YOU MUST BE VERY CAREFUL NOT TO CALL ANYTHING THAT TRIES TO REACQUIRE MUTEX!
    Functions->define(name, f);
}

inline static void defineInternal3(const char* name, ExprFunc f, const char* docString)
{
    // THIS FUNCTION IS NOT THREAD SAFE, it assumes you have a mutex from callee
    // ALSO YOU MUST BE VERY CAREFUL NOT TO CALL ANYTHING THAT TRIES TO REACQUIRE MUTEX!
    Functions->define(name, f, docString);
}

void ExprFunc::initInternal()
{
    // THIS FUNCTION IS NOT THREAD SAFE, it assumes you have a mutex from callee
    // ALSO YOU MUST BE VERY CAREFUL NOT TO CALL ANYTHING THAT TRIES TO REACQUIRE MUTEX!

    // TODO: make thread safe
    if (Functions)
        return;
    Functions = new FuncTable;
    SeExpr2::defineBuiltins(defineInternal, defineInternal3);
    const char* path = getenv("SE_EXPR_PLUGINS");
    if (path)
        loadPlugins(path);
}

void ExprFunc::define(const char* name, ExprFunc f)
{
    mutex.lock();
    if (!Functions)
        initInternal();
    defineInternal(name, f);
    mutex.unlock();
}

void ExprFunc::define(const char* name, ExprFunc f, const char* docString)
{
    mutex.lock();
    if (!Functions)
        initInternal();
    defineInternal3(name, f, docString);
    mutex.unlock();
}

void ExprFunc::getFunctionNames(std::vector<std::string>& names)
{
    mutex.lock();
    if (!Functions)
        initInternal();
    Functions->getFunctionNames(names);
    mutex.unlock();
}

std::string ExprFunc::getDocString(const char* functionName)
{
    mutex.lock();
    if (!Functions)
        initInternal();
    std::string ret = Functions->getDocString(functionName);
    mutex.unlock();
    return ret;
}

size_t ExprFunc::sizeInBytes()
{
    SeExprInternal2::AutoMutex locker(mutex);
    if (!Functions)
        initInternal();
    return Functions->sizeInBytes();
}

SeExpr2::Statistics ExprFunc::statistics()
{
    SeExprInternal2::AutoMutex locker(mutex);
    if (!Functions)
        initInternal();
    return Functions->statistics();
}

#ifndef SEEXPR_WIN32

#if defined(__APPLE__) && !defined(__MAC_10_9)
static int MatchPluginName(const struct dirent* dir)
#else
static int MatchPluginName(const struct dirent* dir)
#endif
{
    const char* name = dir->d_name;
    // return true if name matches SeExpr*.so
    return !strncmp(name, "SeExpr", 6) && !strcmp(name + strlen(name) - 3, ".so");
}
#endif

void ExprFunc::loadPlugins(const char* path)
{
#ifdef SEEXPR_WIN32

#else
    // first split path into individual entries
    char* pathdup = strdup(path);
    char* state = 0;
    char* entry = strtok_r(pathdup, ":", &state);
    while (entry) {
        // if entry ends with ".so", load directly
        if ((!strcmp(entry + strlen(entry) - 3, ".so")))
            loadPlugin(entry);
        else {
            // assume it's a dir - search it for plugins
            struct dirent** matches = 0;
            int numMatches = scandir(entry, &matches, MatchPluginName, alphasort);
            for (int i = 0; i < numMatches; i++) {
                std::string fullpath = entry;
                fullpath += "/";
                fullpath += matches[i]->d_name;
                loadPlugin(fullpath.c_str());
                free(matches[i]);
            }
            if (matches)
                free(matches);
            else {
                std::cerr << "No plugins found matching " << path << "/SeExpr*.so" << std::endl;
            }
        }

        entry = strtok_r(0, ":", &state);
    }
    free(pathdup);
#endif
}

void ExprFunc::loadPlugin(const char* path)
{
#ifdef SEEXPR_WIN32
    std::cerr << "SeExpr: warning Plugins are not supported on windows currently" << std::endl;
#else
    void* handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        std::cerr << "Error reading expression plugin: " << path << std::endl;
        const char* err = dlerror();
        if (err)
            std::cerr << err << std::endl;
        return;
    }

    typedef void (*initfn_v3)(ExprFunc::Define3);
    initfn_v3 init_v3 = (initfn_v3)dlsym(handle, "SeExpr2PluginInit");

    if (init_v3) {
        init_v3(defineInternal3);
        dynlib.push_back(handle);
    } else {
        std::cerr << "Error reading expression plugin: " << path << std::endl;
        std::cerr << "No function named SeExpr2PluginInit defined" << std::endl;
        dlclose(handle);
    }
    return;
#endif
}
}
