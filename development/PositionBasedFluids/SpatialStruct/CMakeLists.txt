cmake_minimum_required(VERSION 2.8)

add_subdirectory("SpatialHashMap")
add_subdirectory("RadixSort")

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSpatialStruct.cpp
   PARENT_SCOPE
)


set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSpatialStruct.h
   PARENT_SCOPE
)
