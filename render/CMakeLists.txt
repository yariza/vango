cmake_minimum_required(VERSION 2.8)
project( render )

# Add directory with macros
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Add cmake file with useful functions
include (Common)

find_package( OpenCV REQUIRED )
find_package( YamlCPP REQUIRED )

append_files( Headers "h" src/ src/canvas/)
append_files( Sources "cpp" src/ src/canvas/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable ( render ${Headers} ${Templates} ${Sources})

target_link_libraries( render ${OpenCV_LIBS} )
