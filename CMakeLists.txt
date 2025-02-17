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


# TEST
enable_testing()

# Define unit test sources
set(TEST_SOURCES
    tests/test_wavetable.c
    tests/test_osc.c
    tests/test_vec.c
    src/osc.c
    src/vec.c
    src/wavetable.c
)

# Create an executable for unit tests
add_executable(unit_tests ${TEST_SOURCES})

# Include your project headers
target_include_directories(unit_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# If you need specific definitions (for instance, if raylib expects PLATFORM_DESKTOP)
# target_compile_definitions(unit_tests PRIVATE PLATFORM_DESKTOP)

# Link against the Criterion library and other system libraries
target_link_libraries(unit_tests criterion m pthread)

# Register the unit tests with CTest
add_test(NAME unit_tests COMMAND unit_tests)
