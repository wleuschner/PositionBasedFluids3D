cmake_minimum_required(VERSION 2.8.8)

add_subdirectory("Effects")
add_subdirectory("Simulation")
add_subdirectory("Voxel")

set(RES_FILES
   ${RES_FILES}
   PARENT_SCOPE
)
