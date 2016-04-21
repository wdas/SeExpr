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
* @file CETool.h
* @brief Contains the declaration of class CETool.
*/ 

#ifndef CETool_h
#define CETool_h

#include <vector>
#include <set>
#include <DMsg/DMsg.h>
//#include <iTool.h>
//#include <iSgExpr.h>
//qt3 #include <qobject.h>
#include <QtCore/QObject>

#include <animlib/AnimCurve.h>
class QWidget;
class CEMainUI;

//****************************************************************************
/**
 * @class CETool
 * @brief Insert one-line brief description of class CETool here.
 *
 * Insert detailed description of class CETool definition here.
 *
 * @author  brentb
 *
 * @version <B>1.0 brentb 11/20/2001:</B> Initial version of class CETool.
 *
 */  

class CETool :
    public QObject
{   
    Q_OBJECT

public:
    /// Constructor
    CETool();

    /// Destructor
    virtual ~CETool();

    typedef std::vector<animlib::AnimCurve*> AnimCurveList;


    /// from iCurveEditor
    virtual err::Result newCurve(msg::cstr name);
    virtual err::Result addCurve(animlib::AnimCurve* curve);
    virtual err::Result removeCurve(int curveIndex);
    virtual err::Result removeSelectedCurves();
    virtual err::Result removeAllCurves();
    virtual err::Result findCurve(int& curveIndex, msg::cstr name);
    virtual err::Result getCurve(animlib::AnimCurve*& id, int curveIndex);
    virtual err::Result getCurves(AnimCurveList& curves);
    virtual err::Result getCurveNames(std::vector<std::string>& names);
    virtual err::Result clearSelection();
    virtual err::Result selectCurve(int curveIndex);
    virtual err::Result selectAddCurve(int curveIndex);
    virtual err::Result deselectCurve(int curveIndex);
    virtual err::Result selectSegment(int curveIndex, int segIndex);
    virtual err::Result selectAddSegment(int curveIndex, int segIndex);
    virtual err::Result deselectSegment(int curveIndex, int segIndex);
    virtual err::Result selectCurves(msg::list curveIndices);
    virtual err::Result selectSegments(int curveIndex, msg::list segIndices);
    virtual err::Result getSelection(msg::list& selections);
    virtual err::Result getSelectedCurve(int& curve);
    virtual err::Result getSelectedSegment(int& curve, int& seg);
    virtual err::Result isSegmentSelected(bool& selected,
					  int curveIndex, int segIndex);
    int  insertKey(double frame);
    void setKeyIn(int curve,int seg,double ang,double weight);
    void setKeyOut(int curve,int seg,double ang,double weight);
    void setSelectedInfinity(animlib::AnimCurve::infinityType preType,animlib::AnimCurve::infinityType postType);
    void setInfinity(int curve,animlib::AnimCurve::infinityType preType,animlib::AnimCurve::infinityType postType);
    void setSelectedSegmentFrame(double frame);
    void setSelectedSegmentValue(double value);
    void setSegmentFrame(double frame, int curve, int segment);
    void setSegmentValue(double value, int curve, int segment);
    void setWeighted(int curve,bool val);
    void setSelectedWeighted(bool val);
    void setLocked(int curve,int segment,bool val);
    void setSelectedLocked(bool val);

    void setSegmentStr(msg::cstr str);
    void setSelectedSegment(double frame,double value,
        double inAngle,double outAngle,double inWeight,double outWeight,
        animlib::AnimKeyframe::tangentType inType,
        animlib::AnimKeyframe::tangentType outType);

    void setSegment(int curve,int segment,double frame,double value,
        double inAngle,double outAngle,double inWeight,double outWeight,
        animlib::AnimKeyframe::tangentType inType,
        animlib::AnimKeyframe::tangentType outType);

    void deleteSegments();

    virtual err::Result setSegmentType(msg::cstr type, msg::list params);
    virtual err::Result changeSegmentType(msg::cstr newtype);
    virtual err::Result pan(double x, double y);
    virtual err::Result zoom(double zoomX, double zoomY);
    virtual err::Result zoomAroundPoint(double zoomX, double zoomY,
					double x, double y);
    virtual err::Result setView(double x, double y, double w, double h);
    virtual err::Result setCurveOffset(int curveIndex,
				       double yPan, double yZoom);
    virtual err::Result frameSelection();
    virtual err::Result getView(double& x, double& y, double& w, double& h);
    virtual err::Result getCurveOffset(double& yPan, double& yZoom, 
				       int curveIndex);
    virtual err::Result setTimeMode(int mode);
    virtual err::Result getTimeMode(int& mode);

    /// from iTool
    /// applies any configuration changes to the tool
    err::Result configureTool() { return errSUCCESS; }
    /// saves any configuration changes to the tool
    err::Result saveToolConfiguration() { return errSUCCESS; }
    /// the tool should map the GUI and return the associated QWidget
    err::Result map(QWidget*& uiComponent, QWidget* parent);
    // called after the gui for the tool has been mapped
    err::Result postMapNotify();
    /// called after the gui for the tool has been unmapped
    err::Result postUnmapNotify();

    /// public methods
    int numCurves() const { return (int) _curves.size(); }

    CEMainUI* ui(){return _ui;}

signals:
    void curveChanged(int index);
    void segChanged(int curve, int segment);
    void curveListChanged();
    void selectionChanged();
    void viewChanged();
    void timeModeChanged();

private:
    /// No definition by design, this is so accidental copying is prevented.
    CETool ( const CETool& );     
    /// No definition by design, this is so accidental assignment is prevented.
    CETool& operator=( const CETool& );  

    /// Called by msg code when tool is unregistered
    virtual void release() { delete this; }

    void addCurveInternal(animlib::AnimCurve* id, const char* name);
    int findCurve(animlib::AnimCurve* curve);
    int findCurve(const std::string& curveName);

    class CurveData;
    msg::RcvrId _rcvrid;
    CEMainUI* _ui;
    double _vx, _vy, _vw, _vh; // view rectangle
    int _timeMode;

    // per-curve data in curve list
    struct CurveData {
        animlib::AnimCurve* animCurve;
	std::string name;
	double ypan;
	double yzoom;
	bool selected;
	int numSegs;
	std::set<int> segsSelected;

        ~CurveData()
        {
            delete animCurve;
        }
    };
    typedef std::vector<CurveData*> CurveList;
    CurveList _curves;
    int _selectedCurve;

    // selection list cache
    msg::list _selList;
    bool _selListValid;
};

#endif //CETool_h
