/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#ifndef CEGraphView_h
#define CEGraphView_h

#include <math.h>

class CEGraphView {
public:
    CEGraphView()
	: _vx(0), _vy(0), _vw(1), _vh(1),
	  _px(0), _py(0), _pw(1), _ph(1) {}
    void setView(double vx, double vy, double vw, double vh, 
		 int px, int py, int pw, int ph)
    {
	_vx = vx;
	_vy = vy;
	_vw = vw;
	_vh = vh;
	_px = px;
	_py = py;
	_pw = pw;
	_ph = ph;
	_pixWidth = _vw / _pw;
	_pixHeight = _vh / _ph;
	_vxScale = _pw / _vw;
	_vyScale = _ph / _vh;
	_vxTrans = (_vw/2 - _vx) * (_pw/_vw) + _px;
	_vyTrans = (_vh/2 - _vy) * (_ph/_vh) + _py;
    }

    // view rectangle in view coords (x,y is center of view)
    double vx() const { return _vx; }
    double vy() const { return _vy; }
    double vw() const { return _vw; }
    double vh() const { return _vh; }

    // sides of view rectangle
    double left()   const { return _vx - _vw/2; }
    double right()  const { return _vx + _vw/2; }
    double bottom() const { return _vy - _vh/2; }
    double top()    const { return _vy + _vh/2; }

    // view rectangle in pixel coords (x,y is offset of view in window)
    int px() const { return _px; }
    int py() const { return _py; }
    int pw() const { return _pw; }
    int ph() const { return _ph; }

    // pixel size in view units
    double pixelWidth() const { return _pixWidth; }
    double pixelHeight() const { return _pixHeight; }

    // view coord to pix coord conversion
    double pxd(double vx) const { return vx * _vxScale + _vxTrans; }
    double pyd(double vy) const { return vy * _vyScale + _vyTrans; }
    int px(double vx) const { return int(floor(vx * _vxScale + _vxTrans)); }
    int py(double vy) const { return int(floor(vy * _vyScale + _vyTrans)); }

    // pix coord to view coord conversion
    double vx(int px) const { return (px - _vxTrans) / _vxScale; }
    double vy(int py) const { return (py - _vyTrans) / _vyScale; }

private:
    double _vx, _vy;     // center of view in view units
    double _vw, _vh;     // size of view in view units
    int    _px, _py;     // offset of view in window
    int    _pw, _ph;     // size of view in pixels
    double _pixWidth;    // width of pixel in view units
    double _pixHeight;   // height of pixel in view units
    double _vxScale;	 // x scale factor for view to pix conversion
    double _vyScale;	 // y scale factor for view to pix conversion
    double _vxTrans;	 // x translation factor for view to pix conversion
    double _vyTrans;	 // y translation factor for view to pix conversion
};


#endif
