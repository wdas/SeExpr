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
* @file SeExprEditor.cpp
* @brief This provides an expression editor for SeExpr syntax with auto ui features
* @author  aselle
*/
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QFormLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QColorDialog>
#include <QtGui/QLabel>
#include "SeExprEditor.h"
#include "SeExprEdHighlighter.h"
#include "SeExprEdCompletionModel.h"
#include "SeExprEdCurve.h"
#include "SeExprEdColorCurve.h"
#include "SeExprEdControl.h"
#include "SeExprEdControlCollection.h"
#include "SeExprEdEditableExpression.h"
#include "SeExprEdEditable.h"


SeExprEdControlCollection::SeExprEdControlCollection(QWidget* parent,bool showAddButton)
    :QWidget(parent),count(0),showAddButton(showAddButton),editableExpression(0)
{
    controlLayout=new QVBoxLayout();
    controlLayout->setMargin(0);
    controlLayout->setSpacing(0);
    controlLayout->insertStretch(-1,100);

    if(showAddButton){
        QPushButton* button=new QPushButton("Add Widget");
        button->setFocusPolicy(Qt::NoFocus);
        QHBoxLayout* buttonLayout=new QHBoxLayout();
        buttonLayout->insertStretch(-1,100);
        buttonLayout->addWidget(button,0);
        controlLayout->addLayout(buttonLayout);
        connect(button, SIGNAL(clicked()), SLOT(addControlDialog()));
    }
    setLayout(controlLayout);
}

