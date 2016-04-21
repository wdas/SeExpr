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
#ifndef CEGraphCurve_h
#define CEGraphCurve_h

#include <string>
#include <vector>

#include <animlib/AnimCurve.h>

class CETool;
class CEGraphUI;
class CEGraphSeg;
class CEGraphKey;;


class CEGraphCurve
{
public:
    CEGraphCurve(CEGraphUI* ui, animlib::AnimCurve* data, int index);
    ~CEGraphCurve();
    CEGraphUI* ui() { return _ui; }
    animlib::AnimCurve& animCurve() { return *_animCurve; }
    int index() { return _index; }
    void paint(bool selected);
    void invalidate();
    void invalidateView();
    int numSegs();
    void getSegAndKey(int segIndex,CEGraphSeg*& seg,CEGraphKey*& key);
    int firstSegInView() { return _firstSegInView; }
    int lastSegInView() { return _lastSegInView; }

private:
    void clearSegs();
    void buildSegPtrs();
    void buildSeg(int n);
    void updateView();
    CEGraphUI* _ui;
    animlib::AnimCurve* _animCurve;
    //iSgExpr _expr;
    int _index; // curve index within graph ui
    
    bool _segsValid;
    bool _viewValid;

    struct SegPtr {
	SegPtr() : seg(0),key(0) {}
	CEGraphSeg* seg; // segment built lazily in getSeg()
	CEGraphKey* key; // segment built lazily in getSeg()
    };
    std::vector<SegPtr> _segs;
    int _firstSegInView;
    int _lastSegInView;
};
#endif
