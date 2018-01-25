/*
 Copyright Disney Enterprises, Inc.  All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License
 and the following modification to it: Section 6 Trademarks.
 deleted and replaced with:

 6. Trademarks. This License does not grant permission to use the
 trade names, trademarks, service marks, or product names of the
 Licensor and its affiliates, except as required for reproducing
 the content of the NOTICE file.

 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
*/
#ifndef _GraphWindow_h_
#define _GraphWindow_h_

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QCompleter>
#include <QStatusBar>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QMessageBox>

#include "Functions.h"
#include "Graph.h"

//! Main window that contains graph widget and function list
class GraphWindow : public QFrame {
    Q_OBJECT;

  public:
    Graph* graph;
    QLineEdit* edit;
    Functions* functions;
    QTableView* table;
    QPushButton* rootbutton, *minbutton, *maxbutton;
    QSlider* timeSlider;
    QStatusBar* status;
    GraphWindow(QWidget* parent = 0);
    ~GraphWindow();
  private
slots:
    //! Add new function when it is entered into the add box
    void addNewFunction();
    //! Start finding a numeric quantity after button clicked
    void findRootOrExtrema();
    //! Update the selection in the internal model
    void selectionChanged(const QItemSelection& selected, const QItemSelection& unselected);
    //! Update time
    void updateTime();

  private:
    float time;
};

#endif
