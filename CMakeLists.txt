#-----------------------------------
# Bhaskar Bhattacharya - backbone taken from Nik Karpinsky
# Iowa State University 
# CMake file for compiling tracking segment 
#-----------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(JohnDeereTracking)

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/CMakeModules")
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/${PROJECT_NAME}")
SET(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

#------
# QT
#------
FIND_PACKAGE(Qt4 4.8.4 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
SET(LIBS ${LIBS} ${QT_LIBRARIES})

#------
# OpenGL (use glut32)
#------
FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
SET(LIBS ${LIBS} ${OPENGL_LIBRARIES})

#------
# OpenCV (2.4)
#------
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${OpenCV_LIB_DIR_DBG}/*d.lib)

#------
# OSG (3.0.1)
#------
FIND_PACKAGE(OSG REQUIRED)
INCLUDE_DIRECTORIES(${OSG_INCLUDE_DIR})
SET(LIBS ${LIBS} "H:/Windows_Stuff/OpenSceneGraph-3.0.1/install/lib/*d.lib")

#------
# VTK
#------
FIND_PACKAGE(VTK REQUIRED)
INCLUDE_DIRECTORIES(${VTK_INC})
SET(LIBS ${LIBS} "H:/Windows_Stuff/VTK/install/lib/vtk-5.10/*.lib")

SET(SOURCES
	${SRC_DIR}/main.cpp
	${SRC_DIR}/OSGView.cpp
	${SRC_DIR}/Tracker.cpp
	${SRC_DIR}/RWVector.cpp
	${SRC_DIR}/mainwindow.cpp
	${SRC_DIR}/OSGQTWidget.cpp

	${SRC_DIR}/OSGView.h
	${SRC_DIR}/Tracker.h
	${SRC_DIR}/RWVector.h
	${SRC_DIR}/mainwindow.h
	${SRC_DIR}/OSGQTWidget.h
	)
SET(UIS
	${SRC_DIR}/mainwindow.ui
	)
SET(MOCS
	${SRC_DIR}/mainwindow.h
	${SRC_DIR}/OSGQTWidget.h
	)

QT4_WRAP_UI(GENERATED_SOURCES ${UIS})
QT4_WRAP_cPP(GENERATED_SOURCES ${MOCS})

SOURCE_GROUP("Generated Sources - Do Not Edit" FILES ${GENERATED_SOURCES})

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}") 

ADD_EXECUTABLE(${PROJECT_NAME}
	${SOURCES}
	${GENERATED_SOURCES}
	${HEADERS}
	${UIS}
	${MOCS}
	)


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})