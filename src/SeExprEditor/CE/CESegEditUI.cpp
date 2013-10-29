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
* @file CESegEditUI.C
* @brief Contains the Definition of class CESegEditUI.
*/

/* qt3
#include <qtooltip.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qvalidator.h>
*/

#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QValidator>
#include <QtGui/QKeyEvent>
#include <QtGui/QCheckBox>

#include "CETool.h"
#include "CESegEditUI.h"


MyTextEdit::MyTextEdit(QWidget* parent)
:QTextEdit(parent),editing(false)
{}

void MyTextEdit::keyPressEvent( QKeyEvent* e )
{
    // Accept expression
    if (e->key()==Qt::Key_Return || e->key()==Qt::Key_Enter){
        selectAll();
        finishEdit();
        return;
    }else if (e->key()==Qt::Key_Escape){
        setText(savedText);
        selectAll();
        finishEdit();
        return;
    }else if(e->key()==Qt::Key_Tab){
        QWidget::keyPressEvent(e);
        return;
    }else if(!editing){
        editing=true;
        setColor(true);
        savedText=toPlainText();
    }
    QTextEdit::keyPressEvent(e);
}

void MyTextEdit::setColor(bool editing)
{
}

void MyTextEdit::finishEdit()
{
    editing=false;
    setColor(false);
    emit editingFinished();
}

void MyTextEdit::focusOutEvent(QFocusEvent* e)
{
    //setTextCursor(QTextCursor());
    finishEdit();
    QTextCursor newCursor=textCursor();
    newCursor.clearSelection();
    setTextCursor(newCursor);
    setColor(false);
    QTextEdit::focusOutEvent(e);
}


namespace {

    const char* TypeToLabel(const char* type)
    {
        if      (!strcmp(type, ""))  return "expr";
        else if (!strcmp(type, "t")) return "tweak";

        return type;
    }

    const char* LabelToType(const char* label)
    {
        if      (!strcmp(label, "expr"))  return "";
        else if (!strcmp(label, "tweak")) return "t";

        return label;
    }
}


static const int EditMinWidth = 30;
static const int EditMaxWidth = 65;
static const int EditPad = 3;

static const int MaxParams = 4;


struct TangTypesEntry{
    const char* name;
    animlib::AnimKeyframe::tangentType type;
} TangTypes[] = {
    {"fixed",animlib::AnimKeyframe::kTangentFixed},
    {"auto",animlib::AnimKeyframe::kTangentAuto},
    //{"global",animlib::AnimKeyframe::kTangentGlobal},
    {"linear",animlib::AnimKeyframe::kTangentLinear},
    {"flat",animlib::AnimKeyframe::kTangentFlat},
    {"step",animlib::AnimKeyframe::kTangentStep},
    {"stepnext",animlib::AnimKeyframe::kTangentStepNext},
    //{"slow",animlib::AnimKeyframe::kTangentSlow},
    //{"fast",animlib::AnimKeyframe::kTangentFast},
    {"spline",animlib::AnimKeyframe::kTangentSmooth},
    //{"clamped",animlib::AnimKeyframe::kTangentClamped},
    //{"plateau",animlib::AnimKeyframe::kTangentPlateau},
    {0,animlib::AnimKeyframe::kTangentGlobal}
};
struct InfinityTypesEntry{
    const char* name;
    animlib::AnimCurve::infinityType type;
} InfinityTypes[] = {
    {"constant",animlib::AnimCurve::kInfinityConstant},
    {"linear",animlib::AnimCurve::kInfinityLinear},
    {"cycle",animlib::AnimCurve::kInfinityCycle},
    {"cycleRelative",animlib::AnimCurve::kInfinityCycleRelative},
    {"oscillate",animlib::AnimCurve::kInfinityOscillate},
    {0,animlib::AnimCurve::kInfinityConstant}
};





MyTextEdit*  CESegEditUI::addEdit(QGridLayout* grid,int row,int col,const QString& label)
{
    MyTextEdit* edit=new MyTextEdit(this);
    edit->setFixedWidth(EditMaxWidth*1.5);
    edit->setFixedHeight(25);
    edit->setContentsMargins(0,0,0,0);
    grid->addWidget(new QLabel(label), row, col);
    grid->addWidget(edit,row+1,col);
    connect(edit, SIGNAL(editingFinished()), SLOT(handleChanged()));
    
    return edit;
}

QComboBox*  CESegEditUI::addCombo(QGridLayout* grid,int row,int col,const QString& label)
{
    QComboBox* combo;
    combo=new QComboBox();
    auto it=&TangTypes[0];
    while(it->name != 0){
        combo->addItem(it->name);
        ++it;
    }
    grid->addWidget(new QLabel(label),row,col);
    grid->addWidget(combo,row+1,col);
    connect(combo,SIGNAL(currentIndexChanged(int)),SLOT(handleChanged()));
    return combo;
}

