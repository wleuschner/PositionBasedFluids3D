cmake_minimum_required(VERSION 2.8)

add_subdirectory("KernelImpl")

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractKernel.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/AbstractKernel.h
   PARENT_SCOPE
)
