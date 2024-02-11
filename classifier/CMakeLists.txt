project(classifier)

set(CMAKE_CXX_STANDARD 17)

set(Torch_DIR /Users/srimanachanta/dev/binaries/libtorch/share/cmake/Torch)
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

file(GLOB SOURCES "src/native/cpp/*.cpp")
add_library(classifier STATIC ${SOURCES})
target_link_libraries(classifier "${TORCH_LIBRARIES}")
