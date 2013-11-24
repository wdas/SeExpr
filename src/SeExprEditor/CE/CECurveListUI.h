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
* @file CECurveListUI.h
* @brief Contains the declaration of class CECurveListUI.
*/

#ifndef CECurveListUI_h
#define CECurveListUI_h

//qt3 #include <qwidget.h>
#include <QtGui/QWidget>

class CETool;
class QListWidget;


//****************************************************************************
/**
 * @class CECurveListUI
 * @brief User interface for the Curve Editor's Curve list UI
 *
 * Insert detailed description of class CECurveListUI definition here.
 *
 * @author  brentb
 *
 * @version <B>1.0 brentb 11/20/2001:</B> Initial version of class CECurveListUI.
 *
 */  

class CECurveListUI : public QWidget
{   
    Q_OBJECT
    
public:
    /// Constructor
    CECurveListUI(QWidget* parent, CETool* tool);
    /// Destructor
    virtual ~CECurveListUI();

    /// override of minimumSizeHint
    virtual QSize minimumSizeHint () const { return QSize(0,0); }

private slots:

    void invalidateCurveList();
    void invalidateSelection();
    void handleSelectionChanged();

private:
    /// No definition by design, so accidental copying is prevented.
    CECurveListUI ( const CECurveListUI& );     
    /// No definition by design, so accidental assignment is prevented.
    CECurveListUI& operator=( const CECurveListUI& );  
    /// QWidget override
    virtual void paintEvent (QPaintEvent* event); 
    virtual void showEvent (QShowEvent* event);

    /// Update state from tool/curve data
    void doUpdate();

    CETool* _tool;
    QListWidget* _list;
    bool _listValid;
    bool _selValid;
    bool _updating;
};

#endif //CECurveListUI_h
