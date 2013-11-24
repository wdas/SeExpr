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
*/
#include "GraphWindow.h"
#include <QtCore/QTimer>
#include <QtGui/QFormLayout>
#include <SeExprEdShortEdit.h>

GraphWindow::
GraphWindow(QWidget* parent)
    :QFrame(parent),time(0),animating(false)
{
    variables["t"].val=0.;

    //#########################################
    // Layout framework
    //#########################################
    QVBoxLayout* mainvbox=new QVBoxLayout();
    mainvbox->setMargin(0);
    setLayout(mainvbox);
    QHBoxLayout* layout=new QHBoxLayout();
    mainvbox->addLayout(layout);
    status=new QStatusBar();
    mainvbox->addWidget(status);


    //#########################################
    // Make our graph
    //#########################################
    graph=new Graph(status,_exprs);
    graph->setMinimumWidth(640);
    graph->setMinimumHeight(480);
    layout->addWidget(graph,2);

    //#########################################
    // Make button bar and function list
    //#########################################
    QVBoxLayout* rightLayout=new QVBoxLayout();

    layout->addLayout(rightLayout,1);
    
    // Make an entry for new functions

    rightLayout->setMargin(0);
    for(int i=0;i<5;i++){
        SeExprEdShortEdit* shortEdit=new SeExprEdShortEdit(0);
        shortEdit->setMinimumWidth(512);
        rightLayout->addWidget(shortEdit);
        QObject::connect(shortEdit,SIGNAL(exprChanged()),this,SLOT(exprsEdited()));
        _edits.push_back(shortEdit);
        _exprs.push_back(new GrapherExpr("",variables));
    }

    // set default exprs

    _edits[0]->setExpressionString("a=0.34483; #-10.0,10.0\nb=0.41379; #-10.0,10.0\nc=-0.34482; #-10.0,10.0\na*x^2+b*x+c");

    _exprs[1]->setExpr("frequency=8.32724; # 0.1, 10.0\
amp = -4; # 0.0, 10.0\
fbm(frequency*x)*amp\
");

    _exprs[2]->setExpr("""frequency=8.32724; # 0.1, 10.0\
amp = 1; # 0.0, 10.0\
sin(frequency*x)*amp-5\
");

    rightLayout->addStretch(1);

    // make a table view that is driven by functions

    // Make buttons for computing roots
    rootbutton=new QPushButton("Find Root");
    rightLayout->addWidget(rootbutton,0);
    connect(rootbutton,SIGNAL(clicked()),SLOT(findRootOrExtrema()));
    minbutton=new QPushButton("Find Min");
    rightLayout->addWidget(minbutton,0);
    connect(minbutton,SIGNAL(clicked()),SLOT(findRootOrExtrema()));
    maxbutton=new QPushButton("Find Max");
    rightLayout->addWidget(maxbutton,0);
    timeSlider=new QSlider(Qt::Horizontal);
    QFormLayout* formLayout=new QFormLayout();
    rightLayout->addLayout(formLayout);
    formLayout->addRow(new QLabel("t"),timeSlider);
    timeSlider->setMinimum(0);
    timeSlider->setMaximum(24);
    timeSlider->setValue(0);
    connect(maxbutton,SIGNAL(clicked()),SLOT(findRootOrExtrema()));

    // Connect edit box to add new function
    // If data,selection,or layout changes in model then redraw graph
    QTimer::singleShot(1,this,SLOT(updateTime()));

}

GraphWindow::
~GraphWindow()
{
    for(unsigned int i=0;i<_exprs.size();i++) delete _exprs[i];
}

void GraphWindow::
findRootOrExtrema()
{
    Graph::OperationCode code=Graph::NONE;
    QObject* sender=QObject::sender();
    if(sender==rootbutton) code=Graph::FIND_ROOT;
    else if(sender==minbutton) code=Graph::FIND_MIN;
    else if(sender==maxbutton) code=Graph::FIND_MAX;

    std::vector<int> selected;
    if(selected.size() != 1){
        status->showMessage("You need to select exactly 1 function");
    }else{
        graph->scheduleRoot(code,selected[0]);
        if(sender==rootbutton) status->showMessage("Click on initial guess");
        else status->showMessage("Select solve region");
    }
}

void GraphWindow::
updateTime()
{
    if(animating){
        time+=1./24;
        if(time>1) time=0;
        timeSlider->setValue(time*24);
        variables["t"].val=time;
        graph->repaint();
    }
    QTimer::singleShot(50.,this,SLOT(updateTime()));
}

void GraphWindow::
exprsEdited()
{
    for(size_t i=0;i<_edits.size();i++){
        SeExprEdShortEdit& edit=*_edits[i];
        _exprs[i]->setExpr(edit.getExpressionString());
        _exprs[i]->isValid();
    }
    graph->repaint();
}
