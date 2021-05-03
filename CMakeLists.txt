cmake_minimum_required(VERSION 3.5)

project(GraphDecimator LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-o3")

set(LINKED_FILES
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    qcustomplot.cpp
    qcustomplot.h
    func.cpp
    func.h
    datadecimator.h
    point.h
)

find_package(Qt5 COMPONENTS Widgets PrintSupport REQUIRED)

if(ANDROID)
  add_library(GraphDecimator SHARED
    main.cpp
    ${LINKED_FILES}
  )
else()
  add_executable(GraphDecimator
    main.cpp
    ${LINKED_FILES}
  )
endif()

target_link_libraries(GraphDecimator PRIVATE Qt5::Widgets Qt5::PrintSupport)
