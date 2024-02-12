cmake_minimum_required(VERSION 3.25.1)
project(transradial_electromyographic_prosthetic)

set(CMAKE_CXX_STANDARD 17)

set(CLASSIFIER_TRACED_MODEL_PATH "${CMAKE_BINARY_DIR}/traced_model.pt")
set(CLASSIFIER_WEIGHTS_PATH "${CMAKE_BINARY_DIR}/weights.pt")

add_subdirectory(bridge)
add_subdirectory(classifier)
add_subdirectory(prosthetic)
