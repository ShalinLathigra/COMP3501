/*
 *
 * A program that demonstrates how to create and render a torus mesh
 *
 * Copyright (c) 2018 Oliver van Kaick <Oliver.vanKaick@carleton.ca>, David Mould <mould@scs.carleton.ca>
 *
 */
#include "Includes.h"
#include "Entity.h"
#include "GameManager.h"

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl


// Globals that define the OpenGL window and viewport
const std::string window_title_g = "Claw Game but it's a Septagon";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);

// Globals that define the OpenGL camera view and projection
glm::vec3 camera_position_g(0.0, 0.0, 3.0); // Position of camera
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0); // Point looking at
glm::vec3 camera_up_g(0.0, 1.0, 0.0); // Up vector
float camera_near_clip_distance_g = 0.01; // Near clipping plane
float camera_far_clip_distance_g = 1000.0; // Far clipping plane
float camera_fov_g = 30.0; // Field-of-view for projection


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
uniform float z;\n\
uniform float zvel;\n\
\n\
// Attributes forwarded to the fragment shader\n\
out vec4 color_interp;\n\
\n\
\n\
void main()\n\
{\n\
\n\
vec3 w = vec3(1.0, 1.0, 1.0);\n\
vec3 r = vec3(1.0, 0.0, 0.0);\n\
vec3 o = vec3(1.0, 0.5, 0.0);\n\
vec3 y = vec3(1.0, 1.0, 0.0);\n\
vec3 g = vec3(0.0, 1.0, 0.0);\n\
vec3 b = vec3(0.0, 0.0, 1.0);\n\
vec3 p = vec3(0.6, 0.0, 1.0);\n\
vec3 end;\n\
\n\
\n\
\n\
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);\n\
	if (zvel > 0.0){\n\
		end = vec3(.5, .5, .5);\n\
	}\n\
    else if (zvel <= 0)\n\
	{\n\
		if (z > -1){\n\
			end = (1 + z) * w + (-z) * r;\n\
		} else if (z > -2){\n\
			end = (2 + z) * r + (-z - 1) * o;\n\
		} else if (z > -3){\n\
			end = (3 + z) * o + (-z - 2) * y;\n\
		} else if (z > -4){\n\
			end = (4 + z) * y + (-z - 3) * g;\n\
		} else if (z > -5){\n\
			end = (5 + z) * g + (-z - 4) * b;\n\
		} else if (z > -4){\n\
			end = (6 + z) * b + (-z - 5) * p;\n\
		} else {\n\
			end = p;\n\
		}\n\
	}\n\
	\n\
	end = (9 + z)/8.0 * end;\n\
    color_interp = vec4(end, 1.0);\n\
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


// Store information of one model for rendering

GameManager* gm;

// Callback for when a key is pressed
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Quit the program when pressing 'q'
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
	
	gm->HandleInput(key);
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



        // Initialize Game Objects
		gm = new GameManager();
		Model* torus = Entity::CreateTorus(.6f, .1f, 7, 4);
		Model* cube = Entity::CreateCube();

		Entity* player = new Entity(glm::vec3(0,0,0), glm::vec3(.5,.5,.5));
		//player->SetModelCube();
		player->SetAxis(glm::vec3(0.0f, 0.0f, 1.0f));
		player->SetRotSpeed(.05f);
		gm->AddPlayer(player);


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

        // Set attributes for shaders
        // Should be consistent with how we created the buffers for the tester

        // Set event callbacks for the window
        glfwSetWindowUserPointer(window, (void *) &projection_matrix);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetFramebufferSizeCallback(window, ResizeCallback);

        // Run the main loop
        while (!glfwWindowShouldClose(window)){
            // Clear background
            glClearColor(viewport_background_color_g[0], 
                         viewport_background_color_g[1],
                         viewport_background_color_g[2], 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Animate the scene
                // Check how much time elapsed since the last update
			static double last_time = 0;
			double current_time = glfwGetTime(); // Get time in seconds
			//Game Logic

			gm->Update(current_time - last_time);


			last_time = current_time;

            // **** Draw the tester

            // Select proper shader program to use
            glUseProgram(program);


			glm::mat4 transf;
			GLint world_mat;
			GLint view_mat;
			GLint projection_mat;



			// Set view matrix in shader
			view_mat = glGetUniformLocation(program, "view_mat");
			glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix));

			// Set projection matrix in shader
			projection_mat = glGetUniformLocation(program, "projection_mat");
			glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix));

			glBindBuffer(GL_ARRAY_BUFFER, torus->vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, torus->ebo);




			GLint vertex_att = glGetAttribLocation(program, "vertex");
			glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
			glEnableVertexAttribArray(vertex_att);

			GLint normal_att = glGetAttribLocation(program, "normal");
			glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(normal_att);

			GLint color_att = glGetAttribLocation(program, "color");
			glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(color_att);

					   
			// Set world transformation matrix for player in shader
			transf = gm->GetEntity(gm->GetCount())->GenerateMatrix();
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));


			GLint z = glGetUniformLocation(program, "z");
			GLint zvel = glGetUniformLocation(program, "zvel");
			glUniform1f(z, gm->GetEntity(gm->GetCount())->GetPosition().z);
			glUniform1f(zvel, gm->GetEntity(gm->GetCount())->GetVelocity().z);

			glDrawElements(GL_TRIANGLES, torus->size, GL_UNSIGNED_INT, 0);




			glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);


			vertex_att = glGetAttribLocation(program, "vertex");
			glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
			glEnableVertexAttribArray(vertex_att);

			normal_att = glGetAttribLocation(program, "normal");
			glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(normal_att);

			color_att = glGetAttribLocation(program, "color");
			glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(color_att);

			for (int i = 0; i < gm->GetCount(); i++)
			{

				// Set world transformation matrix for tester in shader
				transf = gm->GetEntity(i)->GenerateMatrix();
				world_mat = glGetUniformLocation(program, "world_mat");
				glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));



				z = glGetUniformLocation(program, "z");
				zvel = glGetUniformLocation(program, "zvel");
				glUniform1f(z, gm->GetEntity(i)->GetPosition().z);
				glUniform1f(zvel, 0);
				
				glDrawArrays(GL_TRIANGLES, 0, cube->size);

			}


			//std::cout << std::endl;

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
