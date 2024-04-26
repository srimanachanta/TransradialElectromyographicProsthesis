cmake_minimum_required(VERSION 3.28.1)
project(classifier)

set(CMAKE_CXX_STANDARD 17)

# Export traced model and weights
set(CLASSIFIER_TRACED_MODEL_CPU_PATH "${CMAKE_BINARY_DIR}/traced_model_cpu.pt")
configure_file("model_weights/traced_model_cpu.pt" "${CLASSIFIER_TRACED_MODEL_CPU_PATH}" COPYONLY)

configure_file("src/generate/ModelConfig.in.h" "ModelConfig.h" @ONLY)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_library(classifier INTERFACE)
target_include_directories(
        classifier
        INTERFACE
        src/native/include
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(classifier INTERFACE "${TORCH_LIBRARIES}")

file(GLOB SOURCES "src/native/cpp/*.cpp")
add_executable(classifier_benchmarker ${SOURCES})
target_link_libraries(classifier_benchmarker PRIVATE classifier)
