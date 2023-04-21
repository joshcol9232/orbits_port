cmake_minimum_required(VERSION 3.5)

project(orbits_port LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_executable(orbits_port main.cpp)
target_link_libraries (orbits_port Eigen3::Eigen)

install(TARGETS orbits_port
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

