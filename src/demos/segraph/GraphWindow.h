/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
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

class QdSeShortEdit;

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
    bool animating;
    float time;
    std::vector<QdSeShortEdit*> _edits;
    std::vector<GrapherExpr*> _exprs;
    std::map<std::string,SimpleVar> variables;
};

#endif

