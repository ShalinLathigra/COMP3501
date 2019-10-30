cmake_minimum_required(VERSION 2.6)

# Name of project
set(PROJ_NAME "Assignment_03")
project(${PROJ_NAME})

# Specify project files: header files and source files
set(HDRS
    asteroid.h 
	camera.h 
	game.h 
	laser_node.h
	player_node.h
	resource.h 
	resource_manager.h 
	root_node.h 
	scene_graph.h 
	scene_node.h
)
 
set(SRCS
	asteroid.cpp 
	camera.cpp 
	game.cpp 
	main.cpp 
	laser_node.cpp
	player_node.cpp
	resource.cpp 
	resource_manager.cpp 
	root_node.cpp 
	scene_graph.cpp 
	scene_node.cpp 
	ship_material_vp.glsl 
	ship_material_fp.glsl 
	asteroid_material_vp.glsl 
	asteroid_material_fp.glsl 
	laser_material_vp.glsl 
	laser_material_fp.glsl 
	CheckList.txt
	CMakeLists.txt
)

# Add path name to configuration file
configure_file(path_config.h.in path_config.h)

# Add executable based on the header and source files
add_executable(${PROJ_NAME} ${HDRS} ${SRCS})

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})

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
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)
endif(WIN32)
