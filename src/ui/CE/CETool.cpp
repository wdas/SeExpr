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
*
* @file CETool.C
* @brief Contains the Definition of class CETool.
*/

#include <sstream>
#include <math.h>
#include <limits>
#include <alloca.h>
#include <string.h>
#include <iFactory.h>
#include "CEMainUI.h"
#include "CETool.h"

namespace {
    inline double clamp(double x, double min, double max) {
        return x < min ? min : x > max ? max : x;
    }
}

void keyUpdated(animlib::AnimCurve* curve,int key);
void keysUpdated(animlib::AnimCurve* curve,int keyStart,int keyEnd,bool regenerateWeights);

/**
 * Constructor
 */
CETool::CETool()
    :  _ui(0), _vx(50), _vy(50), _vw(100), _vh(100),
      _timeMode(0), _selectedCurve(-1), _selListValid(0)
{
//    _rcvrid.addIRcvr(static_cast<iCurveEditorRcvr*>(this));
//    _rcvrid.addIRcvr(static_cast<iObserverRcvr*>(this));
}

/**
 * Destructor
 */
CETool::~CETool()
{
    // todo - consolidate removeObserver into CurveData dtor
    CurveList::iterator i;
    for (i = _curves.begin(); i != _curves.end(); i++) {
        delete *i;
        // TODO: remove observer
    }
}


err::Result
CETool::newCurve(msg::cstr name)
{
    // TODO: add create curve back
#if 0
    iSgExpr expr;
    EFAIL(iFactory().createInstance(expr, "iSgExpr", msg::value()),
          "Can't create curve.  Expr library not loaded?");
    if (!expr) return err::Result(errINTERNAL, "newCurve failed");
    addCurveInternal(expr, name);
#endif
    return errSUCCESS;
}


// TODO: add curve
err::Result
CETool::addCurve(animlib::AnimCurve* curve)
{
#if 0
    // get name as "node.parameter"
    iSgParam param;
    EFAIL(expr.getParent(param), "Can't get expr param");
    EFAILIF(!param, "addCurve failed, expr parent doesn't support iSgParam");
    msg::str name;
    EFAIL(param.getName(name), "Can't get param name");
    iSgNode node;
    EFAIL(param.getNode(node), "Can't get node from param");
    msg::str nodename;
    std::string cname;
    if (node) {
        node.getName(nodename);
        cname << nodename << "." << name;
    }
    else cname = name;
#endif

    addCurveInternal(curve, "curvename");
    
    //i23-206 Send To CE should Auto Fit
    int curveIndex = findCurve(curve);
    selectCurve(curveIndex);
    frameSelection();
       
    return errSUCCESS;
}


void
CETool::addCurveInternal(animlib::AnimCurve* curve, const char* name)
{
// TODO: add curve
    if (!name || !name[0]) name = "Curve";

    // uniquify name
    const char* basename = name;
    int suffix = 1;
    std::string tmpName=name;
    while (findCurve(tmpName.c_str()) >= 0) {
        suffix++;
        std::stringstream ss;
        ss<<basename<<" "<<suffix;
        tmpName=ss.str();
    }

    // add to end of list
    CurveData* curveData = new CurveData;
    curveData->animCurve = curve;
    curveData->name = tmpName;
    curveData->ypan = 0;
    curveData->yzoom = 1;
    curveData->selected = 0;
    curveData->numSegs = 0;
    curveData->numSegs=curve->getNumKeys();
    _curves.push_back(curveData);
    // TODO: we need an observer fix
    //curve->expr.addObserver(_rcvrid);
    emit curveListChanged();
}

err::Result
CETool::removeCurve(int curveIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;

    //CurveData* curve = _curves[curveIndex];
    //curve->expr.removeObserver(_rcvrid);
    //delete curve;
    delete _curves[curveIndex];
    _curves.erase(_curves.begin()+curveIndex);
    _selListValid = 0;
    emit curveListChanged();
    return errSUCCESS;
}


err::Result
CETool::removeSelectedCurves()
{
    CurveList::iterator i;
    for (i = _curves.begin(); i != _curves.end();) {
        CurveData* curve = *i;
        if (curve->selected) {
            //curve->expr.removeObserver(_rcvrid);
            delete curve;
            i = _curves.erase(i);
        }
        else i++;
    }
    _selListValid = 0;
    emit curveListChanged();
    return errSUCCESS;
}


err::Result
CETool::removeAllCurves()
{
    if (_curves.size()) {
        CurveList::iterator i;
        for (i = _curves.begin(); i != _curves.end(); i++) {
            CurveData* curve = *i;
            //curve->expr.removeObserver(_rcvrid);
            delete curve;
        }
        _curves.clear();
        _selListValid = 0;
        emit curveListChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::findCurve(int& curveIndex, msg::cstr name)
{
    for (int i = 0; i < numCurves(); i++) {
        if (_curves[i]->name == name) {
            curveIndex = i;
            return errSUCCESS;
        }
    }
    return errNOTFOUND;
}

err::Result
CETool::getCurve(animlib::AnimCurve*& animCurve, int curveIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    animCurve = _curves[curveIndex]->animCurve;
    return errSUCCESS;
}


err::Result
CETool::getCurves(AnimCurveList& curves)
{
    curves.clear();
    curves.resize(numCurves());
    for (int i = 0; i < numCurves(); i++) {
        curves[i]=_curves[i]->animCurve;
    }
    return errSUCCESS;
}


err::Result
CETool::getCurveNames(std::vector<std::string>& names)
{
    names.clear();
    for (int i = 0; i < numCurves(); i++) {
        names.push_back(_curves[i]->name);
    }
    return errSUCCESS;
}



err::Result
CETool::selectCurve(int curveIndex)
{
    ECHK(clearSelection());
    return selectAddCurve(curveIndex);
}


err::Result
CETool::selectAddCurve(int curveIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];
    if (!curve->selected || 
        (int) curve->segsSelected.size() != curve->numSegs) 
    {
        _selListValid = 0;
        curve->selected = 1;
        for (int i = 0; i < curve->numSegs; i++)
            curve->segsSelected.insert(i);
        emit selectionChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::deselectCurve(int curveIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];
    if (curve->selected) {
        curve->segsSelected.clear();
        curve->selected = 0;
        _selListValid = 0;
        emit selectionChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::clearSelection()
{
    bool changed = 0;
    for (int i = 0; i < numCurves(); i++) {
        CurveData* curve = _curves[i];
        if (curve->selected) {
            curve->segsSelected.clear();
            curve->selected = 0;
            changed = 1;
        }
    }
    if (changed) {
        _selListValid = 0;
        emit selectionChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::selectSegment(int curveIndex, int segIndex)
{
    ECHK(clearSelection());
    return selectAddSegment(curveIndex, segIndex);
}


err::Result
CETool::selectAddSegment(int curveIndex, int segIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];

    // validate first
    if (segIndex < 0 || segIndex >= curve->numSegs) return errINDEX;
    
    // then apply
    bool changed = 0;
    if (!curve->selected || !curve->segsSelected.count(segIndex)) {
        curve->segsSelected.insert(segIndex);
        curve->selected = 1;
        changed = 1;
    }
    if (changed) {
        _selListValid = 0;
        emit selectionChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::deselectSegment(int curveIndex, int segIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];

    bool changed = 0;
    if (curve->segsSelected.count(segIndex)) {
        curve->segsSelected.erase(segIndex);
        changed = 1;
    }
    if (changed) {
        _selListValid = 0;
        emit selectionChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::selectCurves(msg::list curveIndices)
{
    // validate first
    int ncurves = numCurves();
    bool* newState = (bool*) alloca(sizeof(bool)*ncurves);
    memset(newState, 0, sizeof(bool)*ncurves);

    int i;
    for (i = 0; i < curveIndices.size(); i++) {
        int curveIndex = curveIndices[i];
        if (curveIndex < 0 || curveIndex >= ncurves)
            return errINDEX;
        newState[curveIndex] = 1;
    }
    // apply
    bool changed = 0;
    for (i = 0; i < ncurves; i++) {
        CurveData* curve = _curves[i];
        if (newState[i]) {
            if (!curve->selected || 
                (int) curve->segsSelected.size() != curve->numSegs) 
            {
                curve->selected = 1;
                for (int i = 0; i < curve->numSegs; i++)
                    curve->segsSelected.insert(i);
                changed = 1;
            }
        }
        else {
            if (curve->selected) {
                curve->selected = 0;
                curve->segsSelected.clear();
                changed = 1;
            }
        }
    }

    if (changed) {
        _selListValid = 0;
        emit selectionChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::selectSegments(int curveIndex, msg::list segIndices)
{
    // validate first
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];
    int numSegs=curve->numSegs;
    bool* newState = (bool*) alloca(sizeof(bool)*numSegs);
    memset(newState, 0, sizeof(bool)*numSegs);
    int i;
    for (i = 0; i < segIndices.size(); i++) {
        int segIndex = segIndices[i];
        if (segIndex < 0 || segIndex >= numSegs)
            return errINDEX;
        newState[segIndex] = 1;
    }
    // apply
    bool changed = 0;
    for (i = 0; i < numSegs; i++) {
        bool selected = curve->selected && curve->segsSelected.count(i);
        if (selected != newState[i]) {
            if (newState[i]) {
                curve->selected = 1;
                curve->segsSelected.insert(i);
            } else {
                curve->segsSelected.erase(i);
            }
            changed = 1;
        }
    }

    if (changed) {
        _selListValid = 0;
        emit selectionChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::getSelection(msg::list& selections)
{
    if (!_selListValid) {
        // rebuild selectionList
        // first must count selections
        int numCurveSel = 0;
        for (int i = 0; i < numCurves(); i++)
            if (_curves[i]->selected) numCurveSel++;
        // make an entry for each selected curve
        _selList.newlist(numCurveSel*2);
        int index = 0;
        for (int i = 0; i < numCurves(); i++) {
            CurveData* curve = _curves[i];
            if (_curves[i]->selected) {
                // build list of selected seg indices
                msg::list segs(curve->segsSelected.size());
                std::set<int>::iterator seg;
                int segindex = 0;
                for (seg = curve->segsSelected.begin();
                     seg != curve->segsSelected.end();
                     seg++) 
                {
                    segs.set(segindex++, *seg);
                }
                // add curve index and seg indices to selList
                _selList.set(index++, i);
                _selList.set(index++, segs);
            }
        }
        _selListValid = 1;
    }
    selections = _selList;
    return errSUCCESS;
}


err::Result
CETool::getSelectedCurve(int& curve)
{
    msg::list sellist;
    ECHK(getSelection(sellist));
    int numcurves = sellist.size()/2;
    EFAILIF(!numcurves, "No curve selected");
    EFAILIF(numcurves>1, "More than one curve selected");
    int index = sellist[0].asint();
    ERETIF(index < 0 || index >= (int)_curves.size(), 
           errINTERNAL, "Bad curve index");
    curve = index;
    return errSUCCESS;
}


err::Result
CETool::getSelectedSegment(int& curve, int& seg)
{
    ECHK(getSelectedCurve(curve));
    CurveData* cdata = _curves[curve];
    int numsegs = cdata->segsSelected.size();
    EFAILIF(!numsegs, "No segment selected");
    EFAILIF(numsegs>1, "More than one curve selected");
    seg = *cdata->segsSelected.begin();
    return errSUCCESS;
}


err::Result
CETool::isSegmentSelected(bool& selected, int curveIndex, int segIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];

    if (segIndex < 0 || segIndex >= curve->numSegs) return errINDEX;
    
    selected = curve->selected && curve->segsSelected.count(segIndex) != 0;
    return errSUCCESS;
}


int
CETool::insertKey(double frame)
{
    // TODO: fix
    int curve;
    if(getSelectedCurve(curve)==errSUCCESS){
        animlib::AnimCurve* animCurve=_curves[curve]->animCurve;
        double val=animCurve->getValue(frame);
        auto it=animCurve->addKey(frame,val);
        it->setInTangentType(animlib::AnimKeyframe::kTangentAuto);
        it->setOutTangentType(animlib::AnimKeyframe::kTangentAuto);
        int segment=distance(animCurve->getFirstKey(),it);
        keyUpdated(_curves[curve]->animCurve,segment);
        _curves[curve]->numSegs=animCurve->getNumKeys();
        emit curveChanged(curve);
        return segment ;
    }
    return -1;
}


void
CETool::setSegmentStr(msg::cstr str)
{
    // TODO: fix
#if 0
    int curve;
    int seg;
    ECHK(getSelectedSegment(curve, seg));
    return _curves[curve]->expr.setSegmentStr(seg, str);
#endif
}

/* Operate on a selected segment */
void
CETool::setSelectedSegmentFrame(double frame)
{
    // TODO: fix
    int curve;
    int seg;
    if(getSelectedSegment(curve, seg) == errSUCCESS){
        _curves[curve]->animCurve->setTime(seg,frame);
        emit curveChanged(curve);
    }
}


void
CETool::setSelectedSegmentValue(double value)
{
    // TODO: fix
    int curve;
    int seg;
    if(getSelectedSegment(curve, seg) == errSUCCESS){
        setSegmentValue(value, curve, seg);
    }
}


void keysUpdated(animlib::AnimCurve* curve,int keyStart,int keyEnd,bool regenerateWeights){
        // compute derivatives based on spline rule (central differencing, left or right differencing)
        // we are iterating over keys and gathering left and right neighbor
    const double one_third=1./3.;
    const double deg2rad=M_PI/180.,rad2deg=180./M_PI;
    for(int i=keyStart;i<keyEnd;i++){
        double deriv=0;
        double deltaPrev=0,deltaNext=0,hNext=0,hPrev=0;
        animlib::AnimKeyframe *prev=(*curve)[i-1], *curr=(*curve)[i], *next=(*curve)[i+1];
        if(prev){
            deltaPrev=curr->getValue()-prev->getValue();
            hPrev=curr->getTime()-prev->getTime();
        }
        if(next){
            deltaNext=next->getValue()-curr->getValue();
            hNext=next->getTime()-curr->getTime();
        }
        // NOTE: this reduces to the one sided difference when we don't have prev or next
        deriv=(deltaPrev+deltaNext)/(hNext+hPrev);
        // now create clamped auto tangents
        double clampedDeriv=deriv;
        if(hPrev==0 || deltaPrev == 0) clampedDeriv=0;
        else{
            double clampDelta=deltaPrev/hPrev;
            clampedDeriv=clamp(clampedDeriv/clampDelta,0,3)*clampDelta;
        }
        if(hNext==0 || deltaNext == 0) clampedDeriv=0;
        else{
            double clampDelta=deltaNext/hNext;
            clampedDeriv=clamp(clampedDeriv/clampDelta,0,3)*clampDelta;
        }

        animlib::AnimKeyframe::tangentType inTangentType=curr->getInTangentType(),outTangentType=curr->getOutTangentType();
        double inDeriv=0,outDeriv=0;
        switch(inTangentType){
            case animlib::AnimKeyframe::kTangentFixed: break; //don't change them
            case animlib::AnimKeyframe::kTangentSmooth: inDeriv=deriv;break;
            case animlib::AnimKeyframe::kTangentAuto: inDeriv=clampedDeriv;break;
            case animlib::AnimKeyframe::kTangentLinear: inDeriv=hPrev!=0?deltaPrev/hPrev:0;break;
            case animlib::AnimKeyframe::kTangentFlat: inDeriv=0;break;
            default:  // don't know how to do this type, so pretend it doesn't exist!
                inDeriv=0;
                break;
        }
        switch(outTangentType){
            case animlib::AnimKeyframe::kTangentFixed: break; //don't change them
            case animlib::AnimKeyframe::kTangentSmooth: outDeriv=deriv;break;
            case animlib::AnimKeyframe::kTangentAuto: outDeriv=clampedDeriv;break;
            case animlib::AnimKeyframe::kTangentLinear: outDeriv=hNext!=0?deltaNext/hNext:0;break;
            case animlib::AnimKeyframe::kTangentFlat: outDeriv=0;break;
            default:  // don't know how to do this type, so pretend it doesn't exist!
                outDeriv=0;
                break;
        }

        bool weighted=curve->isWeighted();
        if(next && outTangentType != animlib::AnimKeyframe::kTangentFixed){
            double outAngle=atan(outDeriv);
            curr->setOutAngle(rad2deg*outAngle);
            curr->setOutWeight(weighted ? hNext*one_third/cos(outAngle) : 1);
        }
        if(inTangentType != animlib::AnimKeyframe::kTangentFixed && prev){
            double inAngle=atan(inDeriv);
            curr->setInAngle(rad2deg*inAngle);
            curr->setInWeight(weighted ? hPrev*one_third/cos(inAngle) : 1);
        }

        if(regenerateWeights){
            if(weighted){
                double cosInAngle=cos(curr->getInAngle()*deg2rad);
                double cosOutAngle=cos(curr->getInAngle()*deg2rad);
                curr->setInWeight(hPrev != 0 ? one_third*hPrev/cosInAngle : 1);
                curr->setOutWeight(hNext != 0 ? one_third*hNext/cosOutAngle : 1);
            }else{
                curr->setInWeight(1);
                curr->setOutWeight(1);
            }
        }
    }
    
}

void keyUpdated(animlib::AnimCurve* curve,int key){
    
    int numKeys=curve->getNumKeys();
    // key position changes affect spline tangents of neighbors, so do two in each direction to be safe!
    if(numKeys==1){

    }else{
        int keyStart=std::max(0,key-1);
        int keyEnd=std::min(numKeys,key+2);
        keysUpdated(curve,keyStart,keyEnd,false);
    }
}


void
CETool::setSelectedSegment(double frame,double val,
    double inAngle,double outAngle,
    double inWeight,double outWeight,
    animlib::AnimKeyframe::tangentType inType,
    animlib::AnimKeyframe::tangentType outType
)
{
    // TODO: fix
    int curve;
    int seg;
    getSelectedSegment(curve, seg);
    setSegment(curve,seg,frame,val,inAngle,outAngle,inWeight,outWeight,inType,outType);
}


void
CETool::setSegment(int curve,int seg,double frame,double val,
    double inAngle,double outAngle,
    double inWeight,double outWeight,
    animlib::AnimKeyframe::tangentType inType,
    animlib::AnimKeyframe::tangentType outType
)
{
    if(animlib::AnimKeyframe* key=(*_curves[curve]->animCurve)[seg]){
        _curves[curve]->animCurve->setTime(seg,frame);
        key->setValue(val);
        key->setInAngle(inAngle);
        key->setOutAngle(outAngle);
        key->setInWeight(inWeight);
        key->setOutWeight(outWeight);
        key->setInTangentType(inType);
        key->setOutTangentType(outType);
        keyUpdated(_curves[curve]->animCurve,seg);
        emit(curveChanged(curve));
    }
}


/*Operate on a specified Segment */
void
CETool::setSegmentFrame(double frame, int curve, int seg)
{
    // prevent making key cross other keys
    animlib::AnimCurve& anim=*_curves[curve]->animCurve;
    animlib::AnimKeyframe *prev=anim[seg-1],*curr=anim[seg],*next=anim[seg+1];
    // TODO: next ulp?
    if(prev && prev->getTime()>=frame) frame=prev->getTime()+.001;
    if(next && frame>=next->getTime()) frame=next->getTime()-.001;
    //curr->setTime(frame);
    _curves[curve]->animCurve->setTime(seg,frame);
    keyUpdated(_curves[curve]->animCurve,seg);
    emit curveChanged(curve);
}


void
CETool::setSegmentValue(double value, int curve, int seg)
{
    (_curves[curve]->animCurve->getFirstKey()+seg)->setValue(value);
    keyUpdated(_curves[curve]->animCurve,seg);
    emit curveChanged(curve);
}

void 
CETool::setSelectedWeighted(bool val)
{
    int curve,seg;
    if(getSelectedCurve(curve) == errSUCCESS)
        setWeighted(curve,val);
}

void
CETool::setWeighted(int curve, bool val)
{
    auto animCurve=_curves[curve]->animCurve;
    // now set all curves there had weights to 1.
    // if the curve changes from weighted to non, we set all weights to 1
    // if the curve changes from non-weighted to weighted, we set all weights to equivalent values to not change curve
    _curves[curve]->animCurve->setWeighted(val);
    keysUpdated(_curves[curve]->animCurve,0,_curves[curve]->animCurve->getNumKeys(),true);
    emit curveChanged(curve);
}

void 
CETool::setSelectedLocked(bool val)
{
    int curve,seg;
    if(getSelectedSegment(curve,seg) == errSUCCESS)
        setLocked(curve,seg,val);
}

void
CETool::setLocked(int curve, int segment, bool val)
{
    auto animCurve=_curves[curve]->animCurve;
    // now set all curves there had weights to 1.
    // if the curve changes from weighted to non, we set all weights to 1
    // if the curve changes from non-weighted to weighted, we set all weights to equivalent values to not change curve
    if(animlib::AnimKeyframe* key=(*_curves[curve]->animCurve)[segment]){
        
        key->setLocked(val);
        key->setInAngle(key->getOutAngle());
        key->setInTangentType(key->getOutTangentType());
        key->setInWeight(key->getOutWeight());
        keyUpdated(_curves[curve]->animCurve,segment);
        emit curveChanged(curve);
    }
}



err::Result
CETool::setSegmentType(msg::cstr type, msg::list params)
{
    // TODO: fix
#if 0
    int curve;
    int seg;
    ECHK(getSelectedSegment(curve, seg));
    return _curves[curve]->expr.setSegmentType(seg, type, params);
#endif
    return errSUCCESS;
}

void
CETool::setKeyIn(int curve,int seg,double ang,double weight)
{
    animlib::AnimKeyframe& key=*(_curves[curve]->animCurve->getFirstKey()+seg);
    bool weighted=_curves[curve]->animCurve->isWeighted();
    key.setInAngle(ang);
    key.setInWeight(weighted?weight:1);
    emit curveChanged(curve);
}

void
CETool::setKeyOut(int curve,int seg,double ang,double weight)
{
    animlib::AnimKeyframe& key=*(_curves[curve]->animCurve->getFirstKey()+seg);
    bool weighted=_curves[curve]->animCurve->isWeighted();
    key.setOutAngle(ang);
    key.setOutWeight(weighted?weight:1);
    emit curveChanged(curve);
}

void
CETool::setSelectedInfinity(animlib::AnimCurve::infinityType preType,animlib::AnimCurve::infinityType postType)
{
    int curve;
    int seg;
    getSelectedSegment(curve, seg);
    setInfinity(curve,preType,postType);
}

void
CETool::setInfinity(int curveIndex,animlib::AnimCurve::infinityType preType,animlib::AnimCurve::infinityType postType)
{
    animlib::AnimCurve& curve=*_curves[curveIndex]->animCurve;
    curve.setPreInfinity(preType);
    curve.setPostInfinity(postType);
    emit curveChanged(curveIndex);
}

err::Result
CETool::changeSegmentType(msg::cstr newtype)
{
    // TODO: fix
#if 0
    int curve;
    ECHK(getSelectedCurve(curve));
    CurveData* cdata = _curves[curve];
    std::set<int>::iterator iter, end;
    for (iter = cdata->segsSelected.begin(), end = cdata->segsSelected.end();
         iter != end; iter++)
    {
        if (*iter == 0) continue; // skip seg 0
        ECHK(cdata->expr.changeSegmentType(*iter, newtype));
    }
    return errSUCCESS;
#endif
    return errSUCCESS;
}

void
CETool::deleteSegments()
{
    bool changed = 0;
    for (int i = 0; i < numCurves(); i++) {
        CurveData* curve = _curves[i];
        std::set<int>::reverse_iterator seg;
        // must delete segments in reverse to preserve indices
        for (seg = curve->segsSelected.rbegin();
             seg != curve->segsSelected.rend();
             seg++) 
        {
            curve->animCurve->removeKey(curve->animCurve->getFirstKey()+*seg);
        }
        if (!curve->segsSelected.empty()) {
            curve->segsSelected.clear();
            changed = 1;
            emit curveChanged(i);
        }

    }

    if (changed) {
        _selListValid = 0;
        emit selectionChanged();
    }
}


err::Result
CETool::pan(double x, double y)
{
    if (x == 0 && y == 0) return errSUCCESS;
    _vx += x;
    _vy += y;
    emit viewChanged();
    return errSUCCESS;
}


err::Result
CETool::zoom(double zoomX, double zoomY)
{
    return zoomAroundPoint(zoomX, zoomY, _vx, _vy);
}


err::Result
CETool::zoomAroundPoint(double zoomX, double zoomY, double x, double y)
{
    if (zoomX <= 0 || zoomY <= 0) return errINVPARM;
    if (zoomX == 1 && zoomY == 1) return errSUCCESS;
    double sx = 1/zoomX;
    double sy = 1/zoomY;
    
    return setView(x + (_vx - x) * sx,
                   y + (_vy - y) * sy,
                   _vw * sx,
                   _vh * sy);
}


err::Result
CETool::setView(double x, double y, double w, double h)
{
    w = clamp(w, 1e-3, 1e5);
    h = clamp(h, 1e-3, 1e5);
    x = clamp(x, -1e5, 1e5);
    y = clamp(y, -1e5, 1e5);

    if (x != _vx || y != _vy || w != _vw || h != _vh) {
        _vx = x;
        _vy = y;
        _vw = w;
        _vh = h;
        emit viewChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::setCurveOffset(int curveIndex, double yPan, double yZoom)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];
    if (curve->ypan != yPan || 
        curve->yzoom != yZoom) 
    {
        curve->ypan = yPan;
        curve->yzoom = yZoom;
        emit viewChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::frameSelection()
{
// TODO: fix
    // if no curves, do nothing
    if (!numCurves()) return errSUCCESS;

    // get current view and adjust
    double x=_vx, y=_vy, w=_vw, h=_vh;
    
    // if we have no selection, then frame all
    bool frameAll = 1;
    int i;
    for (i = 0; i < numCurves(); i++) {
        CurveData* curve = _curves[i];
        if (curve->selected) { frameAll = 0; break; }
    }

    // find min/max time
    bool initX=0;
    double minX=std::numeric_limits<double>::max(), 
        maxX=-std::numeric_limits<double>::max(); // TODO: should be lowest in c++11

    for (i = 0; i < numCurves(); i++) {
        CurveData* curve = _curves[i];
        if (curve->numSegs<=1) continue;
        // find first/last selected seg
        int firstSeg = 1;
        int lastSeg = curve->numSegs-1;
        if (!frameAll) {
            if (!curve->selected) continue;
            if (curve->segsSelected.size()) {
                firstSeg = *curve->segsSelected.begin();
                lastSeg = *curve->segsSelected.rbegin()+1;
                if (lastSeg >= curve->numSegs) lastSeg = curve->numSegs-1;
                if (firstSeg < 1) firstSeg = 1;
            }
        }
        double min=curve->animCurve->getFirstKey()->getTime();
        double max=(curve->animCurve->getFirstKey()+curve->animCurve->getNumKeys()-1)->getTime();

        
        minX=std::min(minX,min);
        maxX=std::max(maxX,max);
    }
    
    // adjust horizontal view
    if (numCurves()) {
        x = (maxX+minX)/2;
        if (maxX > minX)
            w = (maxX-minX)*1.1; // add 10% margin
    }
    else {
        minX = x-w/2;
        maxX = x+w/2;
    }

    // find min/max value over min/max time range
    bool initY=false;
    double minY=std::numeric_limits<double>::max(), 
        maxY=-std::numeric_limits<double>::max();// TODO: should be lowest in c++11

    bool gotData=false;
    for (i = 0; i < numCurves(); i++) {
        CurveData* curve = _curves[i];
        if (!frameAll && !curve->selected) continue;
        // TODO: this is not the correct bouds in the presence of oscillation
        for(int i=0;i<(int)curve->animCurve->getNumKeys();i++){
            gotData=true;
            double val=(curve->animCurve->getFirstKey()+i)->getValue();
            maxY=std::max(maxY,val);
            minY=std::min(minY,val);
        }
    }

    // adjust vertical view
    if (gotData) {
        y = (maxY+minY)/2;
        if (maxY > minY)
            h = (maxY-minY)*1.1;
    }

    // update to new view
    setView(x,y,w,h);

    return errSUCCESS;
}

err::Result
CETool::getView(double& x, double& y, double& w, double& h)
{
    x = _vx;
    y = _vy;
    w = _vw;
    h = _vh;
    return errSUCCESS;
}


err::Result
CETool::getCurveOffset(double& yPan, double& yZoom, int curveIndex)
{
    if (curveIndex < 0 || curveIndex >= numCurves())
        return errINDEX;
    CurveData* curve = _curves[curveIndex];
    yPan = curve->ypan;
    yZoom = curve->yzoom;
    return errSUCCESS;
}


err::Result
CETool::setTimeMode(int mode)
{
    if (mode < 0 || mode > 2)
        return errINVPARM;
    if (_timeMode != mode) {
        _timeMode = mode;
        emit timeModeChanged();
    }
    return errSUCCESS;
}


err::Result
CETool::getTimeMode(int& mode)
{
    mode = _timeMode;
    return errSUCCESS;
}


err::Result
CETool::map(QWidget*& uiComponent, QWidget* parent)
{
    _ui = new CEMainUI(parent, this);
    uiComponent = _ui;
    return errSUCCESS;
}


err::Result
CETool::postUnmapNotify()
{
    _ui = 0;
    return errSUCCESS;
}

err::Result
CETool::postMapNotify()
{
    return errSUCCESS;
}

int 
CETool::findCurve(animlib::AnimCurve* curve)
{
    // TODO - do something more efficient than linear search
    // could use dict, but must take care to keep indices current
    for (int i = 0; i < numCurves(); i++)
        if (_curves[i]->animCurve == curve) return i;
    return -1;
}

int 
CETool::findCurve(const std::string& curveName)
{
    // TODO - do something more efficient than linear search
    // could use dict, but must take care to keep indices current
    for (int i = 0; i < numCurves(); i++)
        if (_curves[i]->name==curveName) return i;
    return -1;
}
