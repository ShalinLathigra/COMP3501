/*
 *
 * Used online reference to get outlines, I have an idea of how it works, though I am not an expert. Shader stuff I already knew from last year. 
 * Outline Reference: https://www.codeproject.com/Articles/8499/Generating-Outlines-in-OpenGL
 *
 */
#include "Includes.h"
#include "Entity.h"
#include "GameManager.h"

#include <fstream>
#include <sstream>

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

#define MODEL_TORUS 1
#define MODEL_CUBE 0

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

//Declare Game manager so KeyCallback doesn't freak out
GameManager* gm;

// Callback for when a key is pressed
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Quit the program when pressing 'q'
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
	//Pass input to GM
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

void BindBuffers(int type, Model* m)
{
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	if (type == MODEL_TORUS)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
	}

}

void BindStatic(GLint &vertex_att, GLint &normal_att, GLint &color_att, GLuint &program)
{
	vertex_att = glGetAttribLocation(program, "vertex");
	glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(vertex_att);

	normal_att = glGetAttribLocation(program, "normal");
	glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(normal_att);

	color_att = glGetAttribLocation(program, "color");
	glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(color_att);
}

void ClearStencil()
{
	//Clear all Stencil info
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	// Set the clear value for the stencil buffer, then clear it
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	// Set the stencil buffer to write a 1 in every time
	// a pixel is written to the screen
	glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// Render the object in black
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void SetStencil()
{
	//Set stencil info for outlines
	glStencilFunc(GL_NOTEQUAL, 1, 0xFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// Draw the object with thick lines
	glLineWidth(5.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

std::string LoadString(const char * filepath)
{
	std::string string;

	std::ifstream infile(filepath);

	std::string line;
	while (std::getline(infile, line))
		string.append(line + "\n");

	infile.close();

	return string;
}

void CreateProgram(GLuint & program)
{
	std::string vp_str = LoadString("D:\\COMP3501\\Assignments\\Assignment_01\\vertex_shader.glsl");
	std::string fp_str = LoadString("D:\\COMP3501\\Assignments\\Assignment_01\\fragment_shader.glsl");

	const char* source_vp = vp_str.c_str();
	const char* source_fp = fp_str.c_str();

	// Create a shader from vertex program source code

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &source_vp, NULL);
	glCompileShader(vs);

	// Check if shader compiled successfully
	GLint status;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vs, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error compiling vertex shader: ") + std::string(buffer)));
	}

	// Create a shader from the fragment program source code
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &source_fp, NULL);
	glCompileShader(fs);

	// Check if shader compiled successfully
	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fs, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error compiling fragment shader: ") + std::string(buffer)));
	}

	// Create a shader program linking both vertex and fragment shaders
	// together
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// Check if shaders were linked successfully
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(program, 512, NULL, buffer);
		throw(std::ios_base::failure(std::string("Error linking shaders: ") + std::string(buffer)));
	}

	// Delete memory used by shaders, since they were already compiled
	// and linked
	glDeleteShader(vs);
	glDeleteShader(fs);
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



        // Initialize Game Objects + Models
		gm = new GameManager();
		Model* torus = Entity::CreateTorus(.6f, .05f, 7, 4);
		Model* cube = Entity::CreateCube();

		Entity* player = new Entity(glm::vec3(0,0,0), glm::vec3(.5,.5,.5));
		player->SetAxis(glm::vec3(0.0f, 0.0f, 1.0f));
		player->SetRotSpeed(240.0f);	// Degrees per second
		gm->AddPlayer(player);


        // **** Set up shaders

        // Create a shader from vertex program source code

		GLint status;
		GLuint program;
		CreateProgram(program);

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

			//Update all objects, pass in deltaTime
			gm->Update(current_time - last_time);


			last_time = current_time;

            // **** Draw the tester

            // Select proper shader program to use
            glUseProgram(program);


			//Declare 
			glm::mat4 transf;
			GLint world_mat;
			GLint view_mat;
			GLint projection_mat;

			GLint vertex_att, normal_att, color_att, z, isColored;// , time;


			// Set view matrix in shader
			view_mat = glGetUniformLocation(program, "view_mat");
			glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix));

			// Set projection matrix in shader
			projection_mat = glGetUniformLocation(program, "projection_mat");
			glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix));



			//Clear Stencil information used for drawing the last outline
			ClearStencil();

			//Bind Torus Buffer + other attributes
			BindBuffers(MODEL_TORUS, torus);
			//Bind Vertex, Normal, Color Attributes
			BindStatic(vertex_att, normal_att, color_att, program);

			//Declare values to be filled in later
			// Set world transformation matrix for player in shader
			glm::vec3 offset(0);
			for (int i = 0; i < 5; i++)
			{
				// Set z offset used for drawing the path to Torus
				offset.z = -i * 2.0f;
				transf = glm::translate(glm::mat4(1), gm->GetEntity(gm->GetCount())->GetPosition() + offset) * glm::mat4_cast(gm->GetEntity(gm->GetCount())->GetOrientation()) * glm::scale(glm::mat4(1.0), gm->GetEntity(gm->GetCount())->GetScale());
				world_mat = glGetUniformLocation(program, "world_mat");
				glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));


				z = glGetUniformLocation(program, "z");
				isColored = glGetUniformLocation(program, "isColored");
				//time = glGetUniformLocation(program, "time");
				//Player colour will always be white
				glUniform1f(isColored, 0);
				//glUniform1f(time, current_time);

				//Bind z value, used to calculate colour. See top comment for why.
				if (i == 0)
				{
					glUniform1f(z, gm->GetEntity(gm->GetCount())->GetPosition().z + offset.z);
				}
				else 
				{
					glUniform1f(z, -8.0f + (gm->GetEntity(gm->GetCount())->GetPosition().z / 8.0f));
				}

				//Draw Torus
				glDrawElements(GL_TRIANGLES, torus->size, GL_UNSIGNED_INT, 0);
			}

			//Bind Cube buffer, re-bind all other attributes
			BindBuffers(MODEL_CUBE, cube);

			BindStatic(vertex_att, normal_att, color_att, program);

			//Render all cubes
			for (int i = 0; i < gm->GetCount(); i++)
			{
				// Set world transformation matrix for tester in shader
				transf = gm->GetEntity(i)->GenerateMatrix();
				world_mat = glGetUniformLocation(program, "world_mat");
				glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

				z = glGetUniformLocation(program, "z");
				isColored = glGetUniformLocation(program, "isColored");
				glUniform1f(z, gm->GetEntity(i)->GetPosition().z);
				glUniform1f(isColored, 1);
				
				//Turn off Stencil Info
				ClearStencil();
				//Draw Arrays
				glDrawArrays(GL_TRIANGLES, 0, cube->size);

				//Turn off Color for outline
				glUniform1f(isColored, 0);
				//Enable Outline Drawing
				SetStencil();
				//Draw outlines
				glDrawArrays(GL_TRIANGLES, 0, cube->size);

			}

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
