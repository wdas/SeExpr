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
#include "CEGraphKey.h"
#include "CEGraphCurve.h"
#include "CEGraphView.h"
#include "CEGraphUI.h"
#include "CETool.h"
#include "CEDragHandlers.h"

CEGraphKey::CEGraphKey(CEGraphCurve* curve,int index)
    :_selected(false),_curve(curve),_index(index),_key(*(curve->animCurve().getFirstKey()+index)),_weighted(curve->animCurve().isWeighted())
{
}

bool CEGraphKey::beginPart(Part part)
{
    glPushName(part);
    CEGraphUI* ui = _curve->ui();
    int activeCurve, activeSeg, activePart;
    ui->getActivePart(activeCurve, activeSeg, activePart);
    bool active = (part == activePart && _index == activeSeg 
		   && _curve->index() == activeCurve);
    bool selected = _selected;
    ui->setColor(selected ? 
		 (active ? CEGraphUI::HiSelectedColor : 
		  CEGraphUI::SelectedColor) :
		 (active ? CEGraphUI::HiCurveColor : 
		  CEGraphUI::CurveColor));
    return 1;
}

void CEGraphKey::endPart()
{
    glPopName();
}


void CEGraphKey::paint(bool selected)
{

    glPushName(_index);

    CEGraphUI* ui = _curve->ui();
    ui->tool()->isSegmentSelected(_selected, _curve->index(), _index).suppress();
    const CEGraphView& view = ui->getView();

    // key time and value in screen coordinates
    double kt=view.px(_key.getTime());
    double kv=view.py(_key.getValue());
    
    // draw time dragger
    if(beginPart(KeyTime)){
        glBegin(GL_LINES);
        glVertex2i(kt,kv-15);
        glVertex2i(kt,kv+15);
        glEnd();
        endPart();
    }
    // draw node of time/value
    if(beginPart(Key)){
        glRecti(kt-3,kv-3,kt+4,kv+4);
        endPart();
    }

    // draw the handles
//    if(_selected)
        paintHandles();

    glPopName();
}



void CEGraphKey::paintHandles()
{
    double inWeight=_key.getInWeight(),outWeight=_key.getOutWeight();
    if(!_weighted) inWeight=outWeight=3;
    double inAngle=M_PI*_key.getInAngle()/180.,outAngle=M_PI*_key.getOutAngle()/180.;
    double time=_key.getTime(),value=_key.getValue();
    paintHandle(0,time,value,-inWeight*cos(inAngle),-inWeight*sin(inAngle),_key.getInTangentType() == animlib::AnimKeyframe::kTangentFixed);
    paintHandle(1,time,value,outWeight*cos(outAngle),outWeight*sin(outAngle),_key.getOutTangentType() == animlib::AnimKeyframe::kTangentFixed);
}

namespace{
    double sqr(double x){return x*x;}
}
void CEGraphKey::paintHandle(int num,double x,double y,double dx,double dy,bool fixedHandle)
{
    const CEGraphView& view = _curve->ui()->getView();
    double x1p=view.pxd(x),y1p=view.pyd(y);
    double x2p=view.pxd(x+dx),y2p=view.pyd(y+dy);
    double normFactor=1./sqrt(sqr(x2p-x1p)+sqr(y2p-y1p)); // TODO: this is expensive
    double normx=(x2p-x1p)*normFactor,normy=(y2p-y1p)*normFactor;
    double offx=-normy*4,offy=normx*4;
    if(!_weighted){
        x2p=x1p+normx*20;y2p=y1p+normy*20;
    }
    double backx=.8*x2p+.2*x1p;
    double backy=.8*y2p+.2*y1p;
    _handleAnchorX[num]=view.vx(x2p)-x;
    _handleAnchorY[num]=view.vy(y2p)-y;

    
    glPushAttrib(GL_CURRENT_BIT);
    if(!fixedHandle) glColor3f(1,1,1);
    if(fixedHandle) beginPart(Part(HandleBase+num));
    glBegin(GL_LINES);
    glVertex2d(x1p,y1p);
    glVertex2d(x2p,y2p);
    glEnd();

    if(fixedHandle) endPart();
    if(fixedHandle) beginPart(Part(KnobBase+num));
    if(fixedHandle) glRecti(int(x2p-2), int(y2p-2), int(x2p+3), int(y2p+3));
    else{
        glBegin(GL_TRIANGLES);
        glVertex2d(x2p,y2p);
        glVertex2d(backx+offx,backy+offy);
        glVertex2d(backx-offx,backy-offy);
        glEnd();
    }
    if(fixedHandle) endPart();
    glPopAttrib();
}

CEGraphKey::~CEGraphKey()
{
}

CEDragHandler* CEGraphKey::getDragHandler(int part,int keystate)
{
    bool alt = keystate & Qt::AltModifier;
    switch (part) {
    case NoPart: 
	{
	    return 0;
	}
    case KeyTime: 
	{
	    double hsnap = alt ? 0.0 : 1.0;
	    return new CEKeyMoveHandler(this, 1, 0, hsnap);
	}
    case Key:
	{
	    double hsnap = alt ? 0.0 : 1.0;
	    return new CEKeyMoveHandler(this, alt, 1, hsnap);
	}
        // new CEBezHandler(key,handle,dragAngle,dragLength)
        case HandleBase+0: return new CEBezHandler(this,0,1,0,_weighted);
        case HandleBase+1: return new CEBezHandler(this,1,1,0,_weighted);
        case KnobBase+0: return new CEBezHandler(this,0,alt,1,_weighted);
        case KnobBase+1: return new CEBezHandler(this,1,alt,1,_weighted);
    }
    return 0;
}