SeExprEdControlCollection::~SeExprEdControlCollection()
{
    delete editableExpression;
}


    SeExprEdAddDialog::SeExprEdAddDialog(int& count,QWidget* parent)
        :QDialog(parent)
    {
        QVBoxLayout *verticalLayout;
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(3);
        verticalLayout->setMargin(3);
        setLayout(verticalLayout);
        QHBoxLayout *horizontalLayout = new QHBoxLayout();

        horizontalLayout->addWidget(new QLabel("Variable"));
        // TODO would be nice to unique this over multiple sessions
        variableName = new QLineEdit(QString("$var%1").arg(count++));

        horizontalLayout->addWidget(variableName);
        verticalLayout->addLayout(horizontalLayout);

        tabWidget = new QTabWidget();

        // Curve
        {
            QWidget* curveTab = new QWidget();
            QFormLayout* curveLayout = new QFormLayout(curveTab);
            curveLayout->setWidget(0,QFormLayout::LabelRole,new QLabel("Lookup"));
            curveLookup=new QLineEdit("$u");
            curveLayout->setWidget(0,QFormLayout::FieldRole,curveLookup);
            tabWidget->addTab(curveTab, QString("Curve"));
        }

        // Color Curve
        {
            QWidget* colorCurveTab = new QWidget();
            QFormLayout* colorCurveLayout = new QFormLayout(colorCurveTab);
            colorCurveLayout->setWidget(0,QFormLayout::LabelRole,new QLabel("Lookup"));
            colorCurveLookup=new QLineEdit("$u");
            colorCurveLayout->setWidget(0,QFormLayout::FieldRole,colorCurveLookup);
            tabWidget->addTab(colorCurveTab, QString("Color Curve"));
        }

        // Integer
        {
            QWidget* intTab = new QWidget();
            QFormLayout* intFormLayout = new QFormLayout(intTab);
            intFormLayout->setWidget(0, QFormLayout::LabelRole, new QLabel("Default"));
            intFormLayout->setWidget(1, QFormLayout::LabelRole, new QLabel("Min"));
            intFormLayout->setWidget(2, QFormLayout::LabelRole, new QLabel("Max"));
            intDefault = new QLineEdit("0");
            intFormLayout->setWidget(0, QFormLayout::FieldRole, intDefault);
            intMin = new QLineEdit("0");
            intFormLayout->setWidget(1, QFormLayout::FieldRole, intMin);
            intMax = new QLineEdit("10");
            intFormLayout->setWidget(2, QFormLayout::FieldRole, intMax);
            tabWidget->addTab(intTab, QString("Int"));
        }

        // Float
        {
            QWidget* floatTab = new QWidget();
            QFormLayout* floatFormLayout = new QFormLayout(floatTab);
            floatFormLayout->setWidget(0, QFormLayout::LabelRole, new QLabel("Default"));
            floatFormLayout->setWidget(1, QFormLayout::LabelRole, new QLabel("Min"));
            floatFormLayout->setWidget(2, QFormLayout::LabelRole, new QLabel("Max"));
            floatDefault = new QLineEdit("0");
            floatFormLayout->setWidget(0, QFormLayout::FieldRole, floatDefault);
            floatMin = new QLineEdit("0");
            floatFormLayout->setWidget(1, QFormLayout::FieldRole, floatMin);
            floatMax = new QLineEdit("1");
            floatFormLayout->setWidget(2, QFormLayout::FieldRole, floatMax);

            tabWidget->addTab(floatTab, QString("Float"));
        }

        // Vector
        {
            QWidget* vectorTab = new QWidget();
            QFormLayout* vectorFormLayout = new QFormLayout(vectorTab);
            vectorFormLayout->setWidget(0, QFormLayout::LabelRole, new QLabel("Default"));
            vectorFormLayout->setWidget(1, QFormLayout::LabelRole, new QLabel("Min"));
            vectorFormLayout->setWidget(2, QFormLayout::LabelRole, new QLabel("Max"));
            vectorDefault0 = new QLineEdit("0");
            vectorDefault1 = new QLineEdit("0");
            vectorDefault2 = new QLineEdit("0");
            QHBoxLayout* compLayout=new QHBoxLayout();
            compLayout->addWidget(vectorDefault0);
            compLayout->addWidget(vectorDefault1);
            compLayout->addWidget(vectorDefault2);
            vectorFormLayout->setLayout(0, QFormLayout::FieldRole, compLayout);
            vectorMin = new QLineEdit("0");
            vectorFormLayout->setWidget(1, QFormLayout::FieldRole, vectorMin);
            vectorMax = new QLineEdit("1");
            vectorFormLayout->setWidget(2, QFormLayout::FieldRole, vectorMax);

            tabWidget->addTab(vectorTab, QString("Vector"));
        }

        // Color
        {
            QWidget* colorTab = new QWidget();
            QFormLayout* colorLayout = new QFormLayout(colorTab);
            colorWidget=new QPushButton();
            colorWidget->setFixedWidth(30);
            colorWidget->setFixedWidth(30);
            colorLayout->setWidget(0,QFormLayout::LabelRole,new QLabel("Color"));
            colorLayout->setWidget(0,QFormLayout::FieldRole,colorWidget);
            color=Qt::red;
            QPixmap colorPix(30,30);
            colorPix.fill(color);
            colorWidget->setIcon(QIcon(colorPix));
            tabWidget->addTab(colorTab, QString("Color"));

            connect(colorWidget,SIGNAL(clicked()),this,SLOT(colorChooseClicked()));
        }

        // Color Swatch
        {
            QWidget* swatchTab = new QWidget();
            QFormLayout* swatchLayout = new QFormLayout(swatchTab);
            swatchLookup=new QLineEdit("$u");
            swatchLayout->setWidget(0,QFormLayout::LabelRole,new QLabel("Lookup"));
            swatchLayout->setWidget(0,QFormLayout::FieldRole,swatchLookup);
            rainbowPaletteBtn = new QRadioButton("Rainbow");
            rainbowPaletteBtn->setChecked(true);
            grayPaletteBtn = new QRadioButton("Shades of Gray");
            swatchLayout->setWidget(1,QFormLayout::LabelRole,new QLabel("Colors"));
            swatchLayout->setWidget(1,QFormLayout::FieldRole,rainbowPaletteBtn);
            swatchLayout->setWidget(2,QFormLayout::LabelRole,new QLabel(""));
            swatchLayout->setWidget(2,QFormLayout::FieldRole,grayPaletteBtn);
            tabWidget->addTab(swatchTab, QString("Swatch"));
        }

        // String literal
        {
            QWidget* stringTab = new QWidget();
            QFormLayout* stringLayout = new QFormLayout(stringTab);
            stringTypeWidget=new QComboBox();
            stringTypeWidget->addItem("string");
            stringTypeWidget->addItem("file");
            stringTypeWidget->addItem("directory");
            stringDefaultWidget=new QLineEdit();
            stringNameWidget=new QLineEdit("str1");

            stringLayout->setWidget(0,QFormLayout::LabelRole,new QLabel("String Name"));
            stringLayout->setWidget(0,QFormLayout::FieldRole,stringNameWidget);
            stringLayout->setWidget(1,QFormLayout::LabelRole,new QLabel("String Type"));
            stringLayout->setWidget(1,QFormLayout::FieldRole,stringTypeWidget);
            stringLayout->setWidget(2,QFormLayout::LabelRole,new QLabel("String Default"));
            stringLayout->setWidget(3,QFormLayout::FieldRole,stringDefaultWidget);

            tabWidget->addTab(stringTab, QString("String"));
        }

#ifdef SEEXPR_USE_ANIMLIB
        // Anim Curve
        {
            QWidget* curveTab = new QWidget();
            QFormLayout* curveLayout = new QFormLayout(curveTab);
            curveLayout->setWidget(0,QFormLayout::LabelRole,new QLabel("Lookup"));
            curveLayout->setWidget(1,QFormLayout::LabelRole,new QLabel("Link"));
            animCurveLookup=new QLineEdit("$frame");
            animCurveLink=new QLineEdit("");
            curveLayout->setWidget(0,QFormLayout::FieldRole,animCurveLookup);
            curveLayout->setWidget(1,QFormLayout::FieldRole,animCurveLink);
            tabWidget->addTab(curveTab, QString("AnimCurve"));
        }
#endif

        verticalLayout->addWidget(tabWidget);

        QDialogButtonBox* buttonBox = new QDialogButtonBox();
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

        tabWidget->setCurrentIndex(0);

    }

