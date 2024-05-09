cmake_minimum_required(VERSION 3.28.1)
project(classifier)

set(CMAKE_CXX_STANDARD 17)

# Export traced models and datasets
set(TRACED_FIST_MODEL_PATH "model_weights/fist/traced_model_cpu.pt")
cmake_path(ABSOLUTE_PATH TRACED_FIST_MODEL_PATH NORMALIZE)
set(TRACED_SOLO_FINGER_MODEL_PATH "model_weights/solo_finger/traced_model_cpu.pt")
cmake_path(ABSOLUTE_PATH TRACED_SOLO_FINGER_MODEL_PATH NORMALIZE)
set(DATASET_DIR_PATH "../dataset/")
cmake_path(ABSOLUTE_PATH DATASET_DIR_PATH NORMALIZE)

# Handle generated files
configure_file("src/generate/ModelConfig.in.h" "ModelConfig.h" @ONLY)
configure_file("src/generate/DatasetConfig.in.h" "DatasetConfig.h" @ONLY)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(fmt REQUIRED)

file(GLOB SOURCES "src/native/cpp/*.cpp")
add_library(classifier ${SOURCES})
target_include_directories(
        classifier
        PUBLIC
        src/native/include
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(classifier PUBLIC ${Boost_LIBRARIES} ${TORCH_LIBRARIES} fmt::fmt-header-only)

add_executable(classifier_benchmark "src/benchmark.cpp")
target_link_libraries(classifier_benchmark PRIVATE classifier)
