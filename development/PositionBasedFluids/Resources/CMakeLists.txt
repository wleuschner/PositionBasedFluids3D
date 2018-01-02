cmake_minimum_required(VERSION 2.8.8)


set(RES_FILES
   ${RES_FILES}
   ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert
   ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag
   ${CMAKE_CURRENT_SOURCE_DIR}/shader.glsl
   PARENT_SCOPE
)
