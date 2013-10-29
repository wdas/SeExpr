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
#ifndef SeMutex_h
#define SeMutex_h


// #define DEBUG_THREADING

#include "SePlatform.h"

/** For internal use only */
namespace SeExprInternal {
#ifndef NDEBUG
    template <class T>
    class DebugLock : public T {
     public:
	DebugLock() : _locked(0) {}
	void lock()   { T::lock(); _locked = 1; }
	void unlock() { assert(_locked); _locked = 0; T::unlock(); }
	bool locked() { return _locked != 0; }
     private:
	int _locked;
    };
#endif

    /** Automatically acquire and release lock within enclosing scope. */
    template <class T>
    class AutoLock {
    public:
	AutoLock(T& m) : _m(m) { _m.lock(); }
	~AutoLock()            { _m.unlock(); }
    private:
	T& _m;
    };

#ifndef NDEBUG
    // add debug wrappers to mutex and spinlock
    typedef DebugLock<_Mutex> Mutex;
    typedef DebugLock<_SpinLock> SpinLock;
#else
    typedef _Mutex Mutex;
    typedef _SpinLock SpinLock;
#endif

    typedef AutoLock<Mutex> AutoMutex;
    typedef AutoLock<SpinLock> AutoSpin;
}

#endif
