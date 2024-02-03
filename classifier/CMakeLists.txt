cmake_minimum_required(VERSION 3.27)
project(classifier)

set(Torch_DIR /Users/srimanachanta/dev/binaries/libtorch/share/cmake/Torch)
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 17)

add_executable(classifier src/native/main.cpp)
target_link_libraries(classifier "${TORCH_LIBRARIES}")
