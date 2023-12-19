cmake_minimum_required(VERSION 3.26)
project(classifier)

set(CMAKE_CXX_STANDARD 17)

include_directories(classifier "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(classifier SHARED ${SOURCES})

# Configure libtorch
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
target_link_libraries(classifier "${TORCH_LIBRARIES}")
