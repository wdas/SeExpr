/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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
