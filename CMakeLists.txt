# --------------------------------------------------
# General
# --------------------------------------------------
cmake_minimum_required(VERSION 3.23)

project(sound_processor
    VERSION 1.0.0
    LANGUAGES CXX
)

# --------------------------------------------------
# Options
# --------------------------------------------------

option(BUILD_TESTS "Build tests" OFF)

# --------------------------------------------------
# Global settings
# --------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# --------------------------------------------------
# Library
# --------------------------------------------------

set(COMMON_INCLUDE ${PROJECT_SOURCE_DIR}/include/common)

add_library(io_lib
    src/IO/args_parser.cpp
)

target_include_directories(io_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/IO
        ${COMMON_INCLUDE}
)

target_compile_features(io_lib
    PUBLIC cxx_std_20
)

# Warnings
target_compile_options(io_lib
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:
            -Wall
            -Wextra
            -Wpedantic
            -Werror=return-type
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wsign-conversion
        >

        $<$<CONFIG:Debug>:
            -O0
            -g
        >

        $<$<CONFIG:Release>:
            -O3
        >
)

# --------------------------------------------------
# Executable
# --------------------------------------------------

add_executable(program
    app/main.cpp
)

target_link_libraries(program
    PRIVATE io_lib
)

# --------------------------------------------------
# Output directories
# --------------------------------------------------

set_target_properties(program io_lib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# --------------------------------------------------
# Tests
# --------------------------------------------------

if(BUILD_TESTS)

    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )

    FetchContent_MakeAvailable(Catch2)

    enable_testing()

    add_executable(tests
        tests/tests.cpp
    )

    target_link_libraries(tests
        PRIVATE
            Catch2::Catch2WithMain
            sound_lib
    )

    target_include_directories(tests
        PRIVATE include
    )

    add_test(NAME tests COMMAND tests)

endif()
