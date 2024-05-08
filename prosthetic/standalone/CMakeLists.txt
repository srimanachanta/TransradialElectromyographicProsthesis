cmake_minimum_required(VERSION 3.28.1)
project(standalone_prosthetic)

set(CMAKE_CXX_STANDARD 17)

option(ENABLE_PROSTHETIC "Enable linking of the PCA driver and send outputs to it" ON)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")
add_executable(standalone_prosthetic ${SOURCES})
target_include_directories(standalone_prosthetic PRIVATE include)

target_link_libraries(standalone_prosthetic PRIVATE ${Boost_LIBRARIES} classifier)

if(ENABLE_PROSTHETIC)
    message("ENABLE_PROSTHETIC==ON Linking PCA Driver")
    target_link_libraries(standalone_prosthetic PRIVATE PCA9685_driver)
endif()
