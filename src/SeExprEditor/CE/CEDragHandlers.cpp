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
//#include <DGStrUtils.h>
#include "CETool.h"
#include "CEGraphUI.h"
#include "CEGraphCurve.h"
#include "CEGraphSeg.h"
#include "CEGraphKey.h"
#include "CEDragHandlers.h"


static double RoundNicely(double val, double inc)
{
    // first make inc nice (i.e. [1, 2, or 5] * some power of 10)
    double nice = pow(10, floor(log10(inc)));
    if (nice < inc)
    {
        nice *= 2;
        if (nice < inc)
        {
            nice *= 2.5;
            if (nice < inc) 
            {
                nice *= 2;
            }
        }
    }

    // round val to nearest multiple of inc
    return floor(val/nice+.5)*nice;
}


void CEDragHandler::setAnchor(CEGraphUI* ui, int x, int y)
{
    _ui = ui;
    _anchorX = _x = x;
    _anchorY = _y = y;
    _view = _ui->getView();
    _dx = _dy = 0;
}


void CEDragHandler::movePoint(int x, int y)
{
    _x = x;
    _y = y;
    _dx = _x - _anchorX;
    _dy = _y - _anchorY;
    _moved = true;
}



void CEPanHandler::pan(bool useCommand)
{
    double x = _view.vx() - _dx*_view.pixelWidth();
    double y = _view.vy() - _dy*_view.pixelHeight();
    double w = _view.vw(), h = _view.vh();

    _ui->tool()->setView(x, y, w, h);
}


void CEZoomHandler::zoom(bool useCommand)
{
    double sx = 1, sy = 1;
    if (_zoomX) {
        double dx = _dx;
        sx = dx < 0 ? (1 - dx/100.0) : 1 / (1 + dx/100.0);
    }
    
    if (_zoomY) {
        double dy = _dy;
        sy = dy < 0 ? (1 - dy/100.0) : 1 / (1 + dy/100.0);
    }
    
    double x = _view.vx(), y = _view.vy();
    double w = _view.vw()*sx, h = _view.vh()*sy;
    
    _ui->tool()->setView(x, y, w, h);
}


void CESelBoxHandler::mouseMove()
{
    _ui->moveSelBox(_anchorX, _anchorY, _x, _y);

    // do a select-mode render to get the pending segments
    static const int buffsize = 10000;
    GLuint selbuff[buffsize];
    int numHits = _ui->doSelectModeRender(_anchorX, _x, _anchorY, _y,
					  selbuff, buffsize);
    std::vector<Seg> newsegs;
    GLuint* p = selbuff;
    for (int i = 0; i < numHits; i++)
    {
        GLuint numNames = p[0]; p+=3;
        if (numNames >= 3) {
            if (p[2] == CEGraphSeg::Key) {
                newsegs.push_back(Seg(p[0], p[1]));
            }
        }
        p += numNames;
    }

    if (_segs != newsegs) 
    {
        SegIterator i;
        for (i = _segs.begin(); i != _segs.end(); i++) {
            CEGraphSeg* seg = _ui->getSeg(i->curveIndex, i->segIndex);
            if (seg) seg->setPendingSel(0);
        }

        _segs = newsegs;
        for (i = _segs.begin(); i != _segs.end(); i++) {
            CEGraphSeg* seg = _ui->getSeg(i->curveIndex, i->segIndex);
            if (seg) seg->setPendingSel(1);
        }

        _ui->update();
    }
}


void CESelBoxHandler::mouseUp()
{
    _ui->hideSelBox();
    //iCurveEditor& ce = _ui->ce();
    CETool* tool=_ui->tool();
    SegIterator i;
    for (i = _segs.begin(); i != _segs.end(); i++)
    {
        CEGraphSeg* seg = _ui->getSeg(i->curveIndex, i->segIndex);

        if (seg && seg->isPendingSel()) 
        {
            seg->setPendingSel(0);
            
            bool selected = 0;
            if (_toggle)
                tool->isSegmentSelected(selected, i->curveIndex, i->segIndex);
            if (selected) 
                tool->deselectSegment(i->curveIndex, i->segIndex);
            else 
                tool->selectAddSegment(i->curveIndex, i->segIndex);
        }
    }
    
}


