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
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


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
        -0.5, -0.5,  0.0,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0,
         0.5, -0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 1.0, 0.0,
         0.5,  0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 0.0, 1.0,
        // Second triangle
         0.5,  0.5,  0.0,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
        -0.5,  0.5,  0.0,    0.0,  0.0,  1.0,    0.0, 1.0, 1.0,
        -0.5, -0.5,  0.0,    0.0,  0.0,  1.0,    0.1, 0.0, 1.0,
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


// Create the geometry for a star centered at (0, 0, 0) and facing (0, 0, 1)
Model *CreateStar(void){

    // The construction uses shared vertices 

    // Vertices that form the star
    // 9 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3)
    GLfloat vertex[] = {
        // Center vertex
         0.0,  0.0,  0.0,    0.0,  0.0,  1.0,    0.0,  0.0,  1.0,
        // Corners
         0.0,  0.5,  0.0,    0.0,  0.0,  1.0,    1.0,  0.0,  0.0,
        -0.1,  0.1,  0.0,    0.0,  0.0,  1.0,    0.0,  1.0,  0.0,
        -0.5,  0.0,  0.0,    0.0,  0.0,  1.0,    1.0,  0.0,  0.0,
        -0.1, -0.1,  0.0,    0.0,  0.0,  1.0,    0.0,  1.0,  0.0,
         0.0, -0.5,  0.0,    0.0,  0.0,  1.0,    1.0,  0.0,  0.0,
         0.1, -0.1,  0.0,    0.0,  0.0,  1.0,    0.0,  1.0,  0.0,
         0.5,  0.0,  0.0,    0.0,  0.0,  1.0,    1.0,  0.0,  0.0,
         0.1,  0.1,  0.0,    0.0,  0.0,  1.0,    0.0,  1.0,  0.0,
    };

    // Faces that form the star
    // Every three indices define one triangle
    GLuint face[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 7,
        0, 7, 8,
        0, 8, 1,
    };

    // Create model
    Model *model;
    model = new Model;
    model->size = sizeof(face) / sizeof(GLuint);

    // Create OpenGL buffer for vertices
    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

    // Create OpenGL buffer for faces
    glGenBuffers(1, &model->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face), face, GL_STATIC_DRAW);

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
        Model *star = CreateStar();

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

        // Transformation state of star
        glm::mat4 star_translation = glm::translate(glm::mat4(1.0), glm::vec3(0.3, 0.0, 0.0)); // Translation of square
        glm::mat4 star_scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)); // Scale of star

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
                rotation = glm::rotate(glm::mat4(1.0), current_angle, glm::vec3(0.0, 1.0, 0.0));
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
            glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), 0);
            glEnableVertexAttribArray(vertex_att);

            GLint normal_att = glGetAttribLocation(program, "normal");
            glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
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

            // **** Draw the star

            // Set up data buffers
            glBindBuffer(GL_ARRAY_BUFFER, star->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star->ebo);

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

            // Set world transformation matrix for star in shader
            transf = star_translation * rotation * star_scaling;
            world_mat = glGetUniformLocation(program, "world_mat");
            glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

            // Draw star mesh using both vertex and element buffers
            glDrawElements(GL_TRIANGLES, star->size, GL_UNSIGNED_INT, 0);

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
