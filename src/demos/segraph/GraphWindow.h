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
#ifndef _GraphWindow_h_
#define _GraphWindow_h_

#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QCompleter>
#include <QtGui/QStatusBar>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTableView>
#include <QtGui/QMessageBox>

#include "Graph.h"

#include "GrapherExpr.h"

class SeExprEdShortEdit;

//! Main window that contains graph widget and function list
class GraphWindow:public QFrame
{
    Q_OBJECT;
public:
    Graph* graph;
    QPushButton *rootbutton,*minbutton,*maxbutton;
    QSlider *timeSlider;
    QStatusBar* status;
    GraphWindow(QWidget* parent=0);
    ~GraphWindow();
private slots:
    //! Start finding a numeric quantity after button clicked
    void findRootOrExtrema();
    //! Update time
    void updateTime();
    //! Exprs edits
    void exprsEdited();
private:
    float time;
    bool animating;
    std::vector<SeExprEdShortEdit*> _edits;
    std::vector<GrapherExpr*> _exprs;
    std::map<std::string,SimpleVar> variables;
};

#endif

