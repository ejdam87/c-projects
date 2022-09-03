cmake_minimum_required(VERSION 3.0)
project(hw05)

# -------------------------------------------------------------------
# DO NOT EDIT LINES BELOW lest you break it

# Search for source files.
# If you add a new source to your project, just run ‹cmake -B build -S .›
# or rerun CMake configuration from your IDE.
file(GLOB SOURCES "*.c")


# Targets
set(EXECUTABLE checkperms)

# Executable
add_executable(${EXECUTABLE} ${SOURCES} main.c)
target_compile_definitions(${EXECUTABLE} PRIVATE _POSIX_C_SOURCE=200809L)

# Data files
file(GLOB DATA_FILES "data/*")
foreach (data_file ${DATA_FILES})
    file(COPY ${data_file}
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    # using regular Clang, AppleClang or GCC
    # Strongly suggested: neable -Werror
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic -g")
endif ()

if (MINGW)
    target_compile_definitions(${EXECUTABLE} PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif ()
