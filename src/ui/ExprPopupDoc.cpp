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
#include "ExprPopupDoc.h"
#include <QLabel>
#include <QHBoxLayout>

ExprPopupDoc::ExprPopupDoc(QWidget* parent, const QPoint& placecr, const QString& msg)
{
    Q_UNUSED(parent);
    label = new QLabel(msg);
    QHBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);
    layout->addWidget(label);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint |
                   Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::NoFocus);
    move(placecr);
    raise();
    show();
}

void ExprPopupDoc::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    hide();
}
