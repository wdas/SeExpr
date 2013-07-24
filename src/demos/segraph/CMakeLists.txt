# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

find_package(Qt4 COMPONENTS QtCore QtGui) # find and setup Qt4 for this project
if(QT4_FOUND)
    set(segraph_MOC_HDRS Graph.h GraphWindow.h)
    set(segraph_CPPS segraph.cpp Graph.cpp GraphWindow.cpp)
    qt4_wrap_cpp(segraph_MOC_SRCS ${segraph_MOC_HDRS})
    
    add_executable(segraph ${segraph_CPPS} ${segraph_MOC_SRCS})
    install(TARGETS segraph DESTINATION demo)
    include_directories(${QT_INCLUDE_DIR})
    include_directories(${SEEXPR_EDITOR_INCLUDES})
    target_link_libraries(segraph ${QT_QTCORE_LIBRARY})
    target_link_libraries(segraph ${QT_QTGUI_LIBRARY})
    target_link_libraries(segraph ${SEEXPR_LIBRARIES})
    target_link_libraries(segraph ${SEEXPR_EDITOR_LIBRARIES})
endif(QT4_FOUND)
