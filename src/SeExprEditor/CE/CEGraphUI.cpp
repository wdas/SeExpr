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
* @file CEGraphUI.cpp
* @brief Contains the Definition of class CEGraphUI.
*
* 09/18/06 - rsharma Changed setRawName to fromString for xft
*/

#include "CEGraphUI.h"
#include <iostream>
#include <alloca.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "CETool.h"
#include "CEGraphCurve.h"
#include "CEGraphKey.h"
#include "CEGraphSeg.h"
#include "CEDragHandlers.h"

#include <iostream>
using namespace std;

#include <QtCore/Qt>
#include <QtGui/QMouseEvent>

// #include <GL/glx.h> conflicts w/ Qt!  Just declare what we need.
extern "C" void glXUseXFont(Qt::HANDLE, int, int, int);


static double RaiseToInc(double val, double inc)
{
    return ceil(val/inc)*inc;
}


static double LowerToInc(double val, double inc)
{
    return floor(val/inc)*inc;
}


/**
 * Constructor.
 */
CEGraphUI::CEGraphUI(QWidget* parent, CETool* tool)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer|QGL::HasOverlay),
		parent), 
      _tool(tool),
     //_ce(tool->rcvrId()),
      _fontListBase(0),
      _fontListCount(0),
      _viewValid(0),
      _showCrossHairs(0),
      _crossX(0), _crossY(0),
      _showSelBox(0),
      _selBoxX1(0), _selBoxY1(0), _selBoxX2(0), _selBoxY2(0),
      _curveListValid(0),
      _activeCurve(-1),
      _activeSeg(-1),
      _activePart(-1),
      _dragHandler(0)
{
    setObjectName("Graph");

    setMouseTracking(1);

    QFontMetrics fm(_font);
    _scalesWidth = fm.width("-99999")+15;
    _scalesHeight = fm.ascent()+12;

    // wire-up the notifications
    connect(_tool, SIGNAL(curveChanged(int)),  SLOT(invalidateCurve(int)));
    connect(_tool, SIGNAL(curveListChanged()), SLOT(invalidateCurveList()));
    connect(_tool, SIGNAL(selectionChanged()), SLOT(invalidateSelection()));
    connect(_tool, SIGNAL(viewChanged()),      SLOT(invalidateView()));
    connect(_tool, SIGNAL(timeModeChanged()),  SLOT(invalidateTimeMode()));

    // init the colors
    _colors[BgColor].setRgb(48,48,48);
    _colors[ScaleBgColor].setRgb(128,128,128);
    _colors[ScaleColor].setRgb(0,0,0);
    _colors[GridColor].setRgb(80,80,80);
    _colors[TimebarColor].setRgb(192,192,255);
    _colors[OverlayColor].setRgb(192,192,200);
    _colors[SelectedColor].setRgb(204,204,38);
    _colors[HiSelectedColor] = _colors[SelectedColor].light();
    _colors[CurveColor].setRgb(200,105,50);
    _colors[HiCurveColor].setRgb(200,135,50); //  = _colors[CurveColor].light();
}

/**
 * Destructor.
 */
CEGraphUI::~CEGraphUI()
{
    clearCurves();
}

/*
void
CEGraphUI::show()
{
    QGLWidget::show();
    invalidateView();
    updateGL();
    resize(width()-1, height()-1);
}
*/

void
CEGraphUI::setColor(ColorType color)
{
    qglColor(_colors[color]);
}


const QColor&
CEGraphUI::getColor(ColorType color)
{
    return _colors[color];
}


void
CEGraphUI::clearCurves()
{   
    for (size_t i = 0; i < _curves.size(); i++) {
        delete _curves[i];
    }
    _curves.clear();
}


CEGraphCurve*
CEGraphUI::getCurve(int curve)
{
    if (!_curveListValid) updateCurves();
    if (curve < 0 || curve >= (int) _curves.size()) return 0;
    return _curves[curve];
}


CEGraphSeg*
CEGraphUI::getSeg(int curve, int seg)
{
    if (!_curveListValid) updateCurves();
    if (curve < 0 || curve >= (int) _curves.size()) return 0;
    CEGraphKey* keyPtr=0;
    CEGraphSeg* segPtr=0;
    _curves[curve]->getSegAndKey(seg,segPtr,keyPtr);
    return segPtr;
}

