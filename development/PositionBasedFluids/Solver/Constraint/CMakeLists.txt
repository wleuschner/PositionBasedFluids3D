cmake_minimum_required(VERSION 2.8)

add_subdirectory("ConstraintImpl")

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractConstraint.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractConstraint.h
   PARENT_SCOPE
)

