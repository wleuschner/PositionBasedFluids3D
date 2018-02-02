cmake_minimum_required(VERSION 2.8.8)

set(RES_FILES
   ${RES_FILES}
   ${CMAKE_CURRENT_SOURCE_DIR}/depth.vert
   ${CMAKE_CURRENT_SOURCE_DIR}/depth.frag
   ${CMAKE_CURRENT_SOURCE_DIR}/smooth.vert
   ${CMAKE_CURRENT_SOURCE_DIR}/smooth.frag
   ${CMAKE_CURRENT_SOURCE_DIR}/thickness.vert
   ${CMAKE_CURRENT_SOURCE_DIR}/thickness.frag
   ${CMAKE_CURRENT_SOURCE_DIR}/surface.vert
   ${CMAKE_CURRENT_SOURCE_DIR}/surface.frag
   PARENT_SCOPE
)
