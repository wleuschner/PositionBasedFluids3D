cmake_minimum_required(VERSION 2.8)

set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/Poly6Kernel.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/SpikyKernel.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ViscocityKernel.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/Poly6Kernel.h
   ${CMAKE_CURRENT_SOURCE_DIR}/SpikyKernel.h
   ${CMAKE_CURRENT_SOURCE_DIR}/ViscocityKernel.h
   PARENT_SCOPE
)
