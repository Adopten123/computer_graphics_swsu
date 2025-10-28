cmake_minimum_required(VERSION 3.11)

project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(X11 COMPONENTS X11 Xft)

add_executable(svgprocessor main.cpp CImg.h)

target_link_libraries(svgprocessor ${X11_LIBRARIES})
target_include_directories(svgprocessor PRIVATE ${X11_INCLUDE_DIRS})