cmake_minimum_required(VERSION 2.8.8)

add_subdirectory("Surface")
add_subdirectory("Particles")

set(RES_FILES
   ${RES_FILES}
   ${CMAKE_CURRENT_SOURCE_DIR}/skybox.vert
   ${CMAKE_CURRENT_SOURCE_DIR}/skybox.frag
   PARENT_SCOPE
)
