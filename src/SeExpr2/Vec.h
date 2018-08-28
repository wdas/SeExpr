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
#include "Platform.h"

// To fix differences in template TYPENAME resolution between MSVC and other compilers
#if defined(WINDOWS)
#   define TYPENAME
#else
#   define TYPENAME typename
#endif

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
struct my_enable_if<false, T> {
#if defined(WINDOWS)
    typedef void TYPE;
#endif
};

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

//! Vec class, generic dimension vector class
//! can also point to data if the template argument ref is true
template <class T, int d, bool ref = false>
class Vec {
    // static error types
    struct INVALID_WITH_VECTOR_VALUE {};
    struct INVALID_WITH_VECTOR_REFERENCE {};
    struct INVALID_WITH_DIMENSION {};

    //! internal data (either an explicit arary or a pointer to raw data)
    typename static_if<ref, T*, T[d]>::TYPE x;

  public:
    typedef Vec<T, d, false> T_VEC_VALUE;
    typedef Vec<T, d, true> T_VEC_REF;

    //! Initialize vector value using raw memory
    template <class T2>
    static Vec<T, d, false> copy(T2* raw,
                                 INVALID_WITH_VECTOR_REFERENCE u =
                                     (TYPENAME my_enable_if<!ref, INVALID_WITH_VECTOR_REFERENCE>::TYPE())) {
        Vec<T, d, false> ret;
        for (int k = 0; k < d; k++) ret[k] = static_cast<T>(raw[k]);
        return ret;
    }

    //! Initialize vector to be reference to plain raw data
    explicit Vec(T* raw, INVALID_WITH_VECTOR_VALUE u = (TYPENAME my_enable_if<ref, INVALID_WITH_VECTOR_VALUE>::TYPE()))
        : x(raw) {}

    //! Empty constructor (this is invalid for a reference type)
    Vec(INVALID_WITH_VECTOR_REFERENCE u = (TYPENAME my_enable_if<!ref, INVALID_WITH_VECTOR_REFERENCE>::TYPE())) {}

    //! Convenience constant vector initialization (valid for any d)
    Vec(T v0, INVALID_WITH_VECTOR_REFERENCE u = (TYPENAME my_enable_if<!ref, INVALID_WITH_VECTOR_REFERENCE>::TYPE())) {
        for (int k = 0; k < d; k++) x[k] = v0;
    }

    //! Convenience 2 vector initialization (only for d==2)
    Vec(T v1,
        T v2,
        INVALID_WITH_VECTOR_REFERENCE u = (TYPENAME my_enable_if<!ref, INVALID_WITH_VECTOR_REFERENCE>::TYPE())) {
        TYPENAME seexpr_static_assert<d == 2, INVALID_WITH_DIMENSION>::TYPE();
        x[0] = v1;
        x[1] = v2;
    }

    //! Convenience 3 vector initialization (only for d==3)
    Vec(T v1,
        T v2,
        T v3,
        INVALID_WITH_VECTOR_REFERENCE u = (TYPENAME my_enable_if<!ref, INVALID_WITH_VECTOR_REFERENCE>::TYPE())) {
        TYPENAME seexpr_static_assert<d == 3, INVALID_WITH_DIMENSION>::TYPE();
        x[0] = v1;
        x[1] = v2;
        x[2] = v3;
    }

    //! Convenience 4 vector initialization (only for d==4)
    Vec(T v1,
        T v2,
        T v3,
        T v4,
        INVALID_WITH_VECTOR_REFERENCE u = (TYPENAME my_enable_if<!ref, INVALID_WITH_VECTOR_REFERENCE>::TYPE())) {
        TYPENAME seexpr_static_assert<d == 4, INVALID_WITH_DIMENSION>::TYPE();
        x[0] = v1;
        x[1] = v2;
        x[2] = v3;
        x[3] = v4;
    }
    // Changed this to default. This is safe! for reference case it makes another reference
    // for value it copies
    //! Copy construct. Only valid if we are not going to be a reference data!
    // Vec(const Vec&)
    //{TYPENAME static_assert<!ref,INVALID_WITH_VECTOR_REFERENCE>::TYPE();}

