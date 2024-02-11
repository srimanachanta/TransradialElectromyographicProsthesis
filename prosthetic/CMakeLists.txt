cmake_minimum_required(VERSION 3.25.1)
project(transradial_prosthetic)

set(CMAKE_CXX_STANDARD 17)

# Add Boost BEAST for WS Support
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})




include_directories("include")
file(GLOB SOURCES "src/native/*.cpp")
add_executable(transradial_prosthetic ${SOURCES})
