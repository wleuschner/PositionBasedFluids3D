cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
add_definitions(-DGLM_FORCE_SSE2 -DGLM_FORCE_ALIGNED)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

project(PositionBasedFluids)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5OpenGL)

add_subdirectory("Solver")
add_subdirectory("SpatialStruct")
add_subdirectory("GUI")
add_subdirectory("Graphics")
add_subdirectory("Resources")


set(SOURCE
   ${SOURCE}
   main.cpp
)

set(RES_FILES
   ${RES_FILES}
)

QT5_WRAP_UI(QT_HEADER_FORM GUI/MainWindow.ui GUI/PlacementDialog.ui)
QT5_WRAP_CPP(QT_HEADER_MOC GUI/MainWindow.h GUI/PlacementDialog.h GUI/GLCanvas.h)


add_executable(${PROJECT_NAME} ${RES_FILES} ${SOURCE} ${HEADERS} ${QT_HEADER_FORM} ${QT_HEADER_MOC})


find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
find_package (GLEW REQUIRED)
find_package(OpenGL)
find_package(assimp)
qt5_use_modules(${PROJECT_NAME} Widgets OpenGL Core Gui)
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} assimp)

set (source ${CMAKE_SOURCE_DIR}/Resources)
set (destination ${CMAKE_CURRENT_BINARY_DIR}/Resources)
add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination}
 DEPENDS ${destination}
 COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
