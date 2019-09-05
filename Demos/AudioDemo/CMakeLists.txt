cmake_minimum_required(VERSION 2.6)

# Name of project
project(AudioDemo)

# Specify project files: header files and source files
set(HDRS
    audio_manager.h
)
 
set(SRCS
    audio_manager.cpp audio_demo.cpp
)

# Add path name to configuration file
configure_file(path_config.h.in path_config.h)

# Add executable based on the source files
add_executable(AudioDemo ${HDRS} ${SRCS})

# Set up OpenAL and ALUT libraries
set(LIBRARY_PATH "" CACHE PATH "Folder with audio libraries")
include_directories(${LIBRARY_PATH}/include)
if(NOT WIN32)
    find_library(OPENAL_LIBRARY openal)
    find_library(ALUT_LIBRARY alut)
elseif(WIN32)
    find_library(OPENAL_LIBRARY "OpenAL32.lib" HINTS ${LIBRARY_PATH}/lib)
    find_library(ALUT_LIBRARY "alut.lib" HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)
target_link_libraries(AudioDemo ${OPENAL_LIBRARY})
target_link_libraries(AudioDemo ${ALUT_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(AudioDemo PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)