void SeExprEdAddDialog::colorChooseClicked()
{
    color = QColorDialog::getColor(color);
    if(color.isValid())
    {
        QPixmap colorPix(30,30);
        colorPix.fill(color);
        ((QPushButton *)sender())->setIcon(QIcon(colorPix));
    }
}

const char* SeExprEdAddDialog::initSwatch()
{
    if(rainbowPaletteBtn->isChecked())
        return( "[1,0,0],[1,.6,0],[1,1,0],[0,1,0],[0,1,1],[0,0,1],[.6,.1,.6],[1,0,1],[1,1,1],[0,0,0]");
    else if (grayPaletteBtn->isChecked())
        return( "[1,1,1],[.9,.9,.9],[.8,.8,.8],[.7,.7,.7],[.6,.6,.6],[.5,.5,.5],[.4,.4,.4],[.3,.3,.3],[.2,.2,.2],[0,0,0]");
    else
        return( "[1,1,1],[.5,.5,.5],[0,0,0]");
}

void SeExprEdControlCollection::addControlDialog()
{
    SeExprEdAddDialog* dialog=new SeExprEdAddDialog(count,this);
    if(dialog->exec()){
        QString s;
        switch(dialog->tabWidget->currentIndex()){
            case 0:
                s=QString("%1 = curve(%2,0,0,4,1,1,4);\n")
                    .arg(dialog->variableName->text())
                    .arg(dialog->curveLookup->text());
                break;
            case 1:
                s=QString("%1 = ccurve(%2,0,[0,0,0],4,1,[1,1,1],4);\n")
                    .arg(dialog->variableName->text())
                    .arg(dialog->colorCurveLookup->text());
                break;
            case 2:
                s=dialog->variableName->text()+" = "+dialog->intDefault->text()
                    +"; # "+dialog->intMin->text()+","+dialog->intMax->text()+"\n";
                break;
            case 3:
                s=QString("%1 = %2; # %3, %4\n").arg(dialog->variableName->text())
                    .arg(dialog->floatDefault->text())
                    .arg(atof(dialog->floatMin->text().toStdString().c_str()),0,'f',3)
                    .arg(atof(dialog->floatMax->text().toStdString().c_str()),0,'f',3);
                break;
            case 4:
                s=QString("%1 = [%2,%3,%4]; # %5, %6\n").arg(dialog->variableName->text())
                    .arg(dialog->vectorDefault0->text())
                    .arg(dialog->vectorDefault1->text())
                    .arg(dialog->vectorDefault2->text())
                    .arg(atof(dialog->vectorMin->text().toStdString().c_str()),0,'f',3)
                    .arg(atof(dialog->vectorMax->text().toStdString().c_str()),0,'f',3);
                break;
            case 5:
                s=QString("%1 = [%2,%3,%4];\n")
                    .arg(dialog->variableName->text())
                    .arg(dialog->color.redF())
                    .arg(dialog->color.greenF())
                    .arg(dialog->color.blueF());
                break;
            case 6:
                s=QString("%1 = swatch(%2,%3);\n")
                    .arg(dialog->variableName->text())
                    .arg(dialog->swatchLookup->text())
                    .arg(dialog->initSwatch());
                break;
            case 7:
                s=QString("\"%1\" #%2 %3\n")
                    .arg(dialog->stringDefaultWidget->text())
                    .arg(dialog->stringTypeWidget->currentText())
                    .arg(dialog->stringNameWidget->text());
                break;
#ifdef SEEXPR_USE_ANIMLIB
            case 8:
                s=QString("%1 = animCurve(%2,\"constant\",\"constant\",0,\"%3\");")
                    .arg(dialog->variableName->text())
                    .arg(dialog->animCurveLookup->text())
                    .arg(dialog->animCurveLink->text());
                break;
#endif
        }
        emit insertString(s.toStdString());
    }
}

