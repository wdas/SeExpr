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

if (ENABLE_QT5)
    find_package(Qt5 COMPONENTS Core Gui Widgets) # find and setup Qt5 for this project
    include_directories(${Qt5Core_INCLUDE_DIRS}
                        ${Qt5Gui_INCLUDE_DIRS}
                        ${Qt5OpenGL_INCLUDE_DIRS})
else()
    find_package(Qt4 COMPONENTS QtCore QtGui)
    include_directories(${QT_INCLUDE_DIR}
                        ${QT_INCLUDE_DIR}/QtCore
                        ${QT_INCLUDE_DIR}/QtGui)
endif()

if(Qt5_FOUND OR QT4_FOUND)
    set(segraph_MOC_HDRS Graph.h GraphWindow.h Functions.h)
    set(segraph_CPPS main.cpp Graph.cpp GraphWindow.cpp Functions.cpp)

if (ENABLE_QT5)
    qt5_wrap_cpp(segraph_MOC_SRCS ${segraph_MOC_HDRS})
else()
    qt4_wrap_cpp(segraph_MOC_SRCS ${segraph_MOC_HDRS})
endif()

    add_executable(segraph2 ${segraph_CPPS} ${segraph_MOC_SRCS})
    install(TARGETS segraph2 DESTINATION demo)

if (ENABLE_QT5)
    target_link_libraries(segraph Qt5::Core)
    target_link_libraries(segraph Qt5::Gui)
    target_link_libraries(segraph Qt5::Widgets)
else()
    target_link_libraries(segraph2 ${QT_QTCORE_LIBRARY})
    target_link_libraries(segraph2 ${QT_QTGUI_LIBRARY})
endif()

    target_link_libraries(segraph2 ${SEEXPR_LIBRARIES})
endif()
