cmake_minimum_required(VERSION 3.28.1)
project(standalone_prosthetic)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")
add_executable(standalone_prosthetic ${SOURCES})
target_include_directories(standalone_prosthetic PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(standalone_prosthetic PRIVATE ${Boost_LIBRARIES} classifier PCA9685_driver)
