cmake_minimum_required(VERSION 3.16)

project(sysfault C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(sysfault
    src/main.c
    src/fault.c
    src/fault_catalog.c
    src/search.c
    src/translation.c
    src/translation_en.c
    src/translation_uk.c
)

target_include_directories(sysfault PRIVATE src)

target_compile_options(sysfault PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
