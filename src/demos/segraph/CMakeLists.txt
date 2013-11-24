
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

find_package(Qt4 COMPONENTS QtCore QtGui) # find and setup Qt4 for this project
if(QT4_FOUND)
    set(segraph_MOC_HDRS Graph.h GraphWindow.h)
    set(segraph_CPPS segraph.cpp Graph.cpp GraphWindow.cpp)
    qt4_wrap_cpp(segraph_MOC_SRCS ${segraph_MOC_HDRS})
    
    add_executable(segraph ${segraph_CPPS} ${segraph_MOC_SRCS})
    install(TARGETS segraph DESTINATION share/SeExpr/demo)
    include_directories(${QT_INCLUDE_DIR})
    include_directories(${SEEXPR_EDITOR_INCLUDES})
    target_link_libraries(segraph ${QT_QTCORE_LIBRARY})
    target_link_libraries(segraph ${QT_QTGUI_LIBRARY})
    target_link_libraries(segraph ${SEEXPR_LIBRARIES})
    target_link_libraries(segraph ${SEEXPR_EDITOR_LIBRARIES})
endif(QT4_FOUND)
