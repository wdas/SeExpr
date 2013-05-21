/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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










