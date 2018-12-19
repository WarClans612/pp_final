cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( serial_m serial_matrix.cpp image.cpp)
target_link_libraries( serial_m ${OpenCV_LIBS} )