CEKeyHandler::CEKeyHandler(CEGraphKey* key)
{
    _animCurve = &key->curve()->animCurve();
    _curve = key->curve()->index();
    _seg = key->index();
}


CEKeyMoveHandler::CEKeyMoveHandler(CEGraphKey* key, bool dragTime, bool dragValue,
			   double hSnap, double vSnap)
    : CEKeyHandler(key), _dragTime(dragTime), _dragValue(dragValue),
      _hSnap(hSnap), _vSnap(vSnap)
{
    _anchorTime = key->key().getTime();
    _anchorValue = key->key().getValue();
}



void CEKeyMoveHandler::setMultiDrag(std::vector<CEGraphKey*> graph_segments)
{
    for (unsigned int i = 0; i < graph_segments.size(); i++)
    {
        CEGraphKey* segment = graph_segments[i];
        _anchorTimes.push_back(segment->key().getTime());
        _anchorValues.push_back(segment->key().getValue());
        _curves.push_back(segment->curve()->index());
        _segs.push_back(segment->index());
    }     
}


void CEKeyMoveHandler::moveKey(bool useCommand)
{
    
    if (_anchorTimes.size() >0)
    {
        //We are doing Multi-Drag!
        for (unsigned int i = 0; i < _anchorTimes.size(); i++)
        {
            if (_dragTime) 
            {
                double newtime = _anchorTimes[i] + _dx*_view.pixelWidth();
                double snap = _view.pixelWidth();
                if (snap < _hSnap) snap = _hSnap;
                newtime = RoundNicely(newtime, snap);
                CEGraphSeg* prevseg = _ui->getSeg(_curves[i], _segs[i]-1);
                CEGraphSeg* nextseg = _ui->getSeg(_curves[i], _segs[i]+1);
                if (prevseg && !prevseg->firstSeg() && newtime < prevseg->keyTime())
                    newtime = prevseg->keyTime();
                if (nextseg && newtime > nextseg->keyTime())
                    newtime = nextseg->keyTime();
                _ui->tool()->setSegmentFrame(newtime, _curves[i], _segs[i]);
            }
            if (_dragValue) 
            {
                double newval = _anchorValues[i] + _dy*_view.pixelHeight();
                double snap = _view.pixelHeight();
                if (snap < _vSnap) snap = _vSnap;
                newval = RoundNicely(newval, snap);
                
                _ui->tool()->setSegmentValue(newval, _curves[i], _segs[i]);
            }
        }     
    }
    else
    {
        //Only dragging one key
        if (_dragTime) 
        {
            double newtime = _anchorTime + _dx*_view.pixelWidth();
            double snap = _view.pixelWidth();
            if (snap < _hSnap) snap = _hSnap;
            newtime = RoundNicely(newtime, snap);
            CEGraphSeg* prevseg = _ui->getSeg(_curve, _seg-1);
            CEGraphSeg* nextseg = _ui->getSeg(_curve, _seg+1);
            if (prevseg && !prevseg->firstSeg() && newtime < prevseg->keyTime())
                newtime = prevseg->keyTime();
            if (nextseg && newtime > nextseg->keyTime())
                newtime = nextseg->keyTime();
            _ui->tool()->setSelectedSegmentFrame(newtime);
        }
        if (_dragValue) 
        {
            double newval = _anchorValue + _dy*_view.pixelHeight();
            double snap = _view.pixelHeight();
            if (snap < _vSnap) snap = _vSnap;
            newval = RoundNicely(newval, snap);
            _ui->tool()->setSelectedSegmentValue(newval);
        }
    }
}


void CENewKeyHandler::mouseDown()
{
    //_dragTime = _dragValue = 1;
    double snap = _view.pixelWidth();
    if (snap < 0) snap = 0;
    double _anchorTime = RoundNicely(_view.vx(_x), snap);
    double _anchorValue = RoundNicely(_view.vy(_y), _view.pixelHeight());
    int index=0;
    _ui->tool()->insertKey(_anchorTime);
}


 CEBezHandler::CEBezHandler(CEGraphKey* key, int handle, 
     bool dragAngle, bool dragLength,bool weighted)
     : CEKeyHandler(key), _handle(handle), 
      _dragAngle(dragAngle), _dragLength(dragLength), _weighted(weighted)
 {
     // lookup key parametersweights and angles
     _originalWeightIn=key->key().getInWeight();
     _originalWeightOut=key->key().getOutWeight();
     if(!_weighted) _originalWeightIn=_originalWeightOut=3;
     _time=key->key().getTime();
     _value=key->key().getValue();
 }

 static bool ClampBezHandle(double& x1, double x2, double seglen)
 {
     /* Clamp handle a against handle b:
        For a normalized segment (seglen = 1), two handle are at their max
        length when the following relationship is true:

           a = 1 - 0.5 * (b - sqrt(b * (4 - 3 * b)))

        This equation was derived by solving for coincident real roots
        of the deriviate of the bezier equation. */

     // normalize ref handle to 0..1
     double b = x2 / seglen;

     // make sure b is legal (i.e. within 0..4/3)
     if (b < 0) b = 0;
     else if (b > 4/3.) b = 4/3.;

     // compute max length for input handle for given ref handle
     double a = 1 - 0.5 * (b - sqrt(b * (4 - 3 * b)));

     // scale handle back to seglen (un-normalize)
     a *= seglen; 

     // clamp input handle (if needed)
     if (x1 > a) {
         x1 = a;
         return true; // clamped
     }
     return false; // not clamped
 }

void CEBezHandler::mouseDown(){
    // remember a reference point in view coordinates where the user started interacting
    double scale=_handle==0 ? -1. : 1.; 
    _refX=scale*(_view.vx(_x)-_time);
    _refY=scale*(_view.vy(_y)-_value);
}

 void CEBezHandler::dragHandle(bool useCommand)
 {
     CEGraphKey* key = _ui->getKey(_curve, _seg);
     if(_handle != 0 && _handle != 1) return;

     double pw = _view.pixelWidth();
     double ph =  _view.pixelHeight();
     double dx = _dx * pw;
     double dy = _dy * ph;
     if(_handle == 0){dx=-dx;dy=-dy;}
     //std::cerr<<"we have _dx "<<_dx<<" "<<_dy<<" dx "<<dx<<" "<<dy<<std::endl;

     // compute new offset nx,ny in view coordinates
     double ax = _refX;
     double ay = _refY;
     double nx = ax + dx;
     double ny = ay + dy;

     // TODO: find max horizontal length
     //double seglen = seg->outTime() - seg->keyTime();

     // the new weight is the length of the new offset
     double nweight=sqrt(ny*ny+nx*nx);

     // figure out angles and clamp to be vertical
     double na = atan2(ny, nx);
     if(na > M_PI_2) na = M_PI_2;
     if(na < -M_PI_2) na = -M_PI_2;

     // TODO: clamp handle in non-weighted case
     double inWeight=nweight;
     double outWeight=nweight;
     if(!_dragLength){
         inWeight=_originalWeightIn;
         outWeight=_originalWeightOut;
     }
     if(!key->weighted()){
         double prevTime=0,nextTime=0;
         if(CEGraphKey* prevKey=_ui->getKey(_curve,_seg-1)) 
             prevTime=prevKey->key().getTime();
         if(CEGraphKey* nextKey=_ui->getKey(_curve,_seg+1))
             nextTime=nextKey->key().getTime();
         double currTime=key->key().getTime();
         // cos(na) = xlength / weight
         // we want xlength = (timeInterval) / 3
         // so weight = xlength / cos(na)
         outWeight=((nextTime-currTime)/3.)/cos(na);
         inWeight=((currTime-prevTime)/3.)/cos(na);
     }
     // if we are weighted keep and we are locked we want old weight for 
     // the matched guy
     if(key->weighted()){
         if(_handle==0) outWeight=_originalWeightOut;
         if(_handle==1) inWeight=_originalWeightIn;
     }
     // set if we are locked or if it is our handle
     if(_handle==0 || key->key().isLocked())
         _ui->tool()->setKeyIn(_curve,_seg,na*180./M_PI,inWeight);
     if(_handle==1 || key->key().isLocked()) 
         _ui->tool()->setKeyOut(_curve,_seg,na*180./M_PI,outWeight);

 }