    //! Copy construct. Only valid if we are not going to be reference data!
    template <class T2, bool refother>
    Vec(const Vec<T2, d, refother>& other,
        INVALID_WITH_VECTOR_REFERENCE u =
            (TYPENAME my_enable_if<!ref && refother != ref, INVALID_WITH_VECTOR_REFERENCE>::TYPE())) {
        *this = other;
    }

    template <class T2, bool refother>
    Vec& operator=(const Vec<T2, d, refother>& other) {
        for (int k = 0; k < d; k++) x[k] = other[k];
        return *this;
    }

    template <class Tother, bool refother>
    bool operator==(const Vec<Tother, d, refother>& other) const {
        for (int k = 0; k < d; k++)
            if (x[k] != other[k]) return false;
        return true;
    }

    template <class Tother, bool refother>
    bool operator!=(const Vec<Tother, d, refother>& other) const {
        return !(*this != other);
    }

    // non-const element access
    T& operator[](const int i) { return x[i]; }

    // const element access
    const T& operator[](const int i) const { return x[i]; }

    //! Square of euclidean (2) norm
    T length2() const {
        T data[d];
        for (int k = 0; k < d; k++) data[k] = x[k] * x[k];
        return Reducer<T, d>::sum(data);
    }

    //! Euclidean (2) norm
    T length() const { return sqrt(length2()); }

    //! Normalize in place and return the 2-norm before normalization
    T normalize() {
        T l = length2();
        if (l) {
            l = sqrt(l);
            *this /= l;
        } else {
            *this = T_VEC_VALUE((T)0);
            x[0] = 1;
        }
        return l;
    }

    //! Return a copy of the vector that is normalized
    Vec<T, d, false> normalized() const {
        Vec<T, d, false> other(*this);
        other.normalize();
        return other;
    }

    Vec& operator/=(const T val) {
        T one_over_val = T(1) / val;
        for (int k = 0; k < d; k++) x[k] *= one_over_val;
        return *this;
    }

    Vec& operator*=(const T val) {
        for (int k = 0; k < d; k++) x[k] *= val;
        return *this;
    }

    template <bool refother>
    Vec& operator+=(const Vec<T, d, refother>& other) {
        for (int k = 0; k < d; k++) x[k] += other[k];
        return *this;
    }

    template <bool refother>
    Vec& operator-=(const Vec<T, d, refother>& other) {
        for (int k = 0; k < d; k++) x[k] -= other[k];
        return *this;
    }

    template <bool refother>
    Vec& operator*=(const Vec<T, d, refother>& other) {
        for (int k = 0; k < d; k++) x[k] *= other[k];
        return *this;
    }

    template <bool refother>
    Vec& operator/=(const Vec<T, d, refother>& other) {
        for (int k = 0; k < d; k++) x[k] /= other[k];
        return *this;
    }

    T_VEC_VALUE operator-() const {
        T_VEC_VALUE val(*this);
        for (int k = 0; k < d; k++) val[k] = -val[k];
        return val;
    }

    template <bool refother>
    bool operator==(const Vec<T, d, refother>& other) const {
        bool equal = true;
        for (int k = 0; k < d; k++) equal &= (x[k] == other[k]);
        return equal;
    }

    template <bool refother>
    bool operator!=(const Vec<T, d, refother>& other) const {
        return !(*this == other);
    }

    T_VEC_VALUE operator*(T s) const {
        T_VEC_VALUE val(*this);
        val *= s;
        return val;
    }

    T_VEC_VALUE operator/(T s) const {
        T_VEC_VALUE val(*this);
        val /= s;
        return val;
    }

    template <bool refother>
    T_VEC_VALUE operator+(const Vec<T, d, refother>& other) const {
        T_VEC_VALUE val(*this);
        val += other;
        return val;
    }

    template <bool refother>
    T_VEC_VALUE operator-(const Vec<T, d, refother>& other) const {
        T_VEC_VALUE val(*this);
        val -= other;
        return val;
    }

