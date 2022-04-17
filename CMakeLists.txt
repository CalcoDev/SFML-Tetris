cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(SFML-Tetris VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug
        CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_SOURCE_DIR}/config.h)

add_executable(SFML-Tetris 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/Tetramino.cpp
)
target_include_directories(SFML-Tetris PRIVATE ${CMAKE_SOURCE_DIR}/src/Headers)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Program Files (x86)/SFML/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(SFML-Tetris sfml-graphics sfml-audio)