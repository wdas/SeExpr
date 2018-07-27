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

/** @file Platform.cpp
    @brief Platform-specific classes, functions, and includes' implementations.
*/

#include "Platform.h"

#if defined(WINDOWS)

#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_DEPRECATE 1
#define NOMINMAX 1

// windows - defined for both Win32 and Win64
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

namespace SeExpr2 {

__int64 Timer::time() {
    LARGE_INTEGER perfCounter;
    QueryPerformanceCounter(&perfCounter);
    return perfCounter.QuadPart;
}

Timer::Timer() : started(false) {
    // get the timer frequency
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    ticksPerSeconds = frequency.QuadPart;
}

void Timer::start() {
    started = true;
    startTime = this->time();
}

long Timer::elapsedTime() {
    stopTime = this->time();
    return static_cast<long>(((stopTime - startTime) * 1000000) / ticksPerSeconds);
}

}

namespace SeExprInternal2 {

/*
 * Mutex/SpinLock classes
 */

_Mutex::_Mutex() {
    _mutex = CreateMutex(NULL, FALSE, NULL);
}

_Mutex::~_Mutex() {
    CloseHandle(_mutex);
}

void _Mutex::lock() {
    WaitForSingleObject(_mutex, INFINITE);
}

void _Mutex::unlock() {
    ReleaseMutex(_mutex);
}

_SpinLock::_SpinLock() {
    _spinlock = new CRITICAL_SECTION;
    InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(_spinlock));
}

_SpinLock::~_SpinLock() {
    DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(_spinlock));
    delete _spinlock;
    _spinlock = nullptr;
}

void _SpinLock::lock() {
    EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(_spinlock));
}

void _SpinLock::unlock() {
    LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(_spinlock));
}

}

#endif // defined(WINDOWS)
