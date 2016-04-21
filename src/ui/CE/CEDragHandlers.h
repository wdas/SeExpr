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
#ifndef CEDragHandlers_h
#define CEDragHandlers_h

#include <vector>
//#include <iSgExpr.h>
#include "CEGraphView.h"
#include <animlib/AnimCurve.h>

class CEGraphUI;
class CETool;
class CEGraphSeg;

class CEDragHandler
{
public:
    CEDragHandler() : _moved(false), _anchorX(0), _anchorY(0), _x(0), _y(0),
		      _dx(0), _dy(0), _ui(0) {}
    virtual ~CEDragHandler() {}

    virtual void mouseDown() {}
    virtual void mouseMove() {}
    virtual void mouseUp() {}
    virtual bool showCrossHairs() { return 0; }
    virtual void setMultiDrag(std::vector<CEGraphKey*> graph_segments) {}
    
    void setAnchor(CEGraphUI* ui, int anchorX, int anchorY);
    void movePoint(int x, int y);

protected:
    bool _moved;
    int _anchorX, _anchorY;	// mouse down x,y pos
    int _x, _y;			// current x,y pos
    int _dx, _dy;		// delta x,y (current-anchor) for convenience
    CEGraphView _view;		// view at mouse down
    CEGraphUI* _ui;		// graph ui
};


class CEPanHandler : public CEDragHandler
{
public:
    virtual void mouseMove() { pan(false); }
    virtual void mouseUp() { if (_moved) pan(true); }
private:
    void pan(bool useCommand);
};


class CEZoomHandler : public CEDragHandler
{
public:
    CEZoomHandler(bool x, bool y) : _zoomX(x), _zoomY(y) {}
    virtual void mouseMove() { zoom(false); }
    virtual void mouseUp() { if (_moved) zoom(true); }
private:
    void zoom(bool useCommand);
    bool _zoomX;		// flag enabling x zooming
    bool _zoomY;		// flag enabling y zooming
};


class CESelBoxHandler : public CEDragHandler
{
public:
    CESelBoxHandler(bool toggle) : _toggle(toggle) {}
    virtual void mouseMove();
    virtual void mouseUp();
private:
    bool _toggle;
    struct Seg { 
        Seg(int curveIndex, int segIndex)
            : curveIndex(curveIndex), segIndex(segIndex) {}
        bool operator==(const Seg& seg) const
           { return curveIndex==seg.curveIndex && segIndex==seg.segIndex; }
        bool operator!=(const Seg& seg) const
           { return curveIndex!=seg.curveIndex || segIndex!=seg.segIndex; }
        int curveIndex;
        int segIndex;
    };
    typedef std::vector<Seg>::iterator SegIterator;
    std::vector<Seg> _segs;
};


class CEKeyHandler : public CEDragHandler
{
public:
    CEKeyHandler(CEGraphKey* seg);
    virtual bool showCrossHairs() { return 1; }

protected:
    animlib::AnimCurve* _animCurve;
    int _curve;			// curve index
    int _seg;			// segment index

};


class CEKeyMoveHandler : public CEKeyHandler
{
public:
    CEKeyMoveHandler(CEGraphKey* key, bool dragTime, bool dragValue,
		 double hSnap=0, double vSnap=0);
    virtual void mouseMove() { moveKey(false); }
    virtual void mouseUp() { if (_moved) moveKey(true); }
    virtual void setMultiDrag(std::vector<CEGraphKey*> graph_segments);
protected:
    void moveKey(bool useCommand);
    bool _dragTime;
    bool _dragValue;
    double _anchorTime;
    double _anchorValue;
    std::vector<double> _anchorTimes;    //Mutiple segments only
    std::vector<double> _anchorValues;   //Mutiple segments only
    std::vector<int> _curves;      //Mutiple segments only
    std::vector<int> _segs;        //Mutiple segments only
    double _hSnap;
    double _vSnap;
};


class CENewKeyHandler :public CEDragHandler
{
public:
    CENewKeyHandler(CEGraphSeg* key)
        :_key(key)
    {}
    virtual void mouseDown();

private:
    CEGraphSeg* _key;
};


class CEBezHandler : public CEKeyHandler
{
public:
    CEBezHandler(CEGraphKey* key, int handle, bool dragAngle, bool dragLength, bool weighted);
    virtual void mouseMove() { dragHandle(false); }
    virtual void mouseUp() { if (_moved) dragHandle(true); }
    virtual void mouseDown();
    void dragHandle(bool useCommand);

protected:
    int _handle;
    bool _dragAngle, _dragLength;
    bool _weighted;
    double _refX, _refY; // reference point where we started dragging from
    double _originalWeightIn,_originalWeightOut; // original weights
    double _time,_value;
};
#endif
