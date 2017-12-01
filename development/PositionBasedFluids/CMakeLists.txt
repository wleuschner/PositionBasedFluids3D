cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
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

add_executable(${PROJECT_NAME} ${SOURCE} ${HEADERS} ${QT_HEADER_MOC} ${QT_HEADER_FORM})

include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
qt5_use_modules(${PROJECT_NAME} Widgets OpenGL Core Gui)
