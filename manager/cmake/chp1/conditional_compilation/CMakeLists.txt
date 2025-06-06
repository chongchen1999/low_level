cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(recipe-04 LANGUAGES CXX)

set(USE_LIBRARY OFF)
message(STATUS "Compile sources into a library? ${USE_LIBRARY}")

set(BUILD_SHARED_LIBS OFF)
list(APPEND _sources message.hpp message.cpp)
message(STATUS "Sources: ${_sources}")

if(USE_LIBRARY)
    # add_library will create a static library
    # since BUILD_SHARED_LIBS is OFF
    add_library(message ${_sources})
    add_executable(hello-world hello-world.cpp)
    target_link_libraries(hello-world message)
else()
    add_executable(hello-world hello-world.cpp ${_sources})
endif()