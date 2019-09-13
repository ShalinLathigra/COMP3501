/*
 *
 * A program that demonstrates how to render two different shapes stored
 * as triangle meshes
 *
 * Copyright (c) 2018-2019 Oliver van Kaick <Oliver.vanKaick@carleton.ca>
 *
 */

#include <iostream>
#include <stdexcept>

#include "bin/Application.h"



// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl




// Globals that define the OpenGL window and viewport
const std::string window_title_g = "Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);

// Globals that define the OpenGL camera view and projection
glm::vec3 camera_position_g(0.5, 0.5, 3.0); // Position of camera
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0); // Point looking at
glm::vec3 camera_up_g(0.0, 1.0, 0.0); // Up vector
float camera_near_clip_distance_g = 0.01; // Near clipping plane
float camera_far_clip_distance_g = 1000.0; // Far clipping plane
float camera_fov_g = 20.0; // Field-of-view for projection

// Globals that define the animation parameters and animation state
// Speed of ration in radians/second
float speed = glm::pi<float>()/2.0f; 
// Rotation angle derived from the speed
float angle = fmod(speed, 2.0f*glm::pi<float>());
// Current animation state
bool animating = true;
// Time since last animation update
double last_time;


// Source code of vertex shader
const char *source_vp = "#version 130\n\
\n\
// Vertex buffer\n\
in vec3 vertex;\n\
in vec3 color;\n\
\n\
// Uniform (global) buffer\n\
uniform mat4 world_mat;\n\
uniform mat4 view_mat;\n\
uniform mat4 projection_mat;\n\
\n\
// Attributes forwarded to the fragment shader\n\
out vec4 color_interp;\n\
\n\
\n\
void main()\n\
{\n\
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);\n\
    color_interp = vec4(color, 1.0);\n\
}";


// Source code of fragment shader
const char *source_fp = "\n\
#version 130\n\
\n\
// Attributes passed from the vertex shader\n\
in vec4 color_interp;\n\
\n\
\n\
void main()\n\
{\n\
    gl_FragColor = color_interp;\n\
}";


// Callback for when a key is pressed
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Quit the program when pressing 'q'
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    // Play/pause animation when pressing space bar
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        // Toggle animating flag
        animating = !animating;
        // Reset time
        last_time = glfwGetTime();
    }
}


// Callback for when the window is resized
void ResizeCallback(GLFWwindow* window, int width, int height){

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);

    // Update projection matrix
    void* ptr = glfwGetWindowUserPointer(window);
    glm::mat4 *projection_matrix = (glm::mat4 *) ptr;
    float top = tan((camera_fov_g/2.0)*(glm::pi<float>()/180.0))*camera_near_clip_distance_g;
    float right = top * width/height;
    (*projection_matrix) = glm::frustum(-right, right, -top, top, camera_near_clip_distance_g, camera_far_clip_distance_g);
}


// Store information of one model for rendering
typedef struct model {
    GLuint vbo; // OpenGL vertex buffer object
    GLuint ebo; // OpenGL element buffer object
    GLuint size; // Size of data to be drawn
} Model;


// Create the geometry of a square with sides of length 1 centered at (0, 0, 0) and facing (0, 0, 1)
Model *CreateSquare(void){

    // The construction does *not* use shared vertices
    // 9 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3)
    GLfloat vertex[] = {
         // First triangle
		  0.5,  0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,
          0.5, -0.5,  -0.5,    0.0,  0.0,  -1.0,    0.0, 1.0, 0.0,
		 -0.5, -0.5,  -0.5,    0.0,  0.0,  -1.0,    0.0, 0.0, 1.0,
		 // Second triangle
		 -0.5, -0.5,  -0.5,    0.0,  0.0,  -1.0,    0.0, 0.0, 1.0,
		 -0.5,  0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 1.0, 0.0,
		  0.5,  0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,
		 
		 // 3rd triangle
		 0.5,   0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0, 
		 0.5,   0.5,   0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 1.0, 
		 0.5,  -0.5,  -0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0, 
		 // 4th triangle
		 0.5,   -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 1.0, 
		 0.5,  -0.5,  -0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0, 
		 0.5,   0.5,   0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 1.0, 
		 
		 // 5th triangle
		 -0.5,   0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 1.0, 
		 -0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    0.1, 0.1, 0.1, 
		  0.5,   0.5,  0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 1.0, 
		 // 6th triangle
		  0.5,   0.5,  0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 1.0, 
		 -0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    0.1, 0.1, 0.1, 
		  0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 1.0, 
		  

		 // 7 triangle
		 -0.5,  0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0, 
		 -0.5, -0.5,  -0.5,    0.0,  0.0,  1.0,    0.0, 0.0, 1.0, 
		 -0.5,  0.5,   0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 1.0, 
		 // 8 triangle
		 -0.5,   0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 1.0, 
		 -0.5,  -0.5, -0.5,    0.0,  0.0,  1.0,    0.0, 0.0, 1.0, 
		 -0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    0.1, 0.1, 0.1, 
		 
		 // 9 triangle
		  0.5,  -0.5,  -0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0,
		 -0.5,  -0.5,   0.5,    0.0,  0.0,  1.0,    0.1, 0.1, 0.1,
		 -0.5,  -0.5,  -0.5,    0.0,  0.0,  1.0,    0.0, 0.0, 1.0,
		 // 10 triangle
		  0.5,  -0.5, -0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0, 
		  0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 1.0, 
		 -0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    0.1, 0.1, 0.1, 
		 
		 // 11 triangle
		  0.5,  0.5, -0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0,  
		 -0.5,  0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 1.0,  
		  0.5,  0.5,  0.5,    0.0,  0.0,  1.0,    0.0, 1.0, 1.0,  

		 // 12 triangle
		 -0.5,  0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0, 
		 -0.5,  0.5,   0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 1.0, 
		  0.5,  0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0, 
    };

    // Create model 
    Model *model;
    model = new Model;
    model->size = sizeof(vertex) / (sizeof(GLfloat)*9); 

    // Create OpenGL vertex buffer for model
    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

    return model;
}


// Create the geometry for a torus centered at (0, 0, 0) and facing (0, 0, 1)
Model *CreateTorus(float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30) {

	// Create a torus
	// The torus is built from a large loop with small circles around the loop
	// Vertices are sampled along the circles

	// Number of vertices and faces to be created
	const GLuint vertex_num = num_loop_samples * num_circle_samples;
	const GLuint face_num = num_loop_samples * num_circle_samples * 2;

	// Number of attributes for vertices and faces
	const int vertex_att = 9;  // 9 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3)
	const int face_att = 3; // Vertex indices (3)

	// Data buffers for the torus
	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att];
		face = new GLuint[face_num * face_att];
	}
	catch (std::exception &e) {
		throw e;
	}

	// Create vertices 
	float theta, phi; // Angles for circles
	glm::vec3 loop_center;
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;

	for (int i = 0; i < num_loop_samples; i++) { // large loop

		theta = 2.0*glm::pi<GLfloat>()*i / num_loop_samples; // loop sample (angle theta)
		loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

		for (int j = 0; j < num_circle_samples; j++) { // small circle

			phi = 2.0*glm::pi<GLfloat>()*j / num_circle_samples; // circle sample (angle phi)

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
			vertex_position = loop_center + vertex_normal * circle_radius;
			vertex_color = glm::vec3(1.0 - ((float)i / (float)num_loop_samples),
				(float)i / (float)num_loop_samples,
				(float)j / (float)num_circle_samples);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i*num_circle_samples + j)*vertex_att + k] = vertex_position[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 6] = vertex_color[k];
			}
		}
	}

	// Create triangles
	for (int i = 0; i < num_loop_samples; i++) {
		for (int j = 0; j < num_circle_samples; j++) {
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_loop_samples)*num_circle_samples + j,
				i*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + j);
			glm::vec3 t2(((i + 1) % num_loop_samples)*num_circle_samples + j,
				((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + ((j + 1) % num_circle_samples));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(i*num_circle_samples + j)*face_att * 2 + k] = (GLuint)t1[k];
				face[(i*num_circle_samples + j)*face_att * 2 + k + face_att] = (GLuint)t2[k];
			}
		}
	}

	// Create model 
	Model *model;
	model = new Model;
	model->size = face_num * face_att;

	// Create OpenGL buffer for vertices
	glGenBuffers(1, &model->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	// Create OpenGL buffer for faces
	glGenBuffers(1, &model->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Free data buffers
	delete[] vertex;
	delete[] face;

	return model;
}




// Main function that builds and runs the game
int main(void){

    try {
        // Initialize the window management library (GLFW)
        if (!glfwInit()){
            throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
        }

        // Create a window and its OpenGL context
        GLFWwindow* window;
        window = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
        if (!window){
            glfwTerminate();
            throw(std::runtime_error(std::string("Could not create window")));
        }

        // Make the window's OpenGL context the current one
        glfwMakeContextCurrent(window);

        // Initialize the GLEW library to access OpenGL extensions
        // Need to do it after initializing an OpenGL context
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK){
            throw(std::runtime_error(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
        }

        // Set up z-buffer for rendering
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Set culling of back faces
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        // Set current view 
        glm::mat4 view_matrix = glm::lookAt(camera_position_g, camera_look_at_g, camera_up_g);

        // Set projection
        float top = tan((camera_fov_g/2.0)*(glm::pi<float>()/180.0))*camera_near_clip_distance_g;
        float right = top * window_width_g/window_height_g;
        glm::mat4 projection_matrix = glm::frustum(-right, right, -top, top, camera_near_clip_distance_g, camera_far_clip_distance_g);

        // Create the shapes to be displayed
        Model *square = CreateSquare();
        Model *torus = CreateTorus();

        // **** Set up shaders

        // Create a shader from vertex program source code
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &source_vp, NULL);
        glCompileShader(vs);

        // Check if shader compiled successfully
        GLint status;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE){
            char buffer[512];
            glGetShaderInfoLog(vs, 512, NULL, buffer);
            throw(std::ios_base::failure(std::string("Error compiling vertex shader: ")+std::string(buffer)));
        }

        // Create a shader from the fragment program source code
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &source_fp, NULL);
        glCompileShader(fs);

        // Check if shader compiled successfully
        glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE){
            char buffer[512];
            glGetShaderInfoLog(fs, 512, NULL, buffer);
            throw(std::ios_base::failure(std::string("Error compiling fragment shader: ")+std::string(buffer)));
        }

        // Create a shader program linking both vertex and fragment shaders
        // together
        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        // Check if shaders were linked successfully
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE){
            char buffer[512];
            glGetShaderInfoLog(program, 512, NULL, buffer);
            throw(std::ios_base::failure(std::string("Error linking shaders: ")+std::string(buffer)));
        }

        // Delete memory used by shaders, since they were already compiled
        // and linked
        glDeleteShader(vs);
        glDeleteShader(fs);

        // Set event callbacks for the window
        glfwSetWindowUserPointer(window, (void *) &projection_matrix);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetFramebufferSizeCallback(window, ResizeCallback);

        // Transformation state of square
        glm::mat4 square_translation = glm::translate(glm::mat4(1.0), glm::vec3(-0.3, 0.0, 0.0)); // Translation of square
        glm::mat4 square_scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)); // Scale of square

        // Transformation state of torus
        glm::mat4 torus_translation = glm::translate(glm::mat4(1.0), glm::vec3(0.3, 0.0, 0.0)); // Translation of square
        glm::mat4 torus_scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)); // Scale of torus

        // Get time in seconds at the beginning of execution
        last_time = glfwGetTime();

        // Initialize animation angle
        float current_angle = 0.0;

        // Initialize rotation that we will apply to the shapes
        glm::mat4 rotation(1.0); 

        // Run the main loop
        while (!glfwWindowShouldClose(window)){
            // Clear background
            glClearColor(viewport_background_color_g[0], 
                         viewport_background_color_g[1],
                         viewport_background_color_g[2], 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Animate the scene
            if (animating){
                // Check how much time elapsed since the beginning of
                // execution
                double current_time = glfwGetTime(); // Get time in seconds
                double elapsed_time = current_time - last_time;
                last_time = current_time;

                // Set rotation for shapes
                current_angle += angle*elapsed_time;
                current_angle = fmod(current_angle, 2.0f*glm::pi<float>());
                rotation = glm::rotate(glm::mat4(1.0), current_angle, glm::vec3(0.0, 0.0, 1.0));
            }

            // Select proper shader program to use
            glUseProgram(program);

            // Set view matrix in shader
            GLint view_mat = glGetUniformLocation(program, "view_mat");
            glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix));
            
            // Set projection matrix in shader
            GLint projection_mat = glGetUniformLocation(program, "projection_mat");
            glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix));

            // **** Draw the square

            // Set up vertex data buffer
            glBindBuffer(GL_ARRAY_BUFFER, square->vbo);

            // Set attributes for shader
            // Should be consistent with how we created the buffers for the model
            GLint vertex_att = glGetAttribLocation(program, "vertex");
            glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9 *sizeof(GLfloat), 0);
            glEnableVertexAttribArray(vertex_att);

            GLint normal_att = glGetAttribLocation(program, "normal");
            glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9 *sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
            glEnableVertexAttribArray(normal_att);

            GLint color_att = glGetAttribLocation(program, "color");
            glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
            glEnableVertexAttribArray(color_att);



            // Set world transformation matrix for square in shader
            glm::mat4 transf = square_translation * rotation * square_scaling;
            GLint world_mat = glGetUniformLocation(program, "world_mat");
            glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

            // Draw square mesh (draw only vertex buffer)
            glDrawArrays(GL_TRIANGLES, 0, square->size);





            // **** Draw the torus

            // Set up data buffers
            glBindBuffer(GL_ARRAY_BUFFER, torus->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, torus->ebo);

            // Set attributes for shader
            // Should be consistent with how we created the buffers for the model
            vertex_att = glGetAttribLocation(program, "vertex");
            glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), 0);
            glEnableVertexAttribArray(vertex_att);

            normal_att = glGetAttribLocation(program, "normal");
            glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
            glEnableVertexAttribArray(normal_att);

            color_att = glGetAttribLocation(program, "color");
            glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
            glEnableVertexAttribArray(color_att);



            // Set world transformation matrix for torus in shader
            transf = torus_translation * rotation * torus_scaling;
            world_mat = glGetUniformLocation(program, "world_mat");
            glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

            // Draw torus mesh using both vertex and element buffers
            glDrawElements(GL_TRIANGLES, torus->size, GL_UNSIGNED_INT, 0);

            // Update other events like input handling
            glfwPollEvents();

            // Push buffer drawn in the background onto the display
            glfwSwapBuffers(window);
        }
    }
    catch (std::exception &e){
        PrintException(e);
    }

    return 0;
}
