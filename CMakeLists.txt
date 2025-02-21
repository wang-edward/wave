cmake_minimum_required(VERSION 3.15)
project(wave C)

# Set C standard and common flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(CMAKE_BUILD_TYPE Debug)

# Include your own headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add raylib as a subdirectory if available
if(EXISTS "${PROJECT_SOURCE_DIR}/external/raylib/CMakeLists.txt")
    add_subdirectory(external/raylib)
else()
    message(WARNING "raylib subdirectory not found!")
endif()

# List your source files
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*"
)

# Create the executable target
add_executable(wave ${SOURCES})
# Define PLATFORM_DESKTOP for raylib
target_compile_definitions(wave PRIVATE PLATFORM_DESKTOP)

# Link with raylib and required system libraries
target_link_libraries(wave raylib m soundio pthread criterion)


# TEST
enable_testing()

# Define unit test sources
file(GLOB TEST_SOURCES
    # "${CMAKE_CURRENT_SOURCE_DIR}/tests/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*"
)
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")


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
