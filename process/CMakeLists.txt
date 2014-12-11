cmake_minimum_required(VERSION 2.8)
project( process )

# Add directory with macros
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Add cmake file with useful functions
include (Common)

# TCLAP library is required
find_package (TCLAP REQUIRED)
if (TCLAP_FOUND)
  include_directories (${TCLAP_INCLUDE_PATH})
else (TCLAP_FOUND)
  message (SEND_ERROR "Unable to locate TCLAP")
endif (TCLAP_FOUND)

append_files( Headers "h" src/ src/canvas/)
append_files( Sources "cpp" src/ src/canvas/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable ( render ${Headers} ${Templates} ${Sources})

