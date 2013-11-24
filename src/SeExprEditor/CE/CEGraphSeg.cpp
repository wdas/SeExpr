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


static double HandleMin = 20;



CEGraphSeg::CEGraphSeg(CEGraphCurve* curve,int index)
    : _viewValid(0), _curve(curve), _index(index), _unbounded(0),
      _numParams(0), _params(0),
      _keytime(0), _keyval(0), _outtime(0), _outval(0), _nextval(0),
      _selected(0), _pendingSel(0)
{
    auto it=curve->animCurve().getFirstKey()+index;
    _keytime=it->getTime();
    _keyval=it->getValue();
    if(index<int(curve->animCurve().getNumKeys())-1){
        auto itnext=curve->animCurve().getFirstKey()+index+1;
        _outval=itnext->getValue();
        _outtime=itnext->getTime();
    }else{
        _unbounded=true;

        _outval=_keyval;
        _outtime=_keytime;
    }
    
//    std::cerr<<"we have "<<_keytime<<" "<<_keyval<<" out "<<_outtime<<" "<<_outval<<" tang "<<animlib::AnimKeyframe::tangentTypeToString(it->getInTangentType())<<"  outang "<<it->getOutAngle()<<std::endl;
}


CEGraphSeg::~CEGraphSeg()
{
    delete [] _params;
}


void
CEGraphSeg::paint(bool selected)
{
    selected=false;
    CEGraphUI* ui = _curve->ui();
    if (!selected) {
	// determine if we're selected on the first (non-selection) pass
	_selected = 0;
    }

    const CEGraphView& view = ui->getView();

    // paint seg
    glPushName(_index);

    if(_selected == selected){
        if (beginPart(Seg)) {
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            paintSeg(); // defined in subclasses
            glDisable(GL_BLEND);
            glDisable(GL_LINE_SMOOTH);

            // paint discontinuity between segs (if present)
            if (!_unbounded && _outval != _nextval) {
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(2, 0x5555);
                glBegin(GL_LINES);
                int ot = view.px(_outtime);
                glVertex2i(ot, view.py(_outval));
                glVertex2i(ot, view.py(_nextval));
                glEnd();
                glDisable(GL_LINE_STIPPLE);
            }
            endPart();
        }
    }

    glPopName();

}


bool
CEGraphSeg::beginPart(Part part)
{
    glPushName(part);
    CEGraphUI* ui = _curve->ui();
    int activeCurve, activeSeg, activePart;
    ui->getActivePart(activeCurve, activeSeg, activePart);
    bool active = (part == activePart && _index == activeSeg 
		   && _curve->index() == activeCurve);
    bool selected = _selected || (part == Key && _pendingSel);
    ui->setColor(selected ? 
		 (active ? CEGraphUI::HiSelectedColor : 
		  CEGraphUI::SelectedColor) :
		 (active ? CEGraphUI::HiCurveColor : 
		  CEGraphUI::CurveColor));
    return 1;
}


void
CEGraphSeg::endPart()
{
    glPopName();
}


void
CEGraphSeg::paintKey()
{
    const CEGraphView& view = _curve->ui()->getView();

    int kt = view.px(_keytime);
    int kv = view.py(_keyval);

    // draw line for dragging time
    if (beginPart(KeyTime)) {
	glBegin(GL_LINES);
	glVertex2i(kt, kv-15);
	glVertex2i(kt, kv+16);
	glEnd();
	endPart();
    }
    // draw key rectangle
    if (beginPart(Key)) {
        glRecti(kt-3, kv-3, kt+4, kv+4);
	endPart();
    }
}


CEDragHandler*
CEGraphSeg::getDragHandler(int part, int keystate)
{
    bool alt = keystate & Qt::AltModifier;
    switch (part) {
    case Seg:
	{
	    return alt ? new CENewKeyHandler(this) : 0;
	}
    }
    return 0;
}


void
CEGraphSeg::paintHandle(int num, double x, double y, double dx, double dy,
			double zx, double zy, bool joinKnob)
{
    const CEGraphView& view = _curve->ui()->getView();

    // draw handle
    double x1p = view.pxd(x), y1p = view.pyd(y);
    double x2p = view.pxd(x+dx), y2p = view.pyd(y+dy);
    beginPart(Part(HandleBase + num));
    glBegin(GL_LINES);
    glVertex2d(x1p, y1p);
    glVertex2d(x2p, y2p);
    glEnd();

    // draw handle extender (if needed)
    double x3p=x2p, y3p=y2p;
    bool extend = 0;

    if (dx == 0 && dy == 0) {
	// handle length is 0, use zx, zy to determine extend direction
	double scale = HandleMin / sqrt(zx*zx + zy*zy);
	x3p = x1p + zx * scale;
	y3p = y1p + zy * scale;
	extend = 1;
    } else {
	// find handle length
	double dxp = x2p-x1p;
	double dyp = y2p-y1p;
	double len = sqrt(dxp*dxp + dyp*dyp);
	if (len < HandleMin) {
	    // scale handle up to minimum length
	    double scale = HandleMin / len;
	    x3p = x1p + dxp * scale;
	    y3p = y1p + dyp * scale;
	    extend = 1;
	}
    }

    if (extend) {
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0x5555);
	glBegin(GL_LINES);
	glVertex2d(x2p, y2p);
	glVertex2d(x3p, y3p);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
    }

    if (!joinKnob) {
	endPart();
	beginPart(Part(KnobBase + num));
    }

    // draw knob
    glRecti(int(x3p-2), int(y3p-2), int(x3p+3), int(y3p+3));
    endPart();
}


CEGraphUniformSeg::CEGraphUniformSeg(CEGraphCurve* curve,int index)
    : CEGraphSeg(curve,index), _samples(0)
{
}


CEGraphUniformSeg::~CEGraphUniformSeg()
{
    delete [] _samples;
}


void
CEGraphUniformSeg::paintSeg()
{
    if (!_viewValid) updateView();
    const CEGraphView& view = _curve->ui()->getView();
    if(_numSamples==0) return;
    glPushAttrib(GL_CURRENT_BIT|GL_LINE_BIT);
    glLineWidth(2.);
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, &color[0]);

    if(_unbounded || firstSeg()) glColor3f(.6f,.6f,.6f);
    bool transitionedToRealSegment=false;
    // draw curve samples
    int sample=0;
    if(firstSeg()){
        glBegin(GL_LINE_STRIP);
        while(sample<_numSamples){
            float x=_sampleStart + _sampleSpacing * sample;
            glVertex2d(view.pxd(x),
                view.pyd(_samples[sample]));
            sample++;
            if(x>=_keytime) break;
        }
        glEnd();
        glColor3fv(&color[0]);
    }

    glBegin(GL_LINE_STRIP);
    glVertex2d(view.pxd(_keytime), view.pyd(_keyval));
    for (; sample < _numSamples; sample++){
        float x=_sampleStart + _sampleSpacing * sample;
        glVertex2d(view.pxd(x),
            view.pyd(_samples[sample]));
    }
    if (!_unbounded) glVertex2d(view.pxd(_outtime), view.pyd(_outval));
    glEnd();
    glPopAttrib();
    
}


void
CEGraphUniformSeg::updateView()
{
    // need to update samples based on view
    if (_samples) {
	// free previous samples
	delete [] _samples;
	_samples = 0;
    }

    // determine view range
    const CEGraphView& view = _curve->ui()->getView();
    double l = view.left();
    double r = view.right();

    // aim for a sample every n pixels
    double s = view.pixelWidth()*2;
    // increase to an even power of 2 for smooth zooms
    _sampleSpacing = s;//pow(2,ceil(log(s)*(1/M_LN2)));

    // find the leftmost sample
    int start = int(floor(l/_sampleSpacing));
    if (!firstSeg()) {
	double keysamp = _keytime/_sampleSpacing;
	if (start < keysamp) start = int(floor(keysamp)+1);
    }
    _sampleStart = start * _sampleSpacing;

    // find the rightmost sample
    int end = int(ceil(r/_sampleSpacing));
    if (!_unbounded) {
	double keysamp = _outtime/_sampleSpacing;
	if (end > keysamp) end = int(ceil(keysamp)-1);
    }
    _sampleEnd = end * _sampleSpacing;

    // compute the number of samples
    _numSamples = end-start+1;
    if (_numSamples <= 0) {
	_numSamples = 0;
    }else {
	// sample the curve
        std::vector<double> samples;
        double dx=(_sampleEnd-_sampleStart)/(_numSamples);
        double x=_sampleStart;
        for(int i=0;i<_numSamples;i++){
            x+=dx;
            samples.push_back(x);
        }
        std::vector<double> vals=_curve->animCurve().getValues(samples);

	_samples = new double[_numSamples];
	for (int i = 0; i < _numSamples; i++){
	    _samples[i] = vals[i];
            //std::cerr<<"eval at "<<samples[i]<<"->"<<vals[i]<<std::endl;
        }
    }
    _viewValid = 1;
}	

CEGraphBezSeg::CEGraphBezSeg(CEGraphCurve* curve,int index)
    : CEGraphUniformSeg(curve,index)
{
    double dxi = 0;
    double dyi = 0;
    double dxo = 0;
    double dyo = 0;

    _numParams = 4;
    _params = new double[4];
    auto it=curve->animCurve().getFirstKey()+index;
    double thetaIn=M_PI/180.*it->getOutAngle();
    double thetaOut=0;
    double weightIn=it->getOutWeight();
    double weightOut=1;

    if(index < int(curve->animCurve().getNumKeys())-1){
        thetaOut=M_PI/180.*(it+1)->getInAngle();
        weightOut=(it+1)->getInWeight();
    }
    
    _params[0] = weightIn*cos(thetaIn);
    _params[1] = weightIn*sin(thetaIn);
    _params[2] = weightOut*cos(thetaOut);
    _params[3] = weightOut*sin(thetaOut);
}


void CEGraphBezSeg::paintHandles()
{}


CEDragHandler* 
CEGraphBezSeg::getSegmentHandler(int part, int keystate)
{
    return NULL;
}
