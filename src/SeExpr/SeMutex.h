/* 
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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
