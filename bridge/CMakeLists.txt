project(emg_bridge)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system regex)
include_directories(${Boost_INCLUDE_DIRS})

add_library(bridge INTERFACE)
target_include_directories(bridge INTERFACE src/native/include)

file(GLOB SOURCES "src/native/cpp/*.cpp")
add_executable(logger ${SOURCES})
target_link_libraries(logger PRIVATE bridge)
