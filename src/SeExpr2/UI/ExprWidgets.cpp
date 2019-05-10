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

#include <QPushButton>
#include <QToolButton>
#include "ExprWidgets.h"

QToolButton* toolButton(QWidget* parent, bool solid)
{
    QToolButton* button = new QToolButton(parent);
    if (solid) {
        button->setStyleSheet(
            "QToolButton{background-color: hsv(0,0,80);"
            "border: 0px;"
            "border-radius: 2px}"
            "QToolButton:checked {border: 2px solid rgb(120,130,255);}"
            "QToolButton:hover {background-color: hsv(0,0,100);}"
            "QToolButton:pressed {background-color: hsv(0,0,60);}");
    } else {
        button->setStyleSheet(
            "QToolButton{background-color: transparent;"
            "border: 0px;"
            "border-radius: 2px}"
            "QToolButton:hover {background-color: rgb(100,100,100,64); }"
            "QToolButton:pressed {background-color: rgb(50,50,50,64); }");
    }

    return button;
}
