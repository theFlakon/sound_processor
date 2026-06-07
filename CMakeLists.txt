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

include(GNUInstallDirs)

function(add_module_library LIB_NAME MODULE_NAME)
    set(SRC_DIR     ${PROJECT_SOURCE_DIR}/src/${MODULE_NAME})
    set(INC_DIR     ${PROJECT_SOURCE_DIR}/include/${MODULE_NAME})

    add_library(${LIB_NAME} ${ARGN})

    target_include_directories(${LIB_NAME}
        PUBLIC
            ${INC_DIR}
    )
    target_link_libraries(${LIB_NAME} PUBLIC project_options)
endfunction()

add_module_library(io_lib IO
    src/IO/args_parser.cpp
    src/IO/output.cpp
    src/IO/io_bin.cpp
)

add_module_library(utils_lib utils
    src/utils/dispatcher.cpp
)

add_module_library(sound_lib sound_module
    src/sound_module/waveform.cpp
    src/sound_module/filter_registry.cpp
    src/sound_module/filters.cpp
    src/sound_module/pipeline.cpp
)

target_link_libraries(io_lib PUBLIC sound_lib utils_lib)
target_link_libraries(utils_lib PUBLIC io_lib)
target_link_libraries(sound_lib PUBLIC utils_lib)

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
