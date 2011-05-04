/*
 SEEXPR SOFTWARE
 Copyright 2011 Disney Enterprises, Inc. All rights reserved
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
 Studios" or the names of its contributors may NOT be used to
 endorse or promote products derived from this software without
 specific prior written permission from Walt Disney Pictures.
 
 Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
 IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#include "GraphWindow.h"
#include <QtCore/QTimer>
#include <QtGui/QFormLayout>

GraphWindow::
GraphWindow(QWidget* parent)
    :QFrame(parent),time(0)
{
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

    // Function model
    functions=new Functions();

    //#########################################
    // Make our graph
    //#########################################
    graph=new Graph(functions,status);
    graph->setMinimumWidth(320);
    graph->setMinimumHeight(320);
    layout->addWidget(graph,2);

    //#########################################
    // Make button bar and function list
    //#########################################
    QVBoxLayout* rightLayout=new QVBoxLayout();
    layout->addLayout(rightLayout,1);
    
    // Make an entry for new functions
    edit=new QLineEdit();
    rightLayout->addWidget(new QLabel("f(x)="),0);
    rightLayout->addWidget(edit,0);

    // make a table view that is driven by functions
    table=new QTableView;
    table->setModel(functions);
    table->resizeColumnToContents(0);
    rightLayout->addWidget(table,2);

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
    connect(edit,SIGNAL(editingFinished()),SLOT(addNewFunction()));
    // If data,selection,or layout changes in model then redraw graph
    connect(functions,SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),graph,SLOT(redraw()));
    connect(functions,SIGNAL(layoutChanged()),graph,SLOT(redraw()));
    connect(table->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
        SLOT(selectionChanged(const QItemSelection&,const QItemSelection&)));

    QTimer::singleShot(1,this,SLOT(updateTime()));

}

GraphWindow::
~GraphWindow()
{
    delete functions;
}

void GraphWindow::
addNewFunction()
{
    QString text=edit->text();
    if(text!=QString(""))
        functions->addFunction(text);
    edit->setText("");
    status->showMessage("");
    table->repaint();
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
    functions->getSelected(selected);
    if(selected.size() != 1){
        status->showMessage("You need to select exactly 1 function");
    }else{
        graph->scheduleRoot(code,selected[0]);
        if(sender==rootbutton) status->showMessage("Click on initial guess");
        else status->showMessage("Select solve region");
    }
}

void GraphWindow::
selectionChanged(const QItemSelection& selected,const QItemSelection& unselected)
{
    status->showMessage("");
    QModelIndexList removeList=unselected.indexes();
    for(int i=0;i<removeList.size();i++){
        functions->setSelected(removeList[i].row(),false);
    }
    QModelIndexList addList=selected.indexes();
    for(int i=0;i<addList.size();i++){
        functions->setSelected(addList[i].row(),true);
    }

    graph->repaint();
}

void GraphWindow::
updateTime()
{
    time+=1./24;
    if(time>1) time=0;
    functions->setVar("t",time);
    timeSlider->setValue(time*24);
    graph->repaint();
    QTimer::singleShot(50.,this,SLOT(updateTime()));
}
