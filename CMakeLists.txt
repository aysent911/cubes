cmake_minimum_required(VERSION 3.18)
project(Cubes)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES src/cubes.cpp src/glad.c)
add_executable(cubes ${SOURCE_FILES})
target_link_libraries(cubes glfw)



