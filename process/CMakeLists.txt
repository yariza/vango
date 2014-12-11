cmake_minimum_required(VERSION 2.8)
project( process )

# Add directory with macros
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Add cmake file with useful functions
include (Common)

find_package( OpenCV REQUIRED )
set(YAMLCPP_STATIC_LIBRARY TRUE)
find_package( YamlCpp REQUIRED )

# TCLAP library is required
find_package (TCLAP REQUIRED)
if (TCLAP_FOUND)
  include_directories (${TCLAP_INCLUDE_PATH})
else (TCLAP_FOUND)
  message (SEND_ERROR "Unable to locate TCLAP")
endif (TCLAP_FOUND)

#OpenCV required
find_package( OpenCV REQUIRED)

append_files( Headers "h" src/ src/canvas/)
append_files( Sources "cpp" src/ src/canvas/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable ( process ${Headers} ${Templates} ${Sources})

target_link_libraries( process ${OpenCV_LIBS} ${YAMLCPP_LIBRARY})
