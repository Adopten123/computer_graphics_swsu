cmake_minimum_required(VERSION 3.11)

project(Negate CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED x11)

add_executable(processor main.cpp CImg.h)

target_link_libraries(processor
    PRIVATE
        ${X11_LIBRARIES}
        jpeg
        png
        z
        m
)
target_include_directories(processor PRIVATE ${X11_INCLUDE_DIRS})