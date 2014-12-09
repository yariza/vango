cmake_minimum_required(VERSION 2.8)
project( render )

find_package( OpenCV REQUIRED )
add_executable( render src/render.cpp )
target_link_libraries( render ${OpenCV_LIBS} )
