/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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