CEGraphKey*
CEGraphUI::getKey(int curve, int seg)
{
    if (!_curveListValid) updateCurves();
    if (curve < 0 || curve >= (int) _curves.size()) return 0;
    CEGraphKey* keyPtr=0;
    CEGraphSeg* segPtr=0;
    _curves[curve]->getSegAndKey(seg,segPtr,keyPtr);
    return keyPtr;
}


const CEGraphView&
CEGraphUI::getView()
{
    if (!_viewValid) updateView();
    return _view;
}


int
CEGraphUI::doSelectModeRender(int x1, int x2, int y1, int y2,
			      GLuint* buffer, int bufsize)
{
    makeCurrent();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    if (x1 == x2) x2++;
    if (y1 == y2) y2++;
    
    glOrtho(x1, x2, y1, y2, -1, 1);
    glSelectBuffer(bufsize, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    paintCurves();
    int numHits = glRenderMode(GL_RENDER);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    return numHits;
}


void CEGraphUI::initializeGL()
{
    QGLWidget::initializeGL();
}


void CEGraphUI::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h );
    // set GL projection matrix to map GL units to pixels
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
#ifdef linux
    glTranslated(0.25, 0.25, 0);
#endif
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void CEGraphUI::resizeOverlayGL(int w, int h)
{
    // we use the same coordinate system in the overlay
    resizeGL(w, h);
}


void CEGraphUI::paintGL()
{
    qglClearColor(_colors[BgColor]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!_viewValid)
    {
        updateView();
    }
    if (!_curveListValid)
    {
        updateCurves();
    }
    paintScales();
    paintCurves();
}


void CEGraphUI::paintOverlayGL()
{
    qglClearColor(overlayContext()->overlayTransparentColor());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintOverlay();
}


void CEGraphUI::getNextIncrement(double min, int timeMode,
				 double& inc, int& decimals)
{
    decimals = 0;
    if (min >= 1 && timeMode > 0) {
        // integer case - increments depend on mode
        if (timeMode == 1) {
            // feet-frames mode
            if (min <= 16.001) {
                // sub-foot
            	if (min > 8)      inc = 16;
                else if (min > 4) inc = 8;
                else if (min > 2) inc = 4;
                else if (min > 1) inc = 2;
                else inc = 1;
                return;
            }
            // multiples of feet
            getNextIncrement(min/16, 0, inc, decimals);
            inc *= 16;
            return;
            
        } else if (timeMode == 2) {
            // seconds-frames mode
            if (min <= 24.001) {
                if (min > 12)     inc = 24;
                else if (min > 8) inc = 12;
                else if (min > 4) inc = 8;
                else if (min > 2) inc = 4;
                else if (min > 1) inc = 2;
                else inc = 1;
                return;
            }
            // multiples of seconds
            getNextIncrement(min/24, 0, inc, decimals);
            inc *= 24;
            return;
        }
    }
    // default case, use 1's and 5's
    double temp = floor(log10(min));
    if (min < 1) decimals = int(-temp);
    inc = pow(10, temp);   // 1's
    if (inc >= min) return;
    inc *= 2;   // 2's
    if (inc >= min) return;
    inc *= 2.5; // 5's
    if (inc >= min) return;
    inc *= 2;   // 10's
    if (decimals > 0) decimals--;
}



void CEGraphUI::getNextTickIncrement(double inc, 
				     int timeMode,
				     double& tickInc)
{
    if (inc >= 2 && timeMode > 0) {
        if (timeMode == 1) {
            // feet-frames mode
            if (inc <= 16.001) {
                // sub-foot
                if (inc > 8)      tickInc = 4;
                else if (inc > 4) tickInc = 2;
                else              tickInc = 1;
                return;
            }
            // next power of 10 (in feet) below inc
            tickInc = pow(10, ceil(log10(inc*(.1/16))))*16;
            return;
        
        } else if (timeMode == 2) {
            // seconds-frames mode
            if (inc <= 24.001) {
                // sub-second
                if (inc > 12)     tickInc = 8;
                else if (inc > 8) tickInc = 4;
                else if (inc > 4) tickInc = 2;
                else              tickInc = 1;
                return;
            }
            // next power of 10 (in frames) below inc
            tickInc = pow(10, ceil(log10(inc*(.1/24))))*24;
            return;
        }
    }
    // default: tickInc should always be the next power of 10 below inc
    tickInc = pow(10, ceil(log10(inc*.1)));
}


