cmake_minimum_required(VERSION 2.8)

add_subdirectory("Kernel")
add_subdirectory("Constraint")
add_subdirectory("Force")
add_subdirectory("SolverImpl")


set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSolver.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/Particle.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSolver.h
   ${CMAKE_CURRENT_SOURCE_DIR}/Particle.h
   PARENT_SCOPE
)
