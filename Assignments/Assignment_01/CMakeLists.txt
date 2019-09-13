cmake_minimum_required(VERSION 2.6)

# Name of project
project(Assignment_01)

# Specify project source files
set(SRCS
   main.cpp 
   Entity.h
   Entity.cpp
   GameManager.h
   GameManager.cpp
   Includes.h
   fragment_shader.glsl
   vertex_shader.glsl
)

# Add executable based on the source files
add_executable(Assignment_01 ${SRCS})

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(Assignment_01 ${OPENGL_gl_LIBRARY})

# Other libraries needed
set(LIBRARY_PATH "" CACHE PATH "Folder with GLEW, GLFW, GLM, and SOIL libraries")
include_directories(${LIBRARY_PATH}/include)
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY glfw)
    find_library(SOIL_LIBRARY SOIL)
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)
target_link_libraries(Assignment_01 ${GLEW_LIBRARY})
target_link_libraries(Assignment_01 ${GLFW_LIBRARY})
target_link_libraries(Assignment_01 ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(Assignment_01 PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)
