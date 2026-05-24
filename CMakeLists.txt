cmake_minimum_required(VERSION 3.16)

project(sysfault C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(sysfault
    src/main.c
    src/fault.c
    src/errno_data.c
    src/signal_data.c
    src/search.c
)

target_include_directories(sysfault PRIVATE src)

target_compile_options(sysfault PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
