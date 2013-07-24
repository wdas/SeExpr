/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#include <QtGui/QApplication>
#include "GraphWindow.h"
#include <cmath>
#include <cfloat>
#include <iostream>

int main(int argc,char *argv[])
{
    QApplication app(argc,argv);
    GraphWindow* graph=new GraphWindow;
    graph->show();
    app.exec();
    
    return 0;
}
