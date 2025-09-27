cmake_minimum_required(VERSION 3.11)

project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED x11)

add_executable(imgprocessor main.cpp CImg.h)

target_link_libraries(imgprocessor ${X11_LIBRARIES})
target_include_directories(imgprocessor PRIVATE ${X11_INCLUDE_DIRS})