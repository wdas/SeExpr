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
#ifndef _SeExprEdPopupDocumentation_h_
#define _SeExprEdPopupDocumentation_h_

#include <QtGui/QWidget>

class QLabel;
class SeExprEdPopupDocumentation:public QWidget
{
    Q_OBJECT;

public:
    QLabel* label;
    SeExprEdPopupDocumentation(QWidget* parent,const QPoint& cr,const QString& msg);

protected:
    void	mousePressEvent ( QMouseEvent * event );
};

#endif
