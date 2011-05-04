/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#ifndef _CurveData_h_
#define _CurveData_h_

#include "SeVec3d.h"
#include <vector>

#include <cfloat>
#include <cassert>

namespace SeExpr{

//! Interpolation curve class for double->double and double->SeVec3D
/**   Interpolation curve class for mapping from double -> double or double -> SeVec3D
      Subject to some interpolation points.
      
      Each segment is interpolated according to the interpolation type specified on the
      left control point. Interpolation types supported are members of InterpType
      below.
      
      Valid instantiation types for this are double, or SeVec3D
*/
template <class T>
class SeCurve
{
    mutable int cacheCV;
public:
    //! Supported interpolation types
    enum InterpType { kNone=0, kLinear, kSmooth, kSpline, kMonotoneSpline };
    struct CV{
        CV(double pos,const T& val,InterpType type)
            :_pos(pos),_val(val),_interp(type)
        {}

        double _pos;
        T _val,_deriv;
        InterpType _interp;
    };
private:
    std::vector<CV> _cvData;
    bool prepared;
public:
    SeCurve();

    //! Adds a point to the curve
    void addPoint(double position,const T& val,InterpType type);

    //! Prepares points for evaluation (sorts and computes boundaries, clamps extrema)
    void preparePoints();

    //! Evaluates curve and returns full value
    T getValue(const double param) const;

    //! Evaluates curve for a sub-component of the interpolation values
    //! must call preparePoints() before this is ok to call
    double getChannelValue(const double param,int channel) const;

    //! Returns the control point that is less than the parameter, unless there is no
    //! point, in which case it returns the right point or nothing 
    CV getLowerBoundCV(const double param) const;

    //! Returns whether the given interpolation type is supported
    static bool interpTypeValid(InterpType interp);

    //! CV Parameter ordering (cv1._pos < cv2._pos)
    static bool cvLessThan(const CV &cv1, const CV &cv2);
private:
    //! Performs hermite derivative clamping in canonical space
    void clampCurveSegment(const T& delta,T& d1,T& d2);

    //! Returns a component of the given value
    static double comp(const T& val,const int i);
};

}
#endif
