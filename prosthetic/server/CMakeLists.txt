project(prosthetic_server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system regex filesystem thread chrono)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

include_directories("src/native/include")
file(GLOB SOURCES "src/native/cpp/*.cpp")
add_executable(prosthetic_server ${SOURCES})

target_link_libraries(prosthetic_server PRIVATE "${Boost_LIBRARIES}")
target_link_libraries(prosthetic_server PRIVATE bridge)
target_link_libraries(prosthetic_server PRIVATE "${TORCH_LIBRARIES}")
target_link_libraries(prosthetic_server PRIVATE classifier)
