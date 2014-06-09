cmake_minimum_required(VERSION 2.8.12)

project(oceQt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

find_package(OCE REQUIRED)
include_directories(${OCE_INCLUDE_DIRS})

set(oceQt_SOURCES
  OceWidget.cpp
  MainWindow.cpp
  main.cpp
)

set(oceQt_HEADERS
  OceWidget.h
  MainWindow.h
)

set(oceQt_FORMS
  MainWindow.ui
)

set(oceQt_RESOURCES

)

qt5_wrap_ui(oceQt_UI_HEADERS ${oceQt_FORMS})
qt5_add_resources(oceQt_RESOURCE_HEADERS ${oceQt_RESOURCES})

add_executable(oceQt WIN32 ${oceQt_SOURCES}
  ${oceQt_UI_HEADERS}
  ${oceQt_RESOURCE_HEADERS})
target_link_libraries(oceQt
  TKernel TKMath TKSTL TKSTEP TKV3d TKOpenGl)

qt5_use_modules(oceQt Core Widgets)