void CEGraphUI::paintScales()
{
    // some handy values
    int w = width();
    int h = height();
    int sw = _scalesWidth;
    int sh = _scalesHeight;

    double l = _view.left();
    double r = _view.right();
    double b = _view.bottom();
    double t = _view.top();

    int timeMode;
    _tool->getTimeMode(timeMode);

    // compute increments from view and window sizes
    double gridIncX, gridIncY, tickIncX, tickIncY;
    int gridDecimalsX, gridDecimalsY;
    getNextIncrement(60.0/w*_view.vw(), timeMode, gridIncX, gridDecimalsX);
    getNextIncrement(60.0/h*_view.vh(), 0, gridIncY, gridDecimalsY);
    getNextTickIncrement(gridIncX, timeMode, tickIncX);
    getNextTickIncrement(gridIncY, 0, tickIncY);

    // clear scales to scale bg color
    glEnable(GL_SCISSOR_TEST);
    qglClearColor(_colors[ScaleBgColor]);
    glScissor(0, 0, w, sh-1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glScissor(0, 0, sw-1, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // draw time-display mode button
    QFontMetrics fm(_font);
    setColor(ScaleColor);
    const char* modeText;
    switch (timeMode) {
        case 0: modeText = "f"; break;
        case 1: modeText = "f-f"; break;
        case 2: modeText = "s:f"; break;
        default: modeText = "?"; break;
    }

    // draw box around time mode to indicate button
    { 
        int tw = fm.width(modeText);
        int th = fm.ascent();
        printStringAt(17-tw/2, 4, modeText);
        glBegin(GL_LINE_LOOP);
            glVertex2i(2, 1);
            glVertex2i(2, th+3);
            glVertex2i(32, th+3);
            glVertex2i(32, 1);
        glEnd();
    }

    // draw time scale
    double x, y;
    setColor(ScaleColor);
    glBegin(GL_LINES);
    if (tickIncX) {
        int tickBottom = sh-7;
        int tickTop = sh-1;
        for (x = RaiseToInc(l,tickIncX); x <= r; x += tickIncX) {
            int px = _view.px(x);
            glVertex2i(px, tickBottom);
            glVertex2i(px, tickTop);
        }
    }

    int gridBottom = sh-12;
    int gridTop = sh-1;
    for (x = RaiseToInc(l,gridIncX); x <= r; x += gridIncX) {
        int px = _view.px(x);
        glVertex2i(px, gridBottom);
        glVertex2i(px, gridTop);
    }
    glEnd();

    for (x = RaiseToInc(l,gridIncX); x <= r; x += gridIncX) {
        char prefix[20];
        double frames;
        switch (timeMode) {
            case 1: // feet-frames
                {
                    int feet = int(floor(x/16));
                    frames = x-feet*16;
                    sprintf(prefix, "%d-", feet);
                }
                break;
            case 2: // seconds-frames
                {
                	int seconds = int(floor(x/24));
                	frames = x-seconds*24;
                	sprintf(prefix, "%d:", seconds);
                }
                break;
            default:
                frames = x;
                prefix[0] = '\0';
                break;
        }
        
        char buff[40];
        if (gridDecimalsX)
            sprintf(buff, "%s%.*f", prefix, gridDecimalsX, frames);
        else
            sprintf(buff, "%s%d", prefix, int(frames));
        
        printStringAt(_view.px(x)-fm.width(buff)/2, 4, buff);
    }

    // draw value scale
    glBegin(GL_LINES);
    if (tickIncY) {
        int tickLeft = sw-7;
        int tickRight = sw-1;
        for (y = RaiseToInc(b,tickIncY); y <= t; y += tickIncY) {
            int py = _view.py(y);
            glVertex2i(tickLeft, py);
            glVertex2i(tickRight, py);
        }
    }
    int gridLeft = sw-12;
    int gridRight = sw-1;
    for (y = RaiseToInc(b,gridIncY); y <= t; y += gridIncY) {
        int py = _view.py(y);
        glVertex2i(gridLeft, py);
        glVertex2i(gridRight, py);
    }
    glEnd();
    int textOffsetY = fm.ascent()/2-3;
    for (y = RaiseToInc(b,gridIncY); y <= t; y += gridIncY) {
        char buff[20];
        if (gridDecimalsY)
            sprintf(buff, "%.*f", gridDecimalsY, y);
        else
            sprintf(buff, "%d", int(y));
        int left = (_scalesWidth-fm.width(buff)-7)/2;
        if (left < 0) left = 0;
        printStringAt(left, _view.py(y)-textOffsetY, buff);
    }

    // clear main area to bg color
    glEnable(GL_SCISSOR_TEST);
    qglClearColor(_colors[BgColor]);
    glScissor(sw, sh, w-sw, h-sh);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // draw grid lines
    //glEnable(GL_LINE_STIPPLE);
    //glLineStipple(4, 0x5555);
    setColor(GridColor);
    // (start on multiple of 16 pixels to stabilize stipple)
    int gb = _view.py(LowerToInc(b, 16*_view.pixelHeight()));
    int gt = _view.py(t);
    for (x = RaiseToInc(l,gridIncX); x <= r; x += gridIncX) {
        int px = _view.px(x);
        glBegin(GL_LINES);
        glVertex2i(px, gb);
        glVertex2i(px, gt);
        glEnd();
    }
    
    int gl = _view.px(LowerToInc(l, 16*_view.pixelWidth()));
    int gr = _view.px(r);
    for (y = RaiseToInc(b,gridIncY); y <= t; y += gridIncY) {
        int py = _view.py(y);
        glBegin(GL_LINES);
        glVertex2i(gl, py);
        glVertex2i(gr, py);
        glEnd();
    }
    // glDisable(GL_LINE_STIPPLE);
    glDisable(GL_SCISSOR_TEST);

    // draw border between scales and main area
    setColor(ScaleColor);
    glBegin(GL_LINE_STRIP);
    glVertex2i(sw-1, h);
    glVertex2i(sw-1, sh-1);
    glVertex2i(w, sh-1);
    glEnd();
}



void 
CEGraphUI::paintCurves()
{
    int w = width();
    int h = height();
    int sw = _scalesWidth;
    int sh = _scalesHeight;

    // grab the current GL context.
    makeCurrent();

    glEnable(GL_SCISSOR_TEST);
    glScissor(sw, sh, w-sw, h-sh);

    // paint curves w/ selected segs last to put them on top
    for (int selected = 0; selected <= 1; selected++)
        for (int i = _curves.size()-1; i >= 0; i--)
            _curves[i]->paint(selected);
    glDisable(GL_SCISSOR_TEST);
}


void CEGraphUI::paintOverlay(bool erase)
{
    if (!overlayContext()) return;
    // erase the overlay by re-drawing as transparent to reduce flicker
    qglColor(erase ? overlayContext()->overlayTransparentColor() :
	     _colors[OverlayColor]);
    if (_showCrossHairs)
        paintCrossHairs();
    if (_showSelBox)
        paintSelBox();
    // paintTimeBar();
}


void CEGraphUI::paintCrossHairs()
{
    int x = _crossX;
    int y = _crossY;
    bool drawX = x >= _scalesWidth;
    bool drawY = y >= _scalesHeight;
    if (!drawX || !drawY) return;

    glBegin(GL_LINES);
    if (drawX) {
        glVertex2i(x, _scalesHeight);
        glVertex2i(x, _scalesHeight+10);
    }
    if (drawY) {
        glVertex2i(_scalesWidth, y);
        glVertex2i(_scalesWidth+10, y);
    }
    glEnd();
}


void CEGraphUI::moveCrossHairs(int x, int y)
{
    makeOverlayCurrent();
    if (!_showCrossHairs)
        _showCrossHairs = 1;
    else
        paintOverlay(true); // true => erase? yes.
    _crossX = x;
    _crossY = y;
    paintOverlay();

    makeCurrent();
    glFlush();
}


void CEGraphUI::hideCrossHairs()
{
    if (!_showCrossHairs) return;
    makeOverlayCurrent();
    paintOverlay(true); // true => erase? yes.
    _showCrossHairs = 0;
    paintOverlay();

    makeCurrent();
    glFlush();
}


void CEGraphUI::pickActive(int x, int y)
{
    static const int bufsize = 1000;
    GLuint selbuff[bufsize];
    int numHits = doSelectModeRender(x-4, x+5, y-4, y+5, selbuff, bufsize);

    // Look for highest priority object.  In the case of a tie, we prefer
    // the one that is selected.  If there's still a tie, then we pick the
    // last one rendered (which is the one on top).
    int activeCurve = -1;
    int activeSeg = -1;
    int activePart = -1;
    bool activeSelected = 0;
    GLuint *p = selbuff;

    while (numHits--) {
	GLuint numNames = *p++;
	*p++; // GLuint zmin = 
	*p++; // GLuint zmax = 
	if (numNames >= 3)
    {
        int curve = p[0];
        int seg = p[1];
        int part = p[2];
        bool selected = 0;
        _tool->isSegmentSelected(selected, curve, seg).suppress();
        
        if (part > activePart ||
           (part == activePart && 
           (selected || !activeSelected)))
        {
            activeCurve = curve;
            activeSeg = seg;
            activePart = part;
            activeSelected = selected;
        }
	}
        p += numNames;
    }

    if (activeCurve != _activeCurve ||
        activeSeg != _activeSeg ||
        activePart != _activePart)
    {
        _activeCurve = activeCurve;
        _activeSeg = activeSeg;
        _activePart = activePart;
        update();
    }
}

void CEGraphUI::paintSelBox()
{
    int x1 = _selBoxX1;
    int y1 = _selBoxY1;
    int x2 = _selBoxX2;
    int y2 = _selBoxY2;

    glEnable(GL_SCISSOR_TEST);
    glScissor(_scalesWidth, _scalesHeight,
	      width()-_scalesWidth, height()-_scalesHeight);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x5555);

    glBegin(GL_LINE_STRIP);
    glVertex2i(x1,y1);
    glVertex2i(x2,y1);
    glVertex2i(x2,y2);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex2i(x1,y1);
    glVertex2i(x1,y2);
    glVertex2i(x2,y2);
    glEnd();

    glDisable(GL_LINE_STIPPLE);
    glDisable(GL_SCISSOR_TEST);
}


void CEGraphUI::moveSelBox(int x1, int y1, int x2, int y2)
{
    makeOverlayCurrent();
    if (!_showSelBox)
        _showSelBox = 1;
    else
        paintOverlay(true); // true => erase? yes.
        
    _selBoxX1 = x1;
    _selBoxY1 = y1;
    _selBoxX2 = x2;
    _selBoxY2 = y2;
    paintOverlay();

    makeCurrent();
    glFlush();
}


void CEGraphUI::hideSelBox()
{
    if (!_showSelBox) return;
    makeOverlayCurrent();
    paintOverlay(true); // true => erase? yes.
    _showSelBox = 0;
    paintOverlay();

    makeCurrent();
    glFlush();
}



void CEGraphUI::paintTimeBar(double frame)
{
#if 0
    setViewXForm(getView(),1,0);
    glBegin(GL_LINES);
    glVertex2i(frame, _scalesHeight);
    glVertex2i(frame, height());
    glEnd();
#endif
}


void CEGraphUI::printStringAt(double x, double y, const char* str) 
{
    // check for legal chars in string
    for (const char* cp = str; *cp; cp++) {
        if (*cp < ' ' || ((unsigned char)*cp) >= 128) {
            // got an illegal char - bail out for now
            std::cerr << "Illegal char in GL string - skipping: " << str
                  << std::endl;
            return;
        }
    }

    glEnable(GL_DEPTH_TEST);
    renderText(x, y, 0.1, str, _font);
    glDisable(GL_DEPTH_TEST);
}

void CEGraphUI::resizeEvent(QResizeEvent *e)
{
    invalidateView();
    QGLWidget::resizeEvent(e);
}

void CEGraphUI::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Backspace || event->key()==Qt::Key_Delete){
        _tool->deleteSegments();
    }
}

void CEGraphUI::mousePressEvent(QMouseEvent* e)
{
    setFocus();
    // ignore button press if a drag is already active
    if (_dragHandler) return;

    int x = e->pos().x();
    int y = height() - e->pos().y();

    // select a drag handler
    if (e->button() == Qt::LeftButton) 
    {
        if (_activeCurve >= 0 && _activeSeg >= 0) 
        {
            if(auto key = getKey(_activeCurve, _activeSeg))
            {
                // first, select segment (or add/toggle based on modifiers)
                bool selected = 0;
                _tool->isSegmentSelected(selected, _activeCurve,
					 _activeSeg);
                int keystate = e->modifiers() & (Qt::ShiftModifier   | 
                                                 Qt::ControlModifier |
                                                 Qt::AltModifier     |
                                                 Qt::MetaModifier);
                if (keystate == Qt::ShiftModifier) 
                {
                    // add segment to selection
                    if (!selected)
                    _tool->selectAddSegment(_activeCurve, _activeSeg);
                }
                else if (keystate == Qt::ControlModifier) 
                {
                    // toggle segment selection
                    if (selected) 
                        _tool->deselectSegment(_activeCurve, _activeSeg);
                    else 
                        _tool->selectAddSegment(_activeCurve, _activeSeg);
                }
                else 
                {
                    // select segment
                    int curve=-1, seg=-1;
                    _tool->getSelectedSegment(curve, seg).suppress();
                    if (curve != _activeCurve || seg != _activeSeg)
                        _tool->selectSegment(_activeCurve, _activeSeg);
                }
                // chose a handler for the seg
                //_dragHandler =0; // seg->getDragHandler(_activePart, keystate);
                _dragHandler = key->getDragHandler(_activePart,keystate);
                if(!_dragHandler){
                    if(auto seg = getSeg(_activeCurve,_activeSeg)){
                        _dragHandler = seg->getDragHandler(_activePart,keystate);
                    }
                }
            }
        } 
        else 
        {
            int keystate = e->modifiers() & (Qt::ShiftModifier   | 
                                             Qt::ControlModifier |
                                             Qt::AltModifier     |
                                             Qt::MetaModifier);
            bool append = keystate == Qt::ShiftModifier;
            bool toggle = keystate == Qt::ControlModifier;
            bool addKey = keystate == Qt::AltModifier;
            if(addKey){
                // special case no keys in curve and you still want to add one
                int curve;
                if(_tool->getSelectedCurve(curve)==errSUCCESS){
                    double _anchorTime = _view.vx(x);
                    double _anchorValue = _view.vy(y);
                    int loc=_tool->insertKey(_anchorTime);
                    _tool->setSegment(curve,loc,_anchorTime,_anchorValue,0.,0.,1.,1.,
                        animlib::AnimKeyframe::kTangentAuto,animlib::AnimKeyframe::kTangentAuto);
                }

            }else{
                if (!append && !toggle)
                    _tool->clearSelection();
                _dragHandler = new CESelBoxHandler(toggle);
                
            }
        }
    }
    else if (e->button() == Qt::MidButton) 
    {
        // pan/zoom
        switch (e->modifiers()) 
        {
            case 0:
                _dragHandler = new CEPanHandler;
            break;
            case Qt::ControlModifier: 
            {
                bool zoomX = 1; // TODO: zoomX = 0 if over y scale
                bool zoomY = 1; // TODO: zoomY = 0 if over x scale
                _dragHandler = new CEZoomHandler(zoomX, zoomY);
            }
            break;
            default: break;
        }
    }

    if (_dragHandler) 
    {
        // hide crosshairs if suppressed by handler
        if (!_dragHandler->showCrossHairs())
            hideCrossHairs();

        msg::list selections;
        std::vector<CEGraphKey*> graph_keys;
        int num_segments = 0;
        _tool->getSelection(selections);

        for (int i=0; i < selections.size(); i+=2)
        {
            //Values are in pairs. Curve/list of points
            int my_curve = selections[i].asint();
            msg::list my_segments = selections[i+1];
            for (int j=0; j <my_segments.size(); j++)
            {
                int my_segment = my_segments[j].asint(); 
                CEGraphKey* segment = getKey(my_curve, my_segment);
                graph_keys.push_back(segment);
                num_segments++;
            }
        }  

        if ( num_segments > 0 )
        {
            _dragHandler->setMultiDrag(graph_keys);
        }

        _dragHandler->setAnchor(this, x, y);

        // invoke handler
        _dragHandler->mouseDown();
    }
}


