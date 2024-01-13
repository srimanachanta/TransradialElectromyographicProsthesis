cmake_minimum_required(VERSION 3.27)
project(logger)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system regex)
include_directories(${Boost_INCLUDE_DIRS})

include_directories("include")
file(GLOB SOURCES "src/*.cpp")
add_executable(logger ${SOURCES})
