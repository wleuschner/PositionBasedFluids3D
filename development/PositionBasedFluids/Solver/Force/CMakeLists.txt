cmake_minimum_required(VERSION 2.8)

add_subdirectory("ForceImpl")

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractForce.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractForce.h
   PARENT_SCOPE
)