void CEGraphUI::mouseDoubleClickEvent(QMouseEvent* e)
{
    int x = e->pos().x();
    int y = height() - e->pos().y();

    if (e->button() == Qt::LeftButton) {
        if (_activeCurve >= 0) {
            int keystate = e->modifiers() & (Qt::ShiftModifier   | 
                                             Qt::ControlModifier |
                                             Qt::AltModifier     |
                                             Qt::MetaModifier);
            if (keystate == Qt::ShiftModifier) {
                // add curve to selection
                _tool->selectAddCurve(_activeCurve);
            }
            else if (keystate == Qt::ControlModifier) {
                // do nothing - control just toggles segments
            }
            else {
                // select curve
                _tool->selectCurve(_activeCurve);
                // add key
                int curve;
                if (_tool->getSelectedCurve(curve)==errSUCCESS) {
                    double _anchorTime = _view.vx(x);
                    double _anchorValue = _view.vy(y);
                    int loc=_tool->insertKey(_anchorTime);
                    _tool->setSegment(curve,loc,_anchorTime,_anchorValue,0.,0.,1.,1.,
                        animlib::AnimKeyframe::kTangentAuto,animlib::AnimKeyframe::kTangentAuto);
                }
            }
        }
    }
}


void CEGraphUI::mouseMoveEvent(QMouseEvent* e)
{
    int x = e->pos().x();
    int y = height() - e->pos().y();

    // show cross hairs unless suppressed by handler
    if (!_dragHandler || _dragHandler->showCrossHairs()) {
        moveCrossHairs(x, y);
    }

    if (!_dragHandler) {
        pickActive(x, y);
    } else {
        // update drag data
        _dragHandler->movePoint(x, y);
        
        // invoke handler
        _dragHandler->mouseMove();
    }
}


