cmake_minimum_required(VERSION 2.8)
project(PositionBasedFluids)

add_subdirectory("Solver")

set(SOURCE
   ${SOURCE}
   main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE} ${HEADERS})