    template <bool refother>
    T_VEC_VALUE operator*(const Vec<T, d, refother>& other) const {
        T_VEC_VALUE val(*this);
        val *= other;
        return val;
    }

    template <bool refother>
    T_VEC_VALUE operator/(const Vec<T, d, refother>& other) const {
        T_VEC_VALUE val(*this);
        val /= other;
        return val;
    }

    friend T_VEC_VALUE operator*(T s, const Vec& v) { return v * s; }

    /** Inner product. */
    template <bool refother>
    T dot(const Vec<T, d, refother>& o) const {
        T data[d];
        for (int k = 0; k < d; k++) data[k] = x[k] * o[k];
        return Reducer<T, d>::sum(data);
    }

    /** Cross product. */
    template <bool refother>
    T_VEC_VALUE cross(const Vec<T, 3, refother>& o) const {
        TYPENAME seexpr_static_assert<d == 3, INVALID_WITH_DIMENSION>::TYPE();
        return T_VEC_VALUE(x[1] * o[2] - x[2] * o[1], x[2] * o[0] - x[0] * o[2], x[0] * o[1] - x[1] * o[0]);
    }

    /** Return a vector orthogonal to the current vector. */
    T_VEC_VALUE orthogonal() const {
        TYPENAME seexpr_static_assert<d == 3, INVALID_WITH_DIMENSION>::TYPE();
        return T_VEC_VALUE(x[1] + x[2], x[2] - x[0], -x[0] - x[1]);
    }

    /**
     * Returns the angle in radians between the current vector and the
     * passed in vector.
     */
    template <bool refother>
    T angle(const Vec<T, 3, refother>& o) const {
        TYPENAME seexpr_static_assert<d == 3, INVALID_WITH_DIMENSION>::TYPE();
        T l = length() * o.length();
        if (l == 0) return 0;
        return acos(dot(o) / l);
    }

    /**
     * Returns the vector rotated by the angle given in radians about
     * the given axis. (Axis must be normalized)
     */
    template <bool refother>
    T_VEC_VALUE rotateBy(const Vec<T, 3, refother>& axis, T angle) const {
        TYPENAME seexpr_static_assert<d == 3, INVALID_WITH_DIMENSION>::TYPE();
        double c = cos(angle), s = sin(angle);
        return c * (*this) + (1 - c) * dot(axis) * axis - s * cross(axis);
    }
};

//! Output stream
template <class T, int d, bool r>
std::ostream& operator<<(std::ostream& out, const Vec<T, d, r>& val) {
    if (d > 0) out << "(" << val[0];
    for (int k = 1; k < d; k++) out << "," << val[k];
    out << ")";
    return out;
}

using Vec1d = Vec<double, 1, false>;
using Vec2d = Vec<double, 2, false>;
using Vec3d = Vec<double, 3, false>;
using Vec4d = Vec<double, 4, false>;
using Vec1f = Vec<float, 1, false>;
using Vec2f = Vec<float, 2, false>;
using Vec3f = Vec<float, 3, false>;
using Vec4f = Vec<float, 4, false>;
using Vec1dRef = Vec<double, 1, true>;
using Vec2dRef = Vec<double, 2, true>;
using Vec3dRef = Vec<double, 3, true>;
using Vec4dRef = Vec<double, 4, true>;
using Vec1fRef = Vec<float, 1, true>;
using Vec2fRef = Vec<float, 2, true>;
using Vec3fRef = Vec<float, 3, true>;
using Vec4fRef = Vec<float, 4, true>;
using Vec1dConstRef = Vec<const double, 1, true>;
using Vec2dConstRef = Vec<const double, 2, true>;
using Vec3dConstRef = Vec<const double, 3, true>;
using Vec4dConstRef = Vec<const double, 4, true>;
using Vec1fConstRef = Vec<const float, 1, true>;
using Vec2fConstRef = Vec<const float, 2, true>;
using Vec3fConstRef = Vec<const float, 3, true>;
using Vec4fConstRef = Vec<const float, 4, true>;
}
#endif