void CEGraphUI::mouseReleaseEvent(QMouseEvent* e)
{
    // nothing to do if we have no handler
    if (!_dragHandler) return;

    // ignore button release until all buttons are released
    // bsilva: NO! when the mouse button is up, we are done dragging.
//    if ((e->modifiers() & (Qt::ShiftModifier   | 
//                           Qt::ControlModifier |
//                           Qt::AltModifier     |
//                           Qt::MetaModifier)) != Qt::NoModifier) return;

    // invoke handler
    _dragHandler->mouseUp();

    // drag finished
    delete _dragHandler;
    _dragHandler = 0;

    // show cross-hairs again
    int x = e->pos().x();
    int y = height() - e->pos().y();
    moveCrossHairs(x, y);
}



void CEGraphUI::enterEvent(QEvent* e)
{
}


void CEGraphUI::leaveEvent(QEvent* e)
{
    hideCrossHairs();
}


void
CEGraphUI::invalidateCurve(int index)
{
    if (index >= 0 && index < (int) _curves.size()) {
        _curves[index]->invalidate();
        update(); // schedule repaint
    }
}


void
CEGraphUI::invalidateCurveList()
{
    _curveListValid = 0;
    update(); // schedule repaint
}


void
CEGraphUI::invalidateSelection()
{
    update(); // schedule repaint
}


void
CEGraphUI::invalidateView()
{
    _viewValid = 0;
    for (int i = 0; i < (int) _curves.size(); i++) {
        _curves[i]->invalidateView();
    }
    update(); // schedule repaint
}


void
CEGraphUI::invalidateTimeMode()
{
    update(); // schedule repaint
}


void CEGraphUI::updateView()
{
    double vx, vy, vw, vh;
    _tool->getView(vx, vy, vw, vh);
    _view.setView(vx, vy, vw, vh,
		  _scalesWidth, _scalesHeight,
		  width()-_scalesWidth, height()-_scalesHeight);
}


void CEGraphUI::updateCurves()
{
    // just clear and rebuild the curve list (optimize later if needed)
    clearCurves();

    CETool::AnimCurveList curves;
    _tool->getCurves(curves);
    _curves.reserve(curves.size());

    for (int i = 0; i < (int)curves.size(); i++) {
        _curves.push_back(new CEGraphCurve(this, curves[i], i));
    }
    _curveListValid = 1;
}











