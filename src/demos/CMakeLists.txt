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

include_directories(${CMAKE_BINARY_DIR}/src/SeExpr2)

add_executable(asciiGraph2 "asciiGraph.cpp")
target_link_libraries(asciiGraph2 SeExpr2)
install(TARGETS asciiGraph2 DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(asciiCalc2 "asciiCalculator.cpp")
target_link_libraries(asciiCalc2 SeExpr2)
install(TARGETS asciiCalc2 DESTINATION ${CMAKE_INSTALL_BINDIR})

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
    add_executable(expreditor "expreditor.cpp")
    target_link_libraries(expreditor SeExpr2)

if (ENABLE_QT5)
    target_link_libraries(expreditor Qt5::Core)
    target_link_libraries(expreditor Qt5::Gui)
    target_link_libraries(expreditor Qt5::Widgets)
else()
    target_link_libraries(expreditor ${QT_QTCORE_LIBRARY})
    target_link_libraries(expreditor ${QT_QTGUI_LIBRARY})
endif()

    target_link_libraries(expreditor ${SEEXPR_LIBRARIES})
    target_link_libraries(expreditor ${SEEXPR_EDITOR_LIBRARIES})

    include_directories(SYSTEM ${PNG_INCLUDE_DIR})
    target_link_libraries(expreditor ${PNG_LIBRARIES})
    install(TARGETS expreditor DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(PROGRAMS pyexpreditor DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


# TODO re-enable
add_subdirectory(imageSynth)
add_subdirectory(imageEditor)
add_subdirectory(mandelbrot)
#add_subdirectory(segraph)
#add_subdirectory(rman)

#if (ENABLE_LLVM_BACKEND)
#     add_executable(llvmtest "llvmtest.cpp")
#     target_link_libraries(llvmtest ${SEEXPR_LIBRARIES})
#     install(TARGETS llvmtest DESTINATION ${CMAKE_INSTALL_BINDIR})
#endif()
