cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

project(PositionBasedFluids)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5OpenGL)

add_subdirectory("Solver")
add_subdirectory("SpatialHashMap")
add_subdirectory("GUI")

set(SOURCE
   ${SOURCE}
   main.cpp
)

QT5_WRAP_UI(QT_HEADER_FORM GUI/MainWindow.ui)
QT5_WRAP_CPP(QT_HEADER_MOC GUI/MainWindow.h GUI/GLCanvas.h)


add_executable(${PROJECT_NAME} ${SOURCE} ${HEADERS} ${QT_HEADER_FORM} ${QT_HEADER_MOC})

include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
find_package(OpenGL)
qt5_use_modules(${PROJECT_NAME} Widgets OpenGL Core Gui)
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})
