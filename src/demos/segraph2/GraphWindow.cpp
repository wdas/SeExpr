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
#include "GraphWindow.h"
#include <QtCore/QTimer>
#include <QtGui/QFormLayout>

GraphWindow::GraphWindow(QWidget* parent) : QFrame(parent), time(0) {
    //#########################################
    // Layout framework
    //#########################################
    QVBoxLayout* mainvbox = new QVBoxLayout();
    mainvbox->setMargin(0);
    setLayout(mainvbox);
    QHBoxLayout* layout = new QHBoxLayout();
    mainvbox->addLayout(layout);
    status = new QStatusBar();
    mainvbox->addWidget(status);

    // Function model
    functions = new Functions();

    //#########################################
    // Make our graph
    //#########################################
    graph = new Graph(functions, status);
    graph->setMinimumWidth(320);
    graph->setMinimumHeight(320);
    layout->addWidget(graph, 2);

    //#########################################
    // Make button bar and function list
    //#########################################
    QVBoxLayout* rightLayout = new QVBoxLayout();
    layout->addLayout(rightLayout, 1);

    // Make an entry for new functions
    edit = new QLineEdit();
    rightLayout->addWidget(new QLabel("f(x)="), 0);
    rightLayout->addWidget(edit, 0);

    // make a table view that is driven by functions
    table = new QTableView;
    table->setModel(functions);
    table->resizeColumnToContents(0);
    rightLayout->addWidget(table, 2);

    // Make buttons for computing roots
    rootbutton = new QPushButton("Find Root");
    rightLayout->addWidget(rootbutton, 0);
    connect(rootbutton, SIGNAL(clicked()), SLOT(findRootOrExtrema()));
    minbutton = new QPushButton("Find Min");
    rightLayout->addWidget(minbutton, 0);
    connect(minbutton, SIGNAL(clicked()), SLOT(findRootOrExtrema()));
    maxbutton = new QPushButton("Find Max");
    rightLayout->addWidget(maxbutton, 0);
    timeSlider = new QSlider(Qt::Horizontal);
    QFormLayout* formLayout = new QFormLayout();
    rightLayout->addLayout(formLayout);
    formLayout->addRow(new QLabel("t"), timeSlider);
    timeSlider->setMinimum(0);
    timeSlider->setMaximum(24);
    timeSlider->setValue(0);
    connect(maxbutton, SIGNAL(clicked()), SLOT(findRootOrExtrema()));

    // Connect edit box to add new function
    connect(edit, SIGNAL(editingFinished()), SLOT(addNewFunction()));
    // If data,selection,or layout changes in model then redraw graph
    connect(functions, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), graph, SLOT(redraw()));
    connect(functions, SIGNAL(layoutChanged()), graph, SLOT(redraw()));
    connect(table->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));

    QTimer::singleShot(1, this, SLOT(updateTime()));
}

GraphWindow::~GraphWindow() { delete functions; }

void GraphWindow::addNewFunction() {
    QString text = edit->text();
    if (text != QString("")) functions->addFunction(text);
    edit->setText("");
    status->showMessage("");
    table->repaint();
}

void GraphWindow::findRootOrExtrema() {
    Graph::OperationCode code = Graph::NONE;
    QObject* sender = QObject::sender();
    if (sender == rootbutton)
        code = Graph::FIND_ROOT;
    else if (sender == minbutton)
        code = Graph::FIND_MIN;
    else if (sender == maxbutton)
        code = Graph::FIND_MAX;

    std::vector<int> selected;
    functions->getSelected(selected);
    if (selected.size() != 1) {
        status->showMessage("You need to select exactly 1 function");
    } else {
        graph->scheduleRoot(code, selected[0]);
        if (sender == rootbutton)
            status->showMessage("Click on initial guess");
        else
            status->showMessage("Select solve region");
    }
}

void GraphWindow::selectionChanged(const QItemSelection& selected, const QItemSelection& unselected) {
    status->showMessage("");
    QModelIndexList removeList = unselected.indexes();
    for (int i = 0; i < removeList.size(); i++) {
        functions->setSelected(removeList[i].row(), false);
    }
    QModelIndexList addList = selected.indexes();
    for (int i = 0; i < addList.size(); i++) {
        functions->setSelected(addList[i].row(), true);
    }

    graph->repaint();
}

void GraphWindow::updateTime() {
    time += 1. / 24;
    if (time > 1) time = 0;
    functions->setVar("t", time);
    timeSlider->setValue(time * 24);
    graph->repaint();
    QTimer::singleShot(50., this, SLOT(updateTime()));
}
