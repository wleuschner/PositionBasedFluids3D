cmake_minimum_required(VERSION 2.8)
project(PositionBasedFluids)

add_subdirectory("Solver")
add_subdirectory("SpatialHashMap")

set(SOURCE
   ${SOURCE}
   main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE} ${HEADERS})

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARIES})
