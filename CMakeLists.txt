cmake_minimum_required(VERSION 3.20)
if (CMAKE_VERSION VERSION_LESS 3.20)
    message(FATAL_ERROR "CMake >= 3.20.0 required.")
endif()
project(8bit-system-monitor VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the required packages
find_package(Curses REQUIRED)

# Add the executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link the required libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CURSES_LIBRARIES}
    )