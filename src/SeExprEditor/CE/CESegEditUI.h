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
* @file CESegEditUI.h
* @brief Contains the declaration of class CESegEditUI.
*/

#ifndef CESegEditUI_h
#define CESegEditUI_h


//qt3 #include <qhbox.h>
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <animlib/AnimKeyframe.h>
#include <animlib/AnimCurve.h>
class CETool;
class QComboBox;
class QLineEdit;
class QGridLayout;
class QCheckBox;

namespace animlib{
class AnimCurve;
}

class MyTextEdit:public QTextEdit
{
    Q_OBJECT;


    bool editing;
    QString savedText;

public:
    MyTextEdit(QWidget* parent);

    virtual void keyPressEvent(QKeyEvent* e);
    virtual void focusOutEvent(QFocusEvent* e);

    void setColor(bool editing);
    void finishEdit();
signals:
    void editingFinished();


};


//****************************************************************************
/**
 * @class CESegEditUI
 * @brief Insert one-line brief description of class CESegEditUI here.
 *
 * Insert detailed description of class CESegEditUI definition here.
 *
 * @author  brentb
 *
 * @version <B>1.0 brentb 11/20/2001:</B> Initial version of class CESegEditUI.
 *
 */  

class CESegEditUI : public QWidget
{   
    Q_OBJECT

public:
    /// Constructor
    CESegEditUI(QWidget* parent, CETool* tool);
    /// Destructor
    virtual ~CESegEditUI();

private slots:
    /// Notification methods
    void invalidateCurve(int index);
    void invalidate();
    void handleChanged();
    void handleInfinityChanged();
    void weightedChanged(int val);
    void lockedChanged(int val);
    //void handle
    //void handleFrameChanged();
    //void handleValueChanged();
    //void handleTypeChanged();
    //void handleParamChanged();

private:
    /// No definition by design, so accidental copying is prevented.
    CESegEditUI(const CESegEditUI&);     
    /// No definition by design, so accidental assignment is prevented.
    CESegEditUI& operator=(const CESegEditUI&);  
    
    /// QWidget override
    virtual void paintEvent (QPaintEvent* event); 

    /// Update state from tool/curve data
    void doUpdate();
    void disableControls();
    void enableControls();

    MyTextEdit* addEdit(QGridLayout* layout,int row,int col,const QString& label);
    QComboBox* addCombo(QGridLayout* grid,int row,int col,const QString& label);
    QComboBox* addComboInfinity(QGridLayout* grid,int row,int col,const QString& label);
    void enableTangType(QComboBox* combo,animlib::AnimKeyframe::tangentType type,bool eanble);
    void enableInfinityType(QComboBox* combo,animlib::AnimCurve::infinityType type);

    CETool* _tool;
    QCheckBox* _weighted;
    QComboBox *_preInfinity,*_postInfinity;
    MyTextEdit *_frame;
    MyTextEdit *_value;
    MyTextEdit *_inAngle,*_outAngle;
    MyTextEdit *_inWeight,*_outWeight;
    QComboBox *_inType,*_outType;
    QCheckBox *_locked;
    //QComboBox* _type;
    //QLineEdit* _typeEntry;
    //QLineEdit** _params;
    bool _valid;
    bool _updating;
    int _curveIndex;
    animlib::AnimCurve* _curve;
    int _segIndex;
    int _numParams;
};

#endif //CESegEditUI_h
