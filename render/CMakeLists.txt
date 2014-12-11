cmake_minimum_required(VERSION 2.8)
project( render )

# Add directory with macros
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Add cmake file with useful functions
include (Common)

find_package( OpenCV REQUIRED )
find_package( YamlCPP REQUIRED )

# boost
set(Boost_ADDITIONAL_VERSIONS "1.51" "1.51.0")
find_package(Boost 1.50 REQUIRED
    COMPONENTS "filesystem" "system" "program_options")

append_files( Headers "h" src/ src/canvas/)
append_files( Sources "cpp" src/ src/canvas/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
add_executable ( render ${Headers} ${Templates} ${Sources})

target_link_libraries( render ${OpenCV_LIBS} ${YAMLCPP_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY})
