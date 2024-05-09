cmake_minimum_required(VERSION 3.28.1)
project(PCA9685_driver)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")
add_library(PCA9685_driver ${SOURCES})
target_include_directories(PCA9685_driver PUBLIC include)

target_link_libraries(PCA9685_driver PRIVATE i2c)
