cmake_minimum_required(VERSION 3.3)
project(plls_osp)
project(my_osdp_library)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lssl -lcrypto")
set(SOURCE_FILES main.cpp)



add_library(my_osdp_library SHARED OSDPlib.c OSDPlib.h)
add_executable(plls_osp main.cpp OSDPlib.c OSDPlib.h)
target_link_libraries(plls_osp my_osdp_library)