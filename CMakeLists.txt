cmake_minimum_required(VERSION 3.28.1)
project(transradial_electromyographic_prosthetic)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(bridge)
add_subdirectory(classifier)
add_subdirectory(prosthetic)
