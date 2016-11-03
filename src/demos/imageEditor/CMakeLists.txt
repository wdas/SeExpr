
# Copyright Disney Enterprises, Inc.  All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License
# and the following modification to it: Section 6 Trademarks.
# deleted and replaced with:
#
# 6. Trademarks. This License does not grant permission to use the
# trade names, trademarks, service marks, or product names of the
# Licensor and its affiliates, except as required for reproducing
# the content of the NOTICE file.
#
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

# Set application root dir for use inside the source code.
add_definitions(-DIMAGE_EDITOR_ROOT="${CMAKE_INSTALL_PREFIX}")

find_package(Qt4 COMPONENTS QtCore QtGui)
find_package(PNG)

if (QT4_FOUND AND PNG_FOUND)
    set(imageEditor_MOC_HDRS ImageEditorDialog.h)
    set(imageEditor_CPPS imageEditor.cpp)
    qt4_wrap_cpp(imageEditor_MOC_SRCS ${imageEditor_MOC_HDRS})

    add_executable(imageEditor2 ${imageEditor_CPPS} ${imageEditor_MOC_SRCS})
    include_directories(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${SEEXPR_EDITOR_INCLUDES})
    target_link_libraries(imageEditor2 ${QT_QTCORE_LIBRARY})
    target_link_libraries(imageEditor2 ${QT_QTGUI_LIBRARY})
    target_link_libraries(imageEditor2 ${SEEXPR_LIBRARIES})
    target_link_libraries(imageEditor2 ${SEEXPR_EDITOR_LIBRARIES})

    include_directories(SYSTEM ${PNG_INCLUDE_DIR})
    target_link_libraries(imageEditor2 ${PNG_LIBRARIES})
    install(TARGETS imageEditor2 DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

install(FILES fbm.se noisecolor1.se noisecolor2.se noise.se raytrace.se sinc.se
        DESTINATION share/SeExpr2/expressions)
