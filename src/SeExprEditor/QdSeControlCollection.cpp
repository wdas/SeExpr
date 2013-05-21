/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*
* @file QdSeEditor.cpp
* @brief This provides an exression editor for SeExpr syntax with auto ui features
* @author  aselle
*/
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QPushButton>
#include <QtGui/QFormLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QColorDialog>
#include <QtGui/QLabel>
#include "QdSeEditor.h"
#include "QdSeHighlighter.h"
#include "QdSeCompletionModel.h"
#include "QdSeCurve.h"
#include "QdSeCCurve.h"
#include "QdSeControl.h"
#include "QdSeControlCollection.h"
#include "QdSeEditableExpression.h"
#include "QdSeEditable.h"


QdSeControlCollection::QdSeControlCollection(QWidget* parent,bool showAddButton)
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

QdSeControlCollection::~QdSeControlCollection()
{
    delete editableExpression;
}


    QdSeAddDialog::QdSeAddDialog(int& count,QWidget* parent)
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

        // Float
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

        verticalLayout->addWidget(tabWidget);

        QDialogButtonBox* buttonBox = new QDialogButtonBox();
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        
        verticalLayout->addWidget(buttonBox);
        
        
        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        
        tabWidget->setCurrentIndex(0);

    }

void QdSeAddDialog::colorChooseClicked()
{
    color = QColorDialog::getColor(color);
    if(color.isValid())
    {
        QPixmap colorPix(30,30);
        colorPix.fill(color);
        colorWidget->setIcon(QIcon(colorPix));
    }
}

void QdSeControlCollection::addControlDialog()
{
    QdSeAddDialog* dialog=new QdSeAddDialog(count,this);
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
                s=QString("%1 = animCurve(%2,\"constant\",\"constant\",0,\"%3\");")
                    .arg(dialog->variableName->text())
                    .arg(dialog->animCurveLookup->text())
                    .arg(dialog->animCurveLink->text());
                break;
            case 3:
                s=dialog->variableName->text()+" = "+dialog->intDefault->text()
                    +"; # "+dialog->intMin->text()+","+dialog->intMax->text()+"\n";
                break;
            case 4:
                s=QString("%1 = %2; # %3, %4\n").arg(dialog->variableName->text())
                    .arg(dialog->floatDefault->text())
                    .arg(atof(dialog->floatMin->text().toStdString().c_str()),0,'f',3)
                    .arg(atof(dialog->floatMax->text().toStdString().c_str()),0,'f',3);
                break;
            case 5:
                s=QString("%1 = [%2,%3,%4]; # %5, %6\n").arg(dialog->variableName->text())
                    .arg(dialog->vectorDefault0->text())
                    .arg(dialog->vectorDefault1->text())
                    .arg(dialog->vectorDefault2->text())
                    .arg(atof(dialog->vectorMin->text().toStdString().c_str()),0,'f',3)
                    .arg(atof(dialog->vectorMax->text().toStdString().c_str()),0,'f',3);
                break;
            case 6:
                s=QString("%1 = [%2,%3,%4];\n")
                    .arg(dialog->variableName->text())
                    .arg(dialog->color.redF())
                    .arg(dialog->color.greenF())
                    .arg(dialog->color.blueF());
                break;
            case 7:
                s=QString("\"%1\" #%2 %3\n")
                    .arg(dialog->stringDefaultWidget->text())
                    .arg(dialog->stringTypeWidget->currentText())
                    .arg(dialog->stringNameWidget->text());
                break;
        }
        emit insertString(s.toStdString());
    }
}

bool QdSeControlCollection::
rebuildControls(const QString& expressionText,std::vector<QString>& variables)
{
    // parse a new editable expression so  we can check if we need to make new controls
    QdSeEditableExpression* newEditable=new QdSeEditableExpression;
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
            QdSeEditable* editable=(*editableExpression)[i];
            QdSeControl* widget=0;
            //std::cerr<<"looking at control "<<editable->str()<<std::endl;
            // Create control "factory" (but since its only used here...)
            if(QdSeNumberEditable* x=dynamic_cast<QdSeNumberEditable*>(editable)) widget=new QdSeNumberControl(i,x);
            else if(QdSeVectorEditable* x=dynamic_cast<QdSeVectorEditable*>(editable)) widget=new QdSeVectorControl(i,x);
            else if(QdSeStringEditable* x=dynamic_cast<QdSeStringEditable*>(editable)) widget=new QdSeStringControl(i,x);
            else if(QdSeCurveEditable* x=dynamic_cast<QdSeCurveEditable*>(editable)) widget=new QdSeCurveControl(i,x);
            else if(QdSeCCurveEditable* x=dynamic_cast<QdSeCCurveEditable*>(editable)) widget=new QdSeCCurveControl(i,x);
            else if(QdSeAnimCurveEditable* x=dynamic_cast<QdSeAnimCurveEditable*>(editable)){
                widget=new QdSeAnimCurveControl(i,x);
            }
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

void QdSeControlCollection::showEditor(int idx)
{
    if (idx < 0 || idx >= (int)_controls.size())
        return;

    /* Right now we only launch the anim curve editor.
     * It would be better to launch them generically. */
    QdSeAnimCurveControl *control = dynamic_cast<QdSeAnimCurveControl *>(_controls[idx]);
    if (!control)
        return;

    control->editGraphClicked();
}

void QdSeControlCollection::
linkColorLink(int id)
{
    _linkedId=id;
    for(unsigned int i=0;i<_controls.size();i++){
        _controls[i]->linkDisconnect(_linkedId);
    }
}

void QdSeControlCollection::
linkColorEdited(int id,QColor color)
{
    if(id==_linkedId)
        emit linkColorOutput(color);
}


void QdSeControlCollection::
linkColorInput(QColor color)
{
    // TODO: fix
    if(_linkedId<0 || _linkedId>=(int)_controls.size()) return;
    _controls[_linkedId]->setColor(color);
}

void QdSeControlCollection::
updateText(const int id,QString& text)
{
    if(editableExpression)
        text=QString(editableExpression->getEditedExpr().c_str());
}

void QdSeControlCollection::
singleControlChanged(int id)
{
    emit controlChanged(id);
}

