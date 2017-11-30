cmake_minimum_required(VERSION 2.8)

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/Poly6Kernel.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/Poly6Kernel.h
   PARENT_SCOPE
)
