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
#ifndef _vectest2_h_
#define _vectest2_h_
#include <iosfwd>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include <OpenEXR/ImathVec.h>

//#############################################################################
// Template Metaprogramming Helpers
namespace SeExpr2 {
//! Static assert error case (false)
template <bool b, class T>
struct seexpr_static_assert {};
//! Static assert success case
template <class T>
struct seexpr_static_assert<true, T> {
    typedef T TYPE;
};

//! Enable_if success case (can find the type TYPE)
template <bool c, class T = void>
struct my_enable_if {
    typedef T TYPE;
};
//! Enable_if failure case (substitution failure is not an error)
template <class T>
struct my_enable_if<false, T> {};

//! Static conditional type true case
template <bool c, class T1, class T2>
struct static_if {
    typedef T1 TYPE;
};
//! Static conditional type false case
template <class T1, class T2>
struct static_if<false, T1, T2> {
    typedef T2 TYPE;
};

//#############################################################################
// Reduction class (helps prevent linear data dependency on reduce unroll)
template <class T, int d>
struct Reducer {
    static T sum(T* data) {
        T sum = 0;
        for (int k = 0; k < d; k++) sum += data[k];
        return sum;
    }
};
template <class T>
struct Reducer<T, 1> {
    static T sum(T* data) { return data[0]; }
};
template <class T>
struct Reducer<T, 2> {
    static T sum(T* data) { return data[0] + data[1]; }
};
template <class T>
struct Reducer<T, 3> {
    static T sum(T* data) { return data[0] + data[1] + data[2]; }
};
template <class T>
struct Reducer<T, 4> {
    static T sum(T* data) { return (data[0] + data[1]) + (data[2] + data[3]); }
};

template<class T, class T2>
Imath::Vec2<T> copyRawVec2(const T2* raw) {
    Imath::Vec2<T> result;
    for (int i = 0; i < 2; ++i) result[i] = static_cast<T>(raw[i]);
    return result;
}

template<class T, class T2>
void copyRawVec2(Imath::Vec2<T>& dst, const T2* raw) {
    for (int i = 0; i < 2; ++i) dst[i] = static_cast<T>(raw[i]);
}

template<class T, class T2>
Imath::Vec3<T> copyRawVec3(const T2* raw) {
    Imath::Vec3<T> result;
    for (int i = 0; i < 3; ++i) result[i] = static_cast<T>(raw[i]);
    return result;
}

template<class T, class T2>
void copyRawVec3(Imath::Vec3<T>& dst, const T2* raw) {
    for (int i = 0; i < 3; ++i) dst[i] = static_cast<T>(raw[i]);
}

template<class T, class T2>
Imath::Vec4<T> copyRawVec4(const T2* raw) {
    Imath::Vec4<T> result;
    for (int i = 0; i < 4; ++i) result[i] = static_cast<T>(raw[i]);
    return result;
}

template<class T, class T2>
void copyRawVec4(Imath::Vec4<T>& dst, const T2* raw) {
    for (int i = 0; i < 4; ++i) dst[i] = static_cast<T>(raw[i]);
}

using Vec2d = Imath::Vec2<double>;
using Vec3d = Imath::Vec3<double>;
using Vec4d = Imath::Vec4<double>;
using Vec2f = Imath::Vec2<float>;
using Vec3f = Imath::Vec3<float>;
using Vec4f = Imath::Vec4<float>;
}
#endif
