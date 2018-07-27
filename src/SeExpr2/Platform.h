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
#ifndef Platform_h
#define Platform_h

/** @file Platform.h
    @brief Platform-specific classes, functions, and includes.
*/
#include <iostream>

#ifdef __APPLE__
#include <Availability.h>
#include <libgen.h>
#endif

// platform-specific includes
#if defined(_WIN32) || defined(_WINDOWS) || defined(_MSC_VER)
#ifndef WINDOWS
#define WINDOWS
#endif
#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_DEPRECATE 1
#if !defined(NOMINMAX)
#define NOMINMAX 1
#endif

// note: because there are some conflicts preventing the use of
// windows.h and COFF.h (one of LLVM include files) in the same
// compilation unit (https://groups.google.com/forum/#!topic/llvm-dev/6n5Q0pFdaSA)
// do NOT include windows.h here. The Windows implementation is
// done on the Platform.cpp file, using opaque types.

#include <malloc.h>
#include <io.h>
#include <tchar.h>
#include <process.h>

#else

// linux/unix/posix
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <sys/time.h>
// OS for spinlock
#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#include <sys/types.h>
#endif
#endif  // defined(_WIN32)...

// general includes
#include <stdio.h>
#include <math.h>
#include <assert.h>

// missing functions on Windows
#ifdef WINDOWS
#define snprintf sprintf_s
#define strtok_r strtok_s
typedef __int64 FilePos;
#define fseeko _fseeki64
#define ftello _ftelli64

inline double log2(double x) { return log(x) * 1.4426950408889634; }

typedef unsigned int uint32_t;
#define M_E (2.7182818284590452354)
#define M_PI (3.141592653589793238)
#if !defined(UINT32_MAX)
#define UINT32_MAX (0xffffffff)
#endif
#if !defined(UINT32_MAX)
#define UINT32_MIN (0)
#endif
#else
typedef off_t FilePos;
#endif

namespace SeExpr2 {
#ifndef WINDOWS

class Timer {
#ifdef __APPLE__
    typedef struct timeval Time;
#else
    typedef timespec Time;
#endif
    Time startTime, stopTime;
    bool started;

  public:
    Timer() : started(false) {}

    void start() {
        started = true;
#ifdef __APPLE__
        gettimeofday(&startTime, 0);
#else
        clock_gettime(CLOCK_MONOTONIC, &startTime);
#endif
    }

    long elapsedTime() {
        assert(started);
#ifdef __APPLE__
        gettimeofday(&stopTime, 0);
        long seconds = stopTime.tv_sec - startTime.tv_sec;
        long useconds = stopTime.tv_usec - startTime.tv_usec;
        long elapsedTime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
#else
        clock_gettime(CLOCK_MONOTONIC, &stopTime);
        long seconds = stopTime.tv_sec - startTime.tv_sec;
        long nseconds = stopTime.tv_nsec - startTime.tv_nsec;
        long elapsedTime = ((seconds) * 1000 + nseconds / 1000000.0) + 0.5;
#endif
        return elapsedTime;
    }
};
#else  // Windows
class Timer {
    __int64 time();
    __int64 ticksPerSeconds;
    __int64 startTime, stopTime;
    bool started;

  public:
    Timer();
    void start();
    long elapsedTime();
};
#endif

class PrintTiming {
  public:
    PrintTiming(const std::string& s) : _s(s) { _timer.start(); }

    ~PrintTiming() { std::cout << _s.c_str() << " (" << _timer.elapsedTime() << " ms)" << std::endl; }


  private:
    Timer _timer;
    const std::string _s;
};
}

namespace SeExprInternal2 {

/*
 * Mutex/SpinLock classes
 */

#ifdef WINDOWS

class _Mutex {
  public:
    _Mutex();
    ~_Mutex();
    void lock();
    void unlock();

  private:
    void* _mutex;
};

class _SpinLock {
  public:
    _SpinLock();
    ~_SpinLock();
    void lock();
    void unlock();

  private:
    void* _spinlock;
};

#else
// assume linux/unix/posix
class _Mutex {
  public:
    _Mutex() { pthread_mutex_init(&_mutex, 0); }
    ~_Mutex() { pthread_mutex_destroy(&_mutex); }
    void lock() { pthread_mutex_lock(&_mutex); }
    void unlock() { pthread_mutex_unlock(&_mutex); }

  private:
    pthread_mutex_t _mutex;
};

#ifdef __APPLE__
class _SpinLock {
  public:
    _SpinLock() { _spinlock = 0; }
    ~_SpinLock() {}
    void lock() { OSSpinLockLock(&_spinlock); }
    void unlock() { OSSpinLockUnlock(&_spinlock); }

  private:
    OSSpinLock _spinlock;
};
#else
class _SpinLock {
  public:
    _SpinLock() { pthread_spin_init(&_spinlock, PTHREAD_PROCESS_PRIVATE); }
    ~_SpinLock() { pthread_spin_destroy(&_spinlock); }
    void lock() { pthread_spin_lock(&_spinlock); }
    void unlock() { pthread_spin_unlock(&_spinlock); }

  private:
    pthread_spinlock_t _spinlock;
};
#endif  // __APPLE__
#endif
}

#endif  // Platform_h
