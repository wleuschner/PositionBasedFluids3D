cmake_minimum_required(VERSION 2.8.8)

add_subdirectory("Shader")
add_subdirectory("ShaderProgram")
add_subdirectory("VertexBuffer")
add_subdirectory("IndexBuffer")
add_subdirectory("Light")
add_subdirectory("Vertex")

set(HEADERS
   ${HEADERS}
   PARENT_SCOPE
)

set(SOURCE
   ${SOURCE}
   PARENT_SCOPE
)
