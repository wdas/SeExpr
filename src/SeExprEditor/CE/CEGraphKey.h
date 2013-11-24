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
#ifndef CEGraphKey_h
#define CEGraphKey_h
#include <animlib/AnimKeyframe.h>

class CEGraphCurve;
class CEDragHandler;
class CEGraphKey{
public:
    CEGraphKey(CEGraphCurve* curve,int index);
    ~CEGraphKey();
    void paint(bool selected);

    enum Part {
	// these are listed in increasing grab priority
	NoPart, Seg, HandleBase=100, KnobBase=200, KeyTime=300, Key, 
    };

    CEDragHandler* getDragHandler(int part,int keystate);
    CEGraphCurve* curve(){return _curve;}
    int index(){return _index;}
    const animlib::AnimKeyframe& key() const{return _key;}
    bool weighted() const{return _weighted;}
private:
    bool beginPart(Part part);
    void endPart();
    void paintHandles();
    void paintHandle(int num,double x,double y,double dx,double dy,bool fixedHandle);
    
    bool _selected;
    CEGraphCurve* _curve;
    int _index;
    const animlib::AnimKeyframe _key;
    bool _weighted;
    double _handleAnchorX[2];
    double _handleAnchorY[2];
};


#endif










