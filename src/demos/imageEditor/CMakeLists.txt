# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

find_package(Qt4 COMPONENTS QtCore QtGui) # find and setup Qt4 for this project
if(QT4_FOUND)
    set(imageEditor_MOC_HDRS ImageEditorDialog.h)
    set(imageEditor_CPPS imageEditor.cpp)
    qt4_wrap_cpp(imageEditor_MOC_SRCS ${imageEditor_MOC_HDRS})

    add_executable(imageEditor ${imageEditor_CPPS} ${imageEditor_MOC_SRCS})
    include_directories(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${SEEXPR_EDITOR_INCLUDES})
    target_link_libraries(imageEditor ${QT_QTCORE_LIBRARY})
    target_link_libraries(imageEditor ${QT_QTGUI_LIBRARY})
    target_link_libraries(imageEditor ${SEEXPR_LIBRARIES})
    target_link_libraries(imageEditor ${SEEXPR_EDITOR_LIBRARIES})
endif(QT4_FOUND)

FIND_PACKAGE(PNG)
IF(PNG_FOUND)
    INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES(imageEditor ${PNG_LIBRARIES})
ENDIF(PNG_FOUND)

