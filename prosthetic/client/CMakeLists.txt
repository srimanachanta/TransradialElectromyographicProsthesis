project(prosthetic_client)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

include_directories("src/native/include")
file(GLOB SOURCES "src/native/cpp/*.cpp")
add_executable(prosthetic_client ${SOURCES})
