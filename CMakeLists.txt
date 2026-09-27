cmake_minimum_required(VERSION 3.20)

project(
    pax
    VERSION 0.1.0
    DESCRIPTION "A secure, multi-threaded file archiver"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
    )
elseif(MSVC)
    add_compile_options(/W4)
endif()

add_library(pax_core STATIC
    src/cli.cpp
)

target_include_directories(pax_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(pax
    src/main.cpp
)

target_link_libraries(pax
    PRIVATE
        pax_core
)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
