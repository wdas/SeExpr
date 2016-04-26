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
