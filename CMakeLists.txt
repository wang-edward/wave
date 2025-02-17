cmake_minimum_required(VERSION 3.15)
project(wave C)

# Set C standard and common flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Include your own headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add raylib as a subdirectory if available
if(EXISTS "${PROJECT_SOURCE_DIR}/external/raylib/CMakeLists.txt")
    add_subdirectory(external/raylib)
else()
    message(WARNING "raylib subdirectory not found!")
endif()

# List your source files
set(SOURCES
    src/main.c
    src/osc.c
    src/vec.c
    src/wavetable.c
)

# Create the executable target
add_executable(wave ${SOURCES})
# Define PLATFORM_DESKTOP for raylib
target_compile_definitions(wave PRIVATE PLATFORM_DESKTOP)

# Link with raylib and required system libraries
target_link_libraries(wave raylib m soundio pthread)
