/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#include "SeExprEdPopupDocumentation.h"
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>

SeExprEdPopupDocumentation::SeExprEdPopupDocumentation(QWidget* parent,const QPoint& placecr,const QString& msg)
{
    Q_UNUSED(parent);
    label=new QLabel(msg);
    QHBoxLayout* layout=new QHBoxLayout;
    setLayout(layout);
    layout->addWidget(label);

    setWindowFlags(Qt::Window |  Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint |Qt::X11BypassWindowManagerHint );
    setFocusPolicy(Qt::NoFocus);
    move(placecr);
    raise();
    show();
}

void SeExprEdPopupDocumentation::
mousePressEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    hide();
}
