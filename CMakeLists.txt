cmake_minimum_required(VERSION 3.26)
project(Windows_Tools)

set(CMAKE_CXX_STANDARD 17)
add_link_options(-static -static-libgcc -static-libstdc++)

add_executable(background backgroud.cpp)
target_link_options(background PRIVATE -mwindows)

add_executable(volume volume.cpp)

add_executable(screens screens.cpp)