bool SeExprEdControlCollection::
rebuildControls(const QString& expressionText,std::vector<QString>& variables)
{
    // parse a new editable expression so  we can check if we need to make new controls
    SeExprEdEditableExpression* newEditable=new SeExprEdEditableExpression;
    newEditable->setExpr(expressionText.toStdString());

    // check for new variables

    bool newVariables=true;
    if(editableExpression && editableExpression->getVariables()==newEditable->getVariables()) newVariables=false;
    if(newVariables){
        const std::vector<std::string>& vars=newEditable->getVariables();
        variables.clear();
        for(size_t k=0;k<vars.size();k++){
            variables.push_back(("$"+vars[k]).c_str());
        }
    }

    if (newEditable->size()==0 && !editableExpression) return false;

    if(editableExpression && editableExpression->controlsMatch(*newEditable)){
        // controls match so we only need to update positions (i.e. if the user typed and shifted some controls)
        editableExpression->updateString(*newEditable);
        delete newEditable;
    }else{
        // controls did not match

        // delete old controls
        for (unsigned int i = 0; i < _controls.size(); i++){
            controlLayout->removeWidget(_controls[i]);
            delete _controls[i];
        }
        _linkedId=-1;
        _controls.clear();

        // swap to new editable expression
        delete editableExpression;
        editableExpression=newEditable;

        //build new controls
        for(size_t i=0;i<editableExpression->size();i++){
            SeExprEdEditable* editable=(*editableExpression)[i];
            SeExprEdControl* widget=0;
            // Create control "factory" (but since its only used here...)
            if(SeExprEdNumberEditable* x=dynamic_cast<SeExprEdNumberEditable*>(editable)) widget=new SeExprEdNumberControl(i,x);
            else if(SeExprEdVectorEditable* x=dynamic_cast<SeExprEdVectorEditable*>(editable)) widget=new SeExprEdVectorControl(i,x);
            else if(SeExprEdStringEditable* x=dynamic_cast<SeExprEdStringEditable*>(editable)) widget=new SeExprEdStringControl(i,x);
            else if(SeExprEdCurveEditable* x=dynamic_cast<SeExprEdCurveEditable*>(editable)) widget=new SeExprEdCurveControl(i,x);
            else if(SeExprEdColorCurveEditable* x=dynamic_cast<SeExprEdColorCurveEditable*>(editable)) widget=new SeExprEdCCurveControl(i,x);
            else if(SeExprEdAnimCurveEditable* x=dynamic_cast<SeExprEdAnimCurveEditable*>(editable)){
                widget=new SeExprEdAnimCurveControl(i,x);
            }
            else if(SeExprEdColorSwatchEditable* x=dynamic_cast<SeExprEdColorSwatchEditable*>(editable))
                widget=new SeExprEdColorSwatchControl(i,x);
            else{
                std::cerr<<"SeExpr editor logic error, cannot find a widget for the given editable"<<std::endl;
            }
            if(widget){
                // successfully made widget
                int insertPoint=controlLayout->count()-1;
                if(showAddButton) insertPoint--;
                controlLayout->insertWidget(insertPoint,widget);
                _controls.push_back(widget);
                connect(widget, SIGNAL(controlChanged(int)), SLOT(singleControlChanged(int)));
                connect(widget, SIGNAL(linkColorEdited(int,QColor)), SLOT(linkColorEdited(int,QColor)));
                connect(widget, SIGNAL(linkColorLink(int)), SLOT(linkColorLink(int)));
            }else{
                std::cerr<<"Expr Editor Logic ERROR did not make widget"<<std::endl;
            }
        }
    }
    return newVariables;
}

void SeExprEdControlCollection::showEditor(int idx)
{
    if (idx < 0 || idx >= (int)_controls.size())
        return;

    /* Right now we only launch the anim curve editor.
     * It would be better to launch them generically. */
    SeExprEdAnimCurveControl *control = dynamic_cast<SeExprEdAnimCurveControl *>(_controls[idx]);
    if (!control)
        return;

    control->editGraphClicked();
}

void SeExprEdControlCollection::
linkColorLink(int id)
{
    _linkedId=id;
    for(unsigned int i=0;i<_controls.size();i++){
        _controls[i]->linkDisconnect(_linkedId);
    }
}

void SeExprEdControlCollection::
linkColorEdited(int id,QColor color)
{
    if(id==_linkedId)
        emit linkColorOutput(color);
}


void SeExprEdControlCollection::
linkColorInput(QColor color)
{
    // TODO: fix
    if(_linkedId<0 || _linkedId>=(int)_controls.size()) return;
    _controls[_linkedId]->setColor(color);
}

void SeExprEdControlCollection::
updateText(const int id,QString& text)
{
    Q_UNUSED(id);
    if(editableExpression)
        text=QString(editableExpression->getEditedExpr().c_str());
}

void SeExprEdControlCollection::
singleControlChanged(int id)
{
    emit controlChanged(id);
}

