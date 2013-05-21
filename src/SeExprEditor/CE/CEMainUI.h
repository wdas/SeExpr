/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file CEMainUI.h
* @brief Contains the declaration of class CEMainUI.
*/

#ifndef CEMainUI_h
#define CEMainUI_h

// Base Class Includes
//qt3 #include <qvbox.h>
#include <QtGui/QWidget>

class QLabel;

class CETool;
class CEGraphUI;
class CECurveListUI;
class CESegEditUI;

//****************************************************************************
/**
 * @class CEMainUI
 * @brief Main GUI User interface class for the curve editor
 *
 *
 * @author  brentb
 *
 * @version <B>1.0 brentb 11/20/2001:</B> Initial version of class CEMainUI.
 *
 */  

class CEMainUI : public QWidget
{
public:

    /// Constructor
    CEMainUI(QWidget* parent, CETool* tool);
    /// Destructor
    virtual ~CEMainUI();

protected:

    /// No definition by design, so accidental copying is prevented.
    CEMainUI ( const CEMainUI& );     
    /// No definition by design, so accidental assignment is prevented.
    CEMainUI& operator=( const CEMainUI& );  
    
private:
    CETool*        _tool;
    CEGraphUI*     _graphUI;
    CECurveListUI* _curveListUI;
    CESegEditUI*   _segEditUI;
};

#endif //CEMainUI_h
