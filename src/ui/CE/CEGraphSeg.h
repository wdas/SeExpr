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
#ifndef CEGraphSeg_h
#define CEGraphSeg_h

#include <string>

class CEGraphCurve;
class CEDragHandler;

class CEGraphSeg
{
public:
#if 0
    struct InitData {
	InitData(CEGraphCurve* c, int n) : curve(c), index(n) {}
	CEGraphCurve* curve;
	int index;
    };
#endif
    CEGraphSeg(CEGraphCurve* curve,int index);
    virtual ~CEGraphSeg();
    CEGraphCurve* curve() { return _curve; }
    int index() { return _index; }
    bool firstSeg() { return _index==0; }
    bool unbounded() { return _unbounded; }
    const char* type() { return _type.c_str(); }
    int numParams() { return _numParams; }
    double param(int n) { return n < 0 || n >= _numParams ? 0 : _params[n]; }
    double keyTime() { return _keytime; }
    double keyValue() { return _keyval; }
    double outTime() { return _outtime; }
    double outValue() { return _outval; }

    enum Part {
	// these are listed in increasing grab priority
	NoPart, Seg, HandleBase=100, KnobBase=200, KeyTime=300, Key, 
    };
    void invalidateView() { _viewValid = 0; }

    void paint(bool selected);
    CEDragHandler* getDragHandler(int part, int keystate);
    virtual CEDragHandler* getSegmentHandler(int part, int keystate) 
        { return 0; }
    void setPendingSel(bool pendingSel) { _pendingSel = pendingSel; }
    bool isPendingSel() { return _pendingSel; }

protected:
    bool beginPart(Part part);
    void endPart();
    virtual void paintSeg()=0;
    virtual void paintHandles() {}
    void paintHandle(int num, double x, double y, double dx, double dy,
		     double zx, double zy, bool joinKnob=1);

    bool _viewValid;

    void paintKey();

    CEGraphCurve* _curve;	// parent curve
    int _index;			// index of segment in curve
    bool _unbounded;		// true if seg extends to +infinity
    std::string _type;		// segment interpolationtype
    int _numParams;		// number of segment params
    double *_params;		// segment params

    // key data (not valid for first seg)
    double _keytime;		// time of key starting this seg
    double _keyval;		// value of this seg at key time
    // out data (not valid for last seg)
    double _outtime;		// time of next key
    double _outval;		// value of this seg at out time
    double _nextval;		// value of next seg at out time
    
    bool _selected;		// only valid during a paint
    bool _pendingSel;		// true if being marquee selected
};


// Handles uniform sampling - provides no drag handles
class CEGraphUniformSeg : public CEGraphSeg
{
public:
    CEGraphUniformSeg(CEGraphCurve* curve,int index);
    ~CEGraphUniformSeg();

protected:
    virtual void paintSeg();

private:
    void updateView();

    // sample data
    double _sampleStart;
    double _sampleEnd;
    double _sampleSpacing;
    int _numSamples;
    double* _samples;
};

class CEGraphBezSeg : public CEGraphUniformSeg
{
public:
    CEGraphBezSeg(CEGraphCurve* curve,int index);
protected:
    virtual void paintHandles();
    virtual CEDragHandler* getSegmentHandler(int part, int keystate);
};


#endif
