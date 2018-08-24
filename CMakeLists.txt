cmake_minimum_required(VERSION 2.6)
PROJECT(CorrosionAxial)

set (CMAKE_CXX_STANDARD 14)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

#not now
#find_package(VTK REQUIRED)
#include(${VTK_USE_FILE})

include_directories("../src/")


add_executable(CorrosionAxial "../src/CorrosionAxial.cpp")

target_link_libraries(CorrosionAxial  ${ITK_LIBRARIES} )#  ${VTK_LIBRARIES})


