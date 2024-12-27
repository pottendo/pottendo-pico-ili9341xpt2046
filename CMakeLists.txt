cmake_minimum_required(VERSION 3.13)

# uncomment the following line if building for different pico board
set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(settings.cmake)
include(pico_sdk_import.cmake)


project(${lib_name} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(src)
target_include_directories(${lib_name}
        PUBLIC src
)
add_subdirectory(example)
target_link_libraries(${lib_name}
        pico_stdlib
)
target_link_libraries(${example_name}
        pico_stdlib
)
