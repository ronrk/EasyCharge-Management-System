cmake_minimum_required(VERSION 3.10)
project(my_program C)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Add include directories
include_directories(headers)

# Add source files
set(SOURCES
    src/main.c
    src/Utilis.c
    src/SystemData.c
    src/Queue.c
    src/BinaryTree.c
    src/Car.c
    src/Port.c
    src/Station.c
    src/Menu.c
    src/ErrorHandler.c
)

# Create executable
add_executable(my_program ${SOURCES})

# Copy data folder into build folder
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})