cmake_minimum_required(VERSION 2.8.8)
set(SOURCE
   ${SOURCE}
   ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/PlacementDialog.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/GLCanvas.cpp
   PARENT_SCOPE
)

set(HEADERS
   ${HEADERS}
   ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.h
   ${CMAKE_CURRENT_SOURCE_DIR}/PlacementDialog.h
   ${CMAKE_CURRENT_SOURCE_DIR}/GLCanvas.h
   PARENT_SCOPE
)
