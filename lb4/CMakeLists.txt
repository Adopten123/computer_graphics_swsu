cmake_minimum_required(VERSION 3.11)

project(Negate CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED x11)

add_library(circle_painter STATIC
        svgprocessor_utils.h
        svgprocessor_utils.cpp
        bresenham_algorithm.h
        bresenham_algorithm.cpp
        cimg_algorithm.h
        cimg_algorithm.cpp
        equation_algorithm.h
        equation_algorithm.cpp
        param_equation_algorithm.h
        param_equation_algorithm.cpp
)

add_executable(svgprocessor main.cpp CImg.h)

target_link_libraries(svgprocessor ${X11_LIBRARIES} circle_painter)
target_include_directories(svgprocessor PRIVATE ${X11_INCLUDE_DIRS})