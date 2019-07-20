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
#include "Expression.h"
#include "ExprBuiltins.h"
#include <cfloat>
#include <cassert>
#include <algorithm>

#include "Curve.h"

namespace SeExpr2 {

template <>
double Curve<double>::comp(const double& val, const int)
{
    return val;
}

template <>
double Curve<Vec3d>::comp(const Vec3d& val, const int i)
{
    return val[i];
}

template <class T>
bool Curve<T>::cvLessThan(const CV& cv1, const CV& cv2)
{
    return cv1._pos < cv2._pos;
}

template <class T>
Curve<T>::Curve() : cacheCV(0), prepared(false)
{
    _cvData.push_back(CV(-FLT_MAX, T(), kNone));
    _cvData.push_back(CV(FLT_MAX, T(), kNone));
}

template <class T>
void Curve<T>::addPoint(double position, const T& val, InterpType type)
{
    prepared = false;
    _cvData.push_back(CV(position, val, type));
}

template <class T>
void Curve<T>::preparePoints()
{
    prepared = true;
    cacheCV = 0;
    // sort
    std::sort(_cvData.begin(), _cvData.end(), cvLessThan);

    // Setup boundary conditions on sentinel values
    CV& end = *(_cvData.end() - 1);
    CV& begin = *(_cvData.begin());
    int realCVs = static_cast<int>(_cvData.size()) - 2;
    assert(realCVs >= 0);
    if (realCVs > 0) {
        begin._val = _cvData[1]._val;
        begin._deriv = T();
        begin._interp = kNone;
        int lastIndex = static_cast<int>(_cvData.size()) - 1;
        end._val = _cvData[lastIndex - 1]._val;
        end._deriv = T();
        end._interp = kNone;
    } else {
        begin._pos = end._pos = 0;
        begin._val = end._val = T();
        begin._interp = kNone;
        begin._deriv = end._deriv = T();
    }

    // Initialize "Catmull-Rom" derivatives (centered differences)
    for (unsigned int i = 1; i < _cvData.size() - 1; i++) {
        _cvData[i]._deriv = (_cvData[i + 1]._val - _cvData[i - 1]._val) / (_cvData[i + 1]._pos - _cvData[i - 1]._pos);
    }

    // Fix extrema by going through all intervals
    for (unsigned int i = 0; i < _cvData.size() - 1; i++) {
        if (_cvData[i]._interp == kMonotoneSpline) {
            double h = _cvData[i + 1]._pos - _cvData[i]._pos;
            if (h == 0)
                _cvData[i]._deriv = _cvData[i + 1]._deriv = T();
            else {
                T delta = (_cvData[i + 1]._val - _cvData[i]._val) / h;
                clampCurveSegment(delta, _cvData[i]._deriv, _cvData[i + 1]._deriv);
            }
        }
    }
}

// TODO: this function and the next could be merged with template magic
//       but it might be simpler to just have two copies!
template <class T>
T Curve<T>::getValue(const double param) const
{
    assert(prepared);
    // find the cv data point index just greater than the desired param
    const int numPoints = static_cast<int>(_cvData.size());
    const CV* cvDataBegin = &_cvData[0];
    int index =
        static_cast<int>(std::upper_bound(cvDataBegin, cvDataBegin + numPoints, CV(param, T(), kLinear), cvLessThan) - cvDataBegin);
    index = std::max(1, std::min(index, numPoints - 1));

    const float t0 = static_cast<float>(_cvData[index - 1]._pos);
    const T k0 = _cvData[index - 1]._val;
    const InterpType interp = _cvData[index - 1]._interp;
    const float t1 = static_cast<float>(_cvData[index]._pos);
    const T k1 = _cvData[index]._val;
    switch (interp) {
    case kNone:
        return k0;
        break;
    case kLinear: {
        double u = (param - t0) / (t1 - t0);
        return k0 + u * (k1 - k0);
    } break;
    case kSmooth: {
        double u = (param - t0) / (t1 - t0);
        return k0 * (u - 1) * (u - 1) * (2 * u + 1) + k1 * u * u * (3 - 2 * u);
    } break;
    case kSpline:
    case kMonotoneSpline: {
        double x = param - _cvData[index - 1]._pos;                // xstart
        double h = _cvData[index]._pos - _cvData[index - 1]._pos;  // xend-xstart
        T y = _cvData[index - 1]._val;                             // f(xstart)
        T delta = _cvData[index]._val - _cvData[index - 1]._val;   // f(xend)-f(xstart)
        T d1 = _cvData[index - 1]._deriv;                          // f'(xstart)
        T d2 = _cvData[index]._deriv;                              // f'(xend)
        return (x * (delta * (3 * h - 2 * x) * x + h * (-h + x) * (-(d1 * h) + (d1 + d2) * x))) / (h * h * h) + y;
    } break;
    default:
        assert(false);
        return T();
        break;
    }
}

// TODO: this function and the previous could be merged with template magic
//       but it might be simpler to just have two copies!
template <class T>
double Curve<T>::getChannelValue(const double param, int channel) const
{
    assert(prepared);
    // find the cv data point index just greater than the desired param
    const int numPoints = static_cast<int>(_cvData.size());
    const CV* cvDataBegin = &_cvData[0];
    int index =
        static_cast<int>(std::upper_bound(cvDataBegin, cvDataBegin + numPoints, CV(param, T(), kLinear), cvLessThan) - cvDataBegin);
    index = std::max(1, std::min(index, numPoints - 1));

    const float t0 = static_cast<float>(_cvData[index - 1]._pos);
    const double k0 = comp(_cvData[index - 1]._val, channel);
    const InterpType interp = _cvData[index - 1]._interp;
    const float t1 = static_cast<float>(_cvData[index]._pos);
    const double k1 = comp(_cvData[index]._val, channel);
    switch (interp) {
    case kNone:
        return k0;
        break;
    case kLinear: {
        double u = (param - t0) / (t1 - t0);
        return k0 + u * (k1 - k0);
    } break;
    case kSmooth:
        // standard cubic interpolation
        {
            double u = (param - t0) / (t1 - t0);
            return k0 * (u - 1) * (u - 1) * (2 * u + 1) + k1 * u * u * (3 - 2 * u);
        }
        break;
    case kSpline:
    case kMonotoneSpline: {
        double x = param - _cvData[index - 1]._pos;                                                  // xstart
        double h = _cvData[index]._pos - _cvData[index - 1]._pos;                                    // xend-xstart
        double y = comp(_cvData[index - 1]._val, channel);                                           // f(xtart)
        double delta = comp(_cvData[index]._val, channel) - comp(_cvData[index - 1]._val, channel);  // f(xend)-f(xtart)
        double d1 = comp(_cvData[index - 1]._deriv, channel);                                        // f'(xtart)
        double d2 = comp(_cvData[index]._deriv, channel);                                            // f'(xend)

        return (x * (delta * (3 * h - 2 * x) * x + h * (-h + x) * (-(d1 * h) + (d1 + d2) * x))) / (h * h * h) + y;
    } break;
    default:
        assert(false);
        return 0;
        break;
    }
}

template <class T>
typename Curve<T>::CV Curve<T>::getLowerBoundCV(const double param) const
{
    assert(prepared);
    const CV* cvDataBegin = &_cvData[0];
    int numPoints = static_cast<int>(_cvData.size());
    int index =
        static_cast<int>(std::upper_bound(cvDataBegin, cvDataBegin + numPoints, CV(param, T(), kLinear), cvLessThan) - cvDataBegin);
    index = std::max(1, std::min(index, numPoints - 1));
    if (index - 1 > 0)
        return _cvData[index - 1];
    return _cvData[index];
}

template <class T>
bool Curve<T>::interpTypeValid(InterpType interp)
{
    return interp == kNone || interp == kLinear || interp == kSmooth || interp == kSpline || interp == kMonotoneSpline;
}

template <>
inline void Curve<double>::clampCurveSegment(const double& delta, double& d1, double& d2)
{
    if (delta == 0)
        d1 = d2 = 0;
    else {
        d1 = SeExpr2::clamp(d1 / delta, 0, 3) * delta;
        d2 = SeExpr2::clamp(d2 / delta, 0, 3) * delta;
    }
}

template <>
void Curve<Vec3d>::clampCurveSegment(const Vec3d& delta, Vec3d& d1, Vec3d& d2)
{
    for (int i = 0; i < 3; i++) {
        if (delta[i] == 0)
            d1[i] = d2[i] = 0;
        else {
            d1[i] = SeExpr2::clamp(d1[i] / delta[i], 0, 3) * delta[i];
            d2[i] = SeExpr2::clamp(d2[i] / delta[i], 0, 3) * delta[i];
        }
    }
}

template class Curve<Vec3d>;
template class Curve<double>;
}
