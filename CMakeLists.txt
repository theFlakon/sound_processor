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
# Compile flags and other options
# --------------------------------------------------

add_library(project_options INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_20)

target_compile_options(project_options
    INTERFACE
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
# Libraries
# --------------------------------------------------

set(COMMON_INCLUDE ${PROJECT_SOURCE_DIR}/include/common)

set(IO_SRC_DIR ${PROJECT_SOURCE_DIR}/src/IO)
set(IO_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/IO)

set(UTILS_SRC_DIR ${PROJECT_SOURCE_DIR}/src/utils)

add_library(io_lib
    ${IO_SRC_DIR}/args_parser.cpp
    ${IO_SRC_DIR}/output.cpp
)

add_library(utils_lib
    ${UTILS_SRC_DIR}/dispatcher.cpp
)

target_include_directories(io_lib
    PUBLIC
        ${IO_INCLUDE_DIR}
        ${COMMON_INCLUDE}
)

target_include_directories(utils_lib
    PUBLIC
        ${IO_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include/utils
        ${COMMON_INCLUDE}
)

target_link_libraries(io_lib PUBLIC project_options)
target_link_libraries(utils_lib PRIVATE project_options)

# --------------------------------------------------
# Executable
# --------------------------------------------------

add_executable(program
    app/main.cpp
)

target_link_libraries(program
    PRIVATE project_options io_lib utils_lib
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
