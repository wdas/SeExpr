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

#pragma once

#include <QtGui/QPalette>
inline QPalette createDefaultColorPalette() {
    QPalette palette;
    QPalette::ColorGroup groups[2] = {QPalette::Active, QPalette::Inactive};
    for (QPalette::ColorGroup group : groups) {
        palette.setColor(group, QPalette::Window, QColor(49, 49, 49, 255));
        palette.setColor(group, QPalette::WindowText, QColor(149, 149, 149, 255));
        palette.setColor(group, QPalette::Base, QColor(44, 44, 44, 255));
        palette.setColor(group, QPalette::AlternateBase, QColor(59, 59, 59, 255));
        palette.setColor(group, QPalette::ToolTipBase, QColor(246, 255, 164, 255));
        palette.setColor(group, QPalette::ToolTipText, QColor(0, 0, 0, 255));
        palette.setColor(group, QPalette::Text, QColor(149, 149, 149, 255));
        palette.setColor(group, QPalette::ButtonText, QColor(162, 162, 162, 255));
        palette.setColor(group, QPalette::BrightText, QColor(252, 252, 252, 255));
        palette.setColor(group, QPalette::Light, QColor(81, 81, 81, 255));
        palette.setColor(group, QPalette::Midlight, QColor(71, 71, 71, 255));
        palette.setColor(group, QPalette::Button, QColor(61, 61, 61, 255));
        palette.setColor(group, QPalette::Mid, QColor(40, 40, 40, 255));
        palette.setColor(group, QPalette::Dark, QColor(30, 30, 30, 255));
        palette.setColor(group, QPalette::Shadow, QColor(0, 0, 0, 255));
        palette.setColor(group, QPalette::Highlight, QColor(103, 95, 92, 255));
        palette.setColor(group, QPalette::HighlightedText, QColor(252, 252, 252, 255));
    }
    palette.setColor(QPalette::Disabled, QPalette::Window, QColor(49, 49, 49, 255));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(149, 149, 149, 128));
    palette.setColor(QPalette::Disabled, QPalette::Base, QColor(44, 44, 44, 128));
    palette.setColor(QPalette::Disabled, QPalette::AlternateBase, QColor(59, 59, 59, 255));
    palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, QColor(246, 255, 164, 255));
    palette.setColor(QPalette::Disabled, QPalette::ToolTipText, QColor(0, 0, 0, 255));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(149, 149, 149, 128));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(162, 162, 162, 128));
    palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(252, 252, 252, 128));
    palette.setColor(QPalette::Disabled, QPalette::Light, QColor(45, 45, 45, 255));
    palette.setColor(QPalette::Disabled, QPalette::Midlight, QColor(71, 71, 71, 255));
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor(61, 61, 61, 128));
    palette.setColor(QPalette::Disabled, QPalette::Mid, QColor(15, 15, 15, 255));
    palette.setColor(QPalette::Disabled, QPalette::Dark, QColor(30, 30, 30, 255));
    palette.setColor(QPalette::Disabled, QPalette::Shadow, QColor(0, 0, 0, 128));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(103, 95, 92, 255));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(252, 252, 252, 128));
    return palette;
}
