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
#include <math.h>
#include <GL/gl.h>
#include "CETool.h"
#include "CEGraphUI.h"
#include "CEDragHandlers.h"
#include "CEGraphCurve.h"
#include "CEGraphSeg.h"
#include "CEGraphKey.h"


CEGraphCurve::CEGraphCurve(CEGraphUI* ui, animlib::AnimCurve* curveData, int index)
    : _ui(ui), _animCurve(curveData),_index(index), _segsValid(0), _viewValid(0),
	  _firstSegInView(0), _lastSegInView(0)
{
}


CEGraphCurve::~CEGraphCurve()
{
    clearSegs();
}


void
CEGraphCurve::clearSegs()
{
    for (int i = 0; i < (int) _segs.size(); i++) {
	delete _segs[i].seg;
	delete _segs[i].key;
    }
    _segs.clear();
    _viewValid = 0;
}


void
CEGraphCurve::invalidate()
{
    if (_segsValid) _segsValid = 0;
}


void
CEGraphCurve::invalidateView()
{
    _viewValid = 0;
}



void
CEGraphCurve::paint(bool selected)
{
    if (!_segsValid) buildSegPtrs();
    if (!_viewValid) updateView();

    glPushName(_index);
    // paint segs, keys, handles
    for (int i = _firstSegInView; i <= _lastSegInView; i++) {
        CEGraphSeg* seg=0;CEGraphKey* key=0;
	getSegAndKey(i,seg,key) ;
        if(seg) seg->paint(selected);
        if(key) key->paint(selected);
    }
    glPopName();
}


int
CEGraphCurve::numSegs()
{
    if (!_segsValid) buildSegPtrs();
    return _segs.size(); 
}


void
CEGraphCurve::getSegAndKey(int n,CEGraphSeg*& seg,CEGraphKey*& key)
{
    if (!_segsValid) buildSegPtrs();
    if (n < 0 || n >= (int) _segs.size()){
        seg=0;
        key=0;
        return;
    };
    seg = _segs[n].seg;
    key = _segs[n].key;
    if(!seg) seg=_segs[n].seg = new CEGraphBezSeg(this,n);
    if(!key) key=_segs[n].key = new CEGraphKey(this,n);
}


void
CEGraphCurve::buildSegPtrs()
{
    clearSegs();

    // build seg info list
    int numSegs=_animCurve->getNumKeys();
    _segs.resize(numSegs);
    _segsValid = 1;
}


void
CEGraphCurve::buildSeg(int n)
{
    if (n < 0 || n > (int) _segs.size()) return;
    if (_segs[n].seg) { delete _segs[n].seg; _segs[n].seg = 0; }

}


void
CEGraphCurve::updateView()
{

    const CEGraphView& view = _ui->getView();
    _firstSegInView=0;
    _lastSegInView=numSegs()-1;
    // TODO: make this as adaptive as before
    //if (!_expr.findSegment(_firstSegInView, view.left(), false).ok()) return;
    //if (!_expr.findSegment(_lastSegInView, view.right()).ok()) return;

    if (_segsValid) {
	for (int i = _firstSegInView; i <= _lastSegInView; i++) {
	    if (auto seg = _segs[i].seg)
                seg->invalidateView();
	}
    }
    _viewValid = 1;
}