QComboBox*  CESegEditUI::addComboInfinity(QGridLayout* grid,int row,int col,const QString& label)
{
    QComboBox* combo;
    combo=new QComboBox();
    auto it=&InfinityTypes[0];
    while(it->name != 0){
        combo->addItem(it->name);
        ++it;
    }
    grid->addWidget(new QLabel(label),row,col);
    grid->addWidget(combo,row+1,col);
    connect(combo,SIGNAL(currentIndexChanged(int)),SLOT(handleInfinityChanged()));
    return combo;
}


/**
 * Currently the default constructor does nothing.
 */
CESegEditUI::CESegEditUI(QWidget* parent, CETool* tool) :
    QWidget(parent), _tool(tool),
    _valid(0), _updating(0)
{
    setObjectName("SegEdit");
    setContentsMargins(0,0,0,0);

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
    		                  QSizePolicy::Fixed));

    QGroupBox* box1=new QGroupBox("Full Curve");
    QGridLayout* grid1 = new QGridLayout;
    grid1->setContentsMargins(0,0,0,0);
    grid1->setMargin(5);
    grid1->setSpacing(0);
    box1->setLayout(grid1);

    QGroupBox* box2=new QGroupBox("Key Info");
    QGridLayout* grid2 = new QGridLayout;
    grid2->setContentsMargins(0,0,0,0);
    grid2->setMargin(5);
    grid2->setSpacing(0);
    box2->setLayout(grid2);

    _weighted=new QCheckBox("Weighted"); 
    connect(_weighted,SIGNAL(stateChanged(int)),SLOT(weightedChanged(int)));
    grid1->addWidget(_weighted,2,0);
    grid1->addWidget(new QLabel("(not\nrecommended)"),2,1);
    _preInfinity=addComboInfinity(grid1,0,0,"PreInf");
    _postInfinity=addComboInfinity(grid1,0,1,"PostInf");
    _frame=addEdit(grid2,0,0,"Frame");
    _value=addEdit(grid2,0,1,"Value");
    _inAngle=addEdit(grid2,2,0,"InAngle");
    _inWeight=addEdit(grid2,4,0,"InWeight");
    _outAngle=addEdit(grid2,2,1,"OutAngle");
    _outWeight=addEdit(grid2,4,1,"OutWeight");
    _inType=addCombo(grid2,6,0,"InType");
    _outType=addCombo(grid2,6,1,"OutType");
    _locked=new QCheckBox("Locked");
    connect(_locked,SIGNAL(stateChanged(int)),SLOT(lockedChanged(int)));
    grid2->addWidget(_locked,8,0);

    QVBoxLayout* lay=new QVBoxLayout;
    lay->setSpacing(0);
    lay->setMargin(0);
    setLayout(lay);
    lay->addWidget(box1);
    lay->addWidget(box2);


    connect(_tool, SIGNAL(curveChanged(int)),  SLOT(invalidateCurve(int)));
    connect(_tool, SIGNAL(curveListChanged()), SLOT(invalidate()));
    connect(_tool, SIGNAL(selectionChanged()), SLOT(invalidate()));

    doUpdate();
}

/**
 * Destructor.
 */
CESegEditUI::~CESegEditUI()
{
    //delete [] _params;
}


void
CESegEditUI::invalidateCurve(int index)
{
    if (index == _curveIndex)
        invalidate();
}


void
CESegEditUI::invalidate()
{
    if (_updating) return; // prevent circular update!
    if (_valid) {
        _valid = 0;
        update();
    }
}


void
CESegEditUI::paintEvent(QPaintEvent* event)
{
    if (!_valid) doUpdate();
    QWidget::paintEvent(event);
}


void
CESegEditUI::doUpdate()
{
    _updating = 1;
    // see if we have 1 and only 1 curve selected
    msg::list sel;
    _tool->getSelection(sel);
    if (sel.size() == 2)
    {
        // 1 curve - sel[0] is curveIndex, sel[1] is list of segs
        msg::list segs = sel[1];
        _curveIndex = sel[0];
        //msg::RcvrId curve;
        animlib::AnimCurve* curve;
        _tool->getCurve(curve, _curveIndex);
        _curve = curve; // bind to iSgExpr interface

        // edit seg if only 1 seg is selected (or there's only 1 seg)
        if (segs.size() == 1) {
            _segIndex = segs[0];
        }
        else
        {
            int numSegs = 0;
            numSegs=_curve->getNumKeys();
            if (numSegs == 1)
                _segIndex = 0;
            else _segIndex = -1;
        }
    }
    else
    {
        _curveIndex = -1;
        _segIndex = -1;
        _curve = 0; // msg::RcvrId();
    }

    // enable/disable edit controls as appropriate
    if (_curveIndex==-1 || _segIndex==-1) 
        disableControls();
    else 
        enableControls();

    if(_curveIndex==-1){
        _preInfinity->setEnabled(0);
        _postInfinity->setEnabled(0);
    }else{
        enableInfinityType(_preInfinity,_curve->getPreInfinity());
        _preInfinity->setEnabled(1);
        enableInfinityType(_postInfinity,_curve->getPostInfinity());
        _postInfinity->setEnabled(1);
        _weighted->setEnabled(1);
        _weighted->setChecked(_curve->isWeighted());
    }

    _valid = 1;
    _updating = 0;
}

namespace{
void disableEdit(MyTextEdit* w){w->setEnabled(0);w->setText("");}
}
void
CESegEditUI::disableControls()
{
    disableEdit(_frame);
    disableEdit(_value);
    disableEdit(_inAngle);
    disableEdit(_outAngle);
    disableEdit(_inWeight);
    disableEdit(_outWeight);
    _inType->setEnabled(0);
    _outType->setEnabled(0);

    //_type->setEnabled(_segIndex != 0);

    _frame->setText("");
    _value->setText("");

//qt3    _type->setCurrentText("");
    //_type->setEditText("");

    //for (int i = 0; i < MaxParams; i++)
    //    _params[i]->hide();
}

void CESegEditUI::enableTangType(QComboBox* combo,animlib::AnimKeyframe::tangentType type,bool enable)
{
    auto it=&TangTypes[0];
    int index=0;
    while(TangTypes[index].name != 0){
        if(TangTypes[index].type==type){
            combo->setCurrentIndex(index);
            combo->setEnabled(enable);
            return;
        }else{
            combo->setCurrentIndex(0); // fixed
            combo->setEnabled(enable);
        }
        index++;
    }
}

void CESegEditUI::enableInfinityType(QComboBox *combo,animlib::AnimCurve::infinityType type)
{
    auto it=&TangTypes[0];
    int index=0;
    while(InfinityTypes[index].name != 0){
        if(InfinityTypes[index].type==type){
            combo->setCurrentIndex(index);
            break;
        }
        index++;
    }
}

namespace{
    void setTextAndEnable(MyTextEdit* edit,double val,bool enableState=true){
        edit->setText(QString().sprintf("%g", val));
        edit->setEnabled(enableState);
        if(edit->hasFocus()) edit->selectAll();
    }
}
void
CESegEditUI::enableControls()
{
    double frame;
    msg::str expr;
    msg::str type;
    msg::list params;
    animlib::AnimKeyframe& key=*(_curve->getFirstKey()+_segIndex);

    double val=key.getValue();
    frame=key.getTime();
    
    //_curve.getSegment(frame, expr, type, params, _segIndex);
    bool preseg = false; // _segIndex == 0;


    setTextAndEnable(_frame,key.getTime());
    setTextAndEnable(_value,key.getValue());
    setTextAndEnable(_inAngle,key.getInAngle());
    setTextAndEnable(_outAngle,key.getOutAngle());
    setTextAndEnable(_inWeight,key.getInWeight(),_curve->isWeighted());
    setTextAndEnable(_outWeight,key.getOutWeight(),_curve->isWeighted());
    setTextAndEnable(_frame,key.getTime());
    _locked->setChecked(key.isLocked());
    enableTangType(_inType,key.getInTangentType(),!key.isLocked());
    enableTangType(_outType,key.getOutTangentType(),true);


}

void CESegEditUI::handleInfinityChanged(){
    if(_updating) return;
    if(_curveIndex<0) return;

    _tool->setSelectedInfinity(InfinityTypes[_preInfinity->currentIndex()].type,
        InfinityTypes[_postInfinity->currentIndex()].type);
}

void CESegEditUI::handleChanged()
{
    if(_updating) return;
    if(_curveIndex<0) return;
    bool ok=0;
    double frame = _frame->toPlainText().toDouble(&ok);
    if(!ok) {std::cerr<<"frame is invalid"<<std::endl;return;}
    double value = _value->toPlainText().toDouble(&ok);
    if(!ok) {std::cerr<<"value is invalid"<<std::endl;return;}
    double inAngle = _inAngle->toPlainText().toDouble(&ok);
    if(!ok) {std::cerr<<"inAngle is invalid"<<std::endl;return;}
    double outAngle = _outAngle->toPlainText().toDouble(&ok);
    if(!ok) {std::cerr<<"outAngle is invalid"<<std::endl;return;}
    double inWeight = _inWeight->toPlainText().toDouble(&ok);
    if(!ok) {std::cerr<<"inWeight is invalid"<<std::endl;return;}
    double outWeight = _outWeight->toPlainText().toDouble(&ok);
    if(!ok) {std::cerr<<"outWeight is invalid"<<std::endl;return;}
    bool locked=_locked->isChecked();
    animlib::AnimKeyframe::tangentType inType=TangTypes[_inType->currentIndex()].type;
    animlib::AnimKeyframe::tangentType outType=TangTypes[_outType->currentIndex()].type;
    if(locked) inType=outType;

    _tool->setSelectedSegment(frame,value,inAngle,outAngle,inWeight,outWeight,inType,outType);
}

void CESegEditUI::weightedChanged(int val)
{
    if(_updating) return;
    if(_curveIndex<0) return;

    _tool->setSelectedWeighted(val);
}

void CESegEditUI::lockedChanged(int val)
{
    if(_updating) return;
    if(_curveIndex<0) return;

    _tool->setSelectedLocked(val);
}
