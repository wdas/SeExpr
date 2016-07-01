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

#ifndef ExprBuiltins_h
#define ExprBuiltins_h

#include "ExprFunc.h"
#include "Platform.h"

namespace SeExpr2 {

void initPerlin();

// trig
inline double deg(double angle) { return angle * (180 / M_PI); }
inline double rad(double angle) { return angle * (M_PI / 180); }
inline double cosd(double x) { return cos(rad(x)); }
inline double sind(double x) { return sin(rad(x)); }
inline double tand(double x) { return tan(rad(x)); }
inline double acosd(double x) { return deg(acos(x)); }
inline double asind(double x) { return deg(asin(x)); }
inline double atand(double x) { return deg(atan(x)); }
inline double atan2d(double y, double x) { return deg(atan2(y, x)); }

// clamping
inline double clamp(double x, double lo, double hi) { return x < lo ? lo : x > hi ? hi : x; }
inline double round(double x) { return x < 0 ? ceil(x - 0.5) : floor(x + 0.5); }
inline double max(double x, double y) { return x > y ? x : y; }
inline double min(double x, double y) { return x < y ? x : y; }

// blending / remapping
inline double invert(double x) { return 1 - x; }
double compress(double x, double lo, double hi);
double expand(double x, double lo, double hi);
double fit(double x, double a1, double b1, double a2, double b2);
double gamma(double x, double g);
double bias(double x, double b);
double contrast(double x, double c);
double boxstep(double x, double a);
double linearstep(double x, double a, double b);
double smoothstep(double x, double a, double b);
double gaussstep(double x, double a, double b);
double remap(double x, double s, double r, double f, double interp);
double mix(double x, double y, double alpha);
Vec3d hsi(int n, const Vec3d* args);
Vec3d midhsi(int n, const Vec3d* args);
Vec3d rgbtohsl(const Vec3d& rgb);
Vec3d hsltorgb(const Vec3d& hsl);

// noise
double hash(int n, double* args);
double noise(int n, const Vec3d* args);
double snoise(const Vec3d& p);
Vec3d cnoise(const Vec3d& p);
Vec3d vnoise(const Vec3d& p);
double turbulence(int n, const Vec3d* args);
Vec3d vturbulence(int n, const Vec3d* args);
Vec3d cturbulence(int n, const Vec3d* args);
double fbm(int n, const Vec3d* args);
Vec3d vfbm(int n, const Vec3d* args);
Vec3d cfbm(int n, const Vec3d* args);
double cellnoise(const Vec3d& p);
Vec3d ccellnoise(const Vec3d& p);
double pnoise(const Vec3d& p, const Vec3d& period);

// vectors
double dist(double ax, double ay, double az, double bx, double by, double bz);
double length(const Vec3d& v);
double hypot(double x, double y);
double dot(const Vec3d& a, const Vec3d& b);
Vec3d norm(const Vec3d& a);
Vec3d cross(const Vec3d& a, const Vec3d& b);
double angle(const Vec3d& a, const Vec3d& b);
Vec3d ortho(const Vec3d& a, const Vec3d& b);
Vec3d up(const Vec3d& vec, const Vec3d& upvec);

// variations
double cycle(double index, double loRange, double hiRange);
double pick(int n, double* params);
double choose(int n, double* params);
double wchoose(int n, double* params);
double spline(int n, double* params);

// add builtins to expression function table
void defineBuiltins(ExprFunc::Define define, ExprFunc::Define3 define3);
}

#endif
