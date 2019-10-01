/*
 *
 * Used online reference to get outlines, I have an idea of how it works, though I am not an expert. Shader stuff I already knew from last year. 
 * Outline Reference: https://www.codeproject.com/Articles/8499/Generating-Outlines-in-OpenGL
 *
 */

//Set cool = true for slightly increased coolness
const bool cool = false;

#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>



# define FACE_SET 0
# define INDEXED_FACE_SET 1

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

// Globals that define the OpenGL window and viewport 
const std::string window_title_g = "BOOM BOOM!";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);

// Globals that define the OpenGL camera view and projection
glm::vec3 camera_position_g(0.0, 0.0, 5.0); // Position of camera
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0); // Point looking at
glm::vec3 camera_up_g(0.0, 1.0, 0.0); // Up vector
float camera_near_clip_distance_g = 0.01; // Near clipping plane
float camera_far_clip_distance_g = 1000.0; // Far clipping plane
float camera_fov_g = 30.0; // Field-of-view for projection


// Callback for when a key is pressed
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Quit the program when pressing 'q'
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
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


// Create the geometry for a torus
Model *CreateCylinder(float cylinder_radius = .5, float cylinder_height = 1.0, int num_samples = 11) {

	// Create a torus
	// The torus is built from a large loop with small circles around the loop
	// Vertices are sampled along the circles
	const int num_circle_samples = 2;
	// Number of vertices and faces to be created
	const GLuint vertex_num = num_samples * num_circle_samples * 2;
	const GLuint face_num = num_samples * num_circle_samples * 2 + (num_samples - 2) * 2;

	// Number of attributes for vertices and faces
	//const int vertex_att = 11;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
	const int vertex_att = 9;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3)
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
	//glm::vec2 vertex_coord;

	//used for calculating endcaps
	glm::vec3 alt_normal;
	int alt_start = num_samples * num_circle_samples;
	//------------------------------------------------------------------------------------------------------
	//
	//												Define Verts:
	//
	//------------------------------------------------------------------------------------------------------
	for (int i = 0; i < num_samples; i++) { // large loop

		theta = 2.0*glm::pi<GLfloat>()*i / num_samples; // loop sample (angle theta)
		loop_center = glm::vec3(cylinder_radius*cos(theta), -.5f * cylinder_height, cylinder_radius*sin(theta)); // centre of a small circle

		for (int j = 0; j < num_circle_samples; j++) { // small circle

			phi = .5*glm::pi<GLfloat>() + 2.0*glm::pi<GLfloat>()*j / num_circle_samples; // circle sample (angle phi)

			// Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta)*cos(phi), 0.0f, sin(theta)*cos(phi));
			vertex_position = loop_center + glm::vec3(0.0f, j * cylinder_height, 0.0f);
			vertex_color = glm::vec3(1.0 - ((float)i / (float)num_samples),
				(float)i / (float)num_samples,
				(float)j);

			alt_normal = cos((float)j * glm::pi<GLfloat>()) * glm::vec3(0.0f, 1.0f, 0.0f);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i*num_circle_samples + j)*vertex_att + k] = vertex_position[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 6] = vertex_color[k];

				vertex[alt_start * vertex_att + (i*num_circle_samples + j)*vertex_att + k] = vertex_position[k];
				vertex[alt_start * vertex_att + (i*num_circle_samples + j)*vertex_att + k + 3] = alt_normal[k];
				vertex[alt_start * vertex_att + (i*num_circle_samples + j)*vertex_att + k + 6] = vertex_color[k];
			}
		}
	}
	//------------------------------------------------------------------------------------------------------
	//
	//												Define Sides:
	//
	//------------------------------------------------------------------------------------------------------
	// Create triangles
	for (int i = 0; i < num_samples; i++) {

		int j = 1;

		glm::vec3 t1, t2;
		if (cool)
		{
			t1 = glm::vec3(
				((i + 1) % num_samples)*num_circle_samples + j,
				i*num_circle_samples + j,
				i*num_circle_samples + ((j + 1) % num_circle_samples));
		}
		else 
		{
			t1 = glm::vec3(
				((i + 1) % num_samples)*num_circle_samples + j,
				i*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + j);
		}

		t2 = glm::vec3(
			((i + 1) % num_samples)*num_circle_samples + j,
			((i + 1) % num_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
			i*num_circle_samples + ((j + 1) % num_circle_samples));

		// Add two triangles to the data buffer
		for (int k = 0; k < 3; k++) {
			face[(i)*face_att * 2 + k] = (GLuint)t1[k];
			face[(i)*face_att * 2 + k + face_att] = (GLuint)t2[k];
		}
	}


	//------------------------------------------------------------------------------------------------------
	//
	//												Define Endcaps:
	//
	//------------------------------------------------------------------------------------------------------
	//Normals for sides is calculated properly, endcaps will have normals of +-1 * (0.0f, 1.0f, 0.0f) to account for the sharp corner
	
	int alt_face_start = num_samples * face_att * 2;
	for (int i = 0; i < num_samples - 2; i++)
	{
		glm::vec3 t1(
			alt_start,
			alt_start + (i + 1) * 2,
			alt_start + (i + 2) * 2
		);

		glm::vec3 t2(
			alt_start + 1,
			alt_start + (i + 2) * 2 + 1,
			alt_start + (i + 1) * 2 + 1
		);

		for (int k = 0; k < 3; k++)
		{
			face[alt_face_start + i * face_att * 2 + k] = (GLuint)t1[k];
			face[alt_face_start + i * face_att * 2 + face_att + k] = (GLuint)t2[k];
		}
	}


	//------------------------------------------------------------------------------------------------------
	//
	//												Define Models:
	//
	//------------------------------------------------------------------------------------------------------
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







void BindBuffers(int type, Model* m)
{
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	if (type == FACE_SET)
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
	std::string vp_str = LoadString("D:\\COMP3501\\Assignments\\Assignment_02\\vertex_shader.glsl");
	std::string fp_str = LoadString("D:\\COMP3501\\Assignments\\Assignment_02\\fragment_shader.glsl");

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


        // **** Set up shaders

        // Create a shader from vertex program source code

		GLuint program;
		CreateProgram(program);

        // Set attributes for shaders
        // Should be consistent with how we created the buffers for the tester

        // Set event callbacks for the window
        glfwSetWindowUserPointer(window, (void *) &projection_matrix);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetFramebufferSizeCallback(window, ResizeCallback);


		Model* cylinder = CreateCylinder();


		//ISROT
		//Translate to get joint to origin
		//rotate
		//translate joint to end pos for joint
		//
		float yOff = .5f;
		//central structure
		glm::vec3 base_pos(0, .2f - yOff, 0);
		glm::vec3 base_joint(0, 0, 0);
		glm::vec3 base_axis(0, 1, 0);
		float base_rot_speed = glm::pi<float>() / 180.0f;

		glm::mat4 base_translation;
		glm::quat base_orientation;
		glm::mat4 base_orbit;
		glm::mat4 base_scale = glm::scale(glm::mat4(1.0), glm::vec3(.6f, .4f, .6f));



		glm::vec3 stand_pos(0, -.375f, 0);
		//glm::vec3 stand_joint(0, 0, 0);
		//glm::vec3 stand_axis(0, 1, 0);
		//float stand_rot_speed = 0.0f;	//No rotation with respect to base

		glm::mat4 stand_translation;
		//glm::quat stand_orientation;
		//glm::mat4 stand_orbit;
		glm::mat4 stand_scale = glm::scale(glm::mat4(1.0), glm::vec3(.3f, .75f, .3f));




		glm::vec3 outer_pos(0, -.05, 0);	//Position in relation to the base
		glm::vec3 outer_joint(0, -.5f, 0);	//joint located at local 0, -.5, 0
		glm::vec3 outer_flex_axis(1, 0, 0);	//Rotating only about x axis, getting it to tilt down
		//glm::vec3 outer_rot_axis(0, 1, 0);	//Rotating only about z axis, getting it to rotate
		//float outer_rot_speed = -1.0f * glm::pi<float>() / 90.0f;
		float outer_flex_speed = 1.0f;

		glm::mat4 outer_translation;
		//glm::quat outer_orientation;
		glm::mat4 outer_orbit;
		glm::mat4 outer_scale = glm::scale(glm::mat4(1.0), glm::vec3(.1f, 1.0f, .1f));



		glm::vec3 inner_pos(0, 0, 0);	//Position in relation to arm
		//glm::vec3 inner_joint(0, 0, 0);	//joint located at local 0, -.5, 0
		//glm::vec3 inner_flex_axis(1, 0, 0);	//Rotating only about x axis, getting it to tilt down
		//glm::vec3 inner_rot_axis(0, 1, 0);	//Rotating only about z axis, getting it to rotate
		//float inner_rot_speed = 2.0f * glm::pi<float>() / 90.0f;
		float inner_move_speed = 1.5f;

		glm::mat4 inner_translation;
		//glm::quat inner_orientation;
		//glm::mat4 inner_orbit;
		glm::mat4 inner_scale = glm::scale(glm::mat4(1.0), glm::vec3(.05f, .75f, .05f));



		float arm_flex_speed = 1.0f;
		glm::vec3 arm_0_pos(0, .15, 0);	//Position in relation to the base
		glm::vec3 arm_0_joint(0, -.15f, 0);	//joint located at local 0, -.5, 0
		glm::vec3 arm_0_flex_axis(0.0f, 0.0f, 1.0f);	//Rotating only about x axis, getting it to tilt down
		arm_0_flex_axis = glm::normalize(arm_0_flex_axis);
		//glm::vec3 outer_rot_axis(0, 1, 0);	//Rotating only about z axis, getting it to rotate
		//float outer_rot_speed = -1.0f * glm::pi<float>() / 90.0f;
		float arm_0_flex_range = 4.0f;

		glm::mat4 arm_0_translation;
		glm::mat4 arm_0_orbit;
		glm::mat4 arm_0_scale = glm::scale(glm::mat4(1.0), glm::vec3(.1f, .3f, .1f));


		glm::vec3 arm_1_pos(0, .15, 0);	//Position in relation to the base
		glm::vec3 arm_1_joint(0, -.15f, 0);	//joint located at local 0, -.5, 0
		glm::vec3 arm_1_flex_axis(0.0f, 0.0f, 1.0f);	//Rotating only about x axis, getting it to tilt down
		arm_1_flex_axis = glm::normalize(arm_1_flex_axis);
		//glm::vec3 outer_rot_axis(0, 1, 0);	//Rotating only about z axis, getting it to rotate
		//float outer_rot_speed = -1.0f * glm::pi<float>() / 90.0f;
		float arm_1_flex_range = 4.0f;

		glm::mat4 arm_1_translation;
		glm::mat4 arm_1_orbit;
		glm::mat4 arm_1_scale = glm::scale(glm::mat4(1.0), glm::vec3(.1f, .3f, .1f));


		glm::vec3 arm_2_pos(0, .15, 0);	//Position in relation to the base
		glm::vec3 arm_2_joint(0, -.15f, 0);	//joint located at local 0, -.5, 0
		glm::vec3 arm_2_flex_axis(0.0f, 0.0f, 1.0f);	//Rotating only about x axis, getting it to tilt down
		arm_2_flex_axis = glm::normalize(arm_2_flex_axis);
		//glm::vec3 outer_rot_axis(0, 1, 0);	//Rotating only about z axis, getting it to rotate
		//float outer_rot_speed = -1.0f * glm::pi<float>() / 90.0f;
		float arm_2_flex_range = 4.0f;

		glm::mat4 arm_2_translation;
		glm::mat4 arm_2_orbit;
		glm::mat4 arm_2_scale = glm::scale(glm::mat4(1.0), glm::vec3(.1f, .3f, .1f));



		glm::vec3 claw_0_pos(0, 0, 0);	//Position in relation to the base
		glm::vec3 claw_0_joint(0, -.25f, 0);	//joint located at local 0, -.5, 0
		glm::vec3 claw_0_flex_axis(0.0f, 0.0f, 1.0f);	//Rotating only about x axis, getting it to tilt down
		claw_0_flex_axis = glm::normalize(claw_0_flex_axis);
		//glm::vec3 outer_rot_axis(0, 1, 0);	//Rotating only about z axis, getting it to rotate
		//float outer_rot_speed = -1.0f * glm::pi<float>() / 90.0f;
		float claw_0_flex_range = 4.0f;

		glm::mat4 claw_0_translation;
		glm::mat4 claw_0_orbit;
		glm::mat4 claw_0_scale = glm::scale(glm::mat4(1.0), glm::vec3(.1f, .25f, .1f));


		glm::vec3 claw_1_pos(0, 0, 0);	//Position in relation to the base
		glm::vec3 claw_1_joint(0, -.25f, 0);	//joint located at local 0, -.5, 0
		glm::vec3 claw_1_flex_axis(0.0f, 0.0f, -1.0f);	//Rotating only about x axis, getting it to tilt down
		claw_1_flex_axis = glm::normalize(claw_1_flex_axis);
		//glm::vec3 outer_rot_axis(0, 1, 0);	//Rotating only about z axis, getting it to rotate
		//float outer_rot_speed = -1.0f * glm::pi<float>() / 90.0f;
		float claw_1_flex_range = 4.0f;

		glm::mat4 claw_1_translation;
		glm::mat4 claw_1_orbit;
		glm::mat4 claw_1_scale = glm::scale(glm::mat4(1.0), glm::vec3(.1f, .25f, .1f));



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
			
			if (current_time - last_time > .01)
			{
				//Game Logic
				//Animate Base
				glm::quat base_rotation = glm::angleAxis(base_rot_speed, base_axis);
				base_orientation *= base_rotation;
				base_orientation = glm::normalize(base_orientation);

				//Animate Stand (Nothing actually needs to happen here, it is just gonna mimic the Base)
				//glm::quat stand_rotation = glm::angleAxis(stand_rot_speed, stand_axis);
				//stand_orientation *= stand_rotation;
				//stand_orientation = glm::normalize(stand_orientation);

				//Animate Arm
				//glm::quat outer_rotation = glm::angleAxis(outer_rot_speed, outer_rot_axis);
				//outer_orientation *= outer_rotation;
				//outer_orientation = glm::normalize(outer_orientation);
				//from 2 to 2.5
				// = 2.25 + sin(time) * .25
				float amount = 2.5 + sin(glm::pi<float>() * current_time * outer_flex_speed) * .5;
				glm::quat outer_flex = glm::angleAxis(glm::pi<float>() / amount, outer_flex_axis);
				outer_orbit = glm::mat4_cast(outer_flex) * glm::translate(glm::mat4(1.0), -outer_joint);


				//Animate Inner
				//glm::quat inner_rotation = glm::angleAxis(inner_rot_speed, inner_rot_axis);
				//inner_orientation *= inner_rotation;
				//inner_orientation = glm::normalize(inner_orientation);
				inner_pos = glm::vec3(0.0f, .5f + .375f * cos(glm::pi<float>() * current_time * inner_move_speed), 0.0f);


				//4.0 to -4.0
				amount = sin(glm::pi<float>() * (float)current_time);

				//-1/4 to 1/4
				//-.25 + .5
				amount = cos(glm::pi<float>() * current_time * arm_flex_speed) * .25f;
				glm::quat arm_0_flex = glm::angleAxis(glm::pi<float>() * amount, arm_0_flex_axis);
				arm_0_orbit = glm::mat4_cast(arm_0_flex) * glm::translate(glm::mat4(1), -arm_0_joint);
				
				glm::quat arm_1_flex = glm::angleAxis(glm::pi<float>() * amount, arm_1_flex_axis);
				arm_1_orbit = glm::mat4_cast(arm_1_flex) * glm::translate(glm::mat4(1), -arm_1_joint);

				glm::quat arm_2_flex = glm::angleAxis(glm::pi<float>() * amount, arm_2_flex_axis);
				arm_2_orbit = glm::mat4_cast(arm_2_flex) * glm::translate(glm::mat4(1), -arm_2_joint);

				amount = .05f + .20f * abs(sin(glm::pi<float>() * (float)current_time));
				glm::quat claw_0_flex = glm::angleAxis(glm::pi<float>() * amount, claw_0_flex_axis);
				claw_0_orbit = glm::mat4_cast(claw_0_flex) * glm::translate(glm::mat4(1), -claw_0_joint);


				glm::quat claw_1_flex = glm::angleAxis(glm::pi<float>() * amount, claw_1_flex_axis);
				claw_1_orbit = glm::mat4_cast(claw_1_flex) * glm::translate(glm::mat4(1), -claw_1_joint);



				last_time = current_time;
			}

            // **** Draw the tester

            // Select proper shader program to use
            glUseProgram(program);


			//Declare 
			glm::mat4 transf;
			GLint world_mat;
			GLint view_mat;
			GLint projection_mat;

			GLint vertex_att, normal_att, color_att;// , time;

			//Bind Torus Buffer + other attributes
			BindBuffers(INDEXED_FACE_SET, cylinder);
			//Bind Vertex, Normal, Color Attributes
			BindStatic(vertex_att, normal_att, color_att, program);

			view_mat = glGetUniformLocation(program, "view_mat");
			glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix));
			projection_mat = glGetUniformLocation(program, "projection_mat");
			glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix));


			//Declare values to be filled in later
			// Set world transformation matrix for player in shader


			//RENDERING




			//Order of Operations:


			//COMPOSE BASE TRANSLATIONS!!!!!-------------------------------------------------------
			base_translation = glm::translate(glm::mat4(1.0), base_pos);
			glm::mat4 base_matrix = base_translation * glm::mat4_cast(base_orientation);
			//DRAW BASE
			transf = base_matrix * base_scale;
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);


			//COMPOSE STAND TRANSLATIONS!!!!!-------------------------------------------------------
			stand_translation = glm::translate(glm::mat4(1.0), stand_pos);
			//glm::mat4 stand_matrix = base_matrix * stand_translation * glm::mat4_cast(stand_orientation);
			glm::mat4 stand_matrix = base_matrix * stand_translation;
			transf = stand_matrix * stand_scale;
			//DRAW STAND
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);


			//COMPOSE OUTER TRANSLATIONS!!!!!-------------------------------------------------------
			outer_translation = glm::translate(glm::mat4(1.0), outer_pos);
			//glm::mat4 outer_matrix = outer_translation * outer_orbit * glm::mat4_cast(outer_orientation);
			glm::mat4 outer_matrix = outer_translation * outer_orbit;
			transf = base_matrix * outer_matrix * outer_scale * glm::mat4(1);
			//DRAW OUTER
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);


			//COMPOSE INNER TRANSLATIONS!!!!!-------------------------------------------------------
			inner_translation = glm::translate(glm::mat4(1.0), inner_pos);
			//glm::mat4 inner_matrix = inner_translation * inner_orbit * glm::mat4_cast(inner_orientation);
			glm::mat4 inner_matrix = inner_translation;
			transf = base_matrix * outer_matrix * inner_matrix * inner_scale * glm::mat4(1);
			//DRAW INNER
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);


			//COMPOSE ARM_0 TRANSLATIONS!!!!!-------------------------------------------------------
			arm_0_translation = glm::translate(glm::mat4(1.0), arm_0_pos);
			glm::mat4 arm_0_matrix = arm_0_translation * arm_0_orbit;
			transf = base_matrix * arm_0_matrix * arm_0_scale * glm::mat4(1);
			//DRAW ARM_0
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);


			//COMPOSE ARM_1 TRANSLATIONS!!!!!-------------------------------------------------------
			arm_1_translation = glm::translate(glm::mat4(1.0), arm_1_pos);
			glm::mat4 arm_1_matrix = arm_1_translation * arm_1_orbit;
			transf = base_matrix * arm_0_matrix * arm_1_matrix * arm_1_scale * glm::mat4(1);
			//DRAW ARM_1
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);



			//COMPOSE ARM_2 TRANSLATIONS!!!!!-------------------------------------------------------
			arm_2_translation = glm::translate(glm::mat4(1.0), arm_2_pos);
			glm::mat4 arm_2_matrix = arm_2_translation * arm_2_orbit;
			transf = base_matrix * arm_0_matrix * arm_1_matrix * arm_2_matrix * arm_2_scale * glm::mat4(1);
			//DRAW ARM_2
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);



			//COMPOSE CLAW_0 TRANSLATIONS!!!!!-------------------------------------------------------
			claw_0_translation = glm::translate(glm::mat4(1.0), claw_0_pos);
			glm::mat4 claw_0_matrix = claw_0_translation * claw_0_orbit;
			transf = base_matrix * arm_0_matrix * arm_1_matrix * arm_2_matrix * claw_0_matrix * claw_0_scale * glm::mat4(1);
			//DRAW CLAW_0
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);

			//COMPOSE CLAW_1 TRANSLATIONS!!!!!-------------------------------------------------------
			claw_1_translation = glm::translate(glm::mat4(1.0), claw_1_pos);
			glm::mat4 claw_1_matrix = claw_1_translation * claw_1_orbit;
			transf = base_matrix * arm_0_matrix * arm_1_matrix * arm_2_matrix * claw_1_matrix * claw_1_scale * glm::mat4(1);
			//DRAW CLAW_1
			world_mat = glGetUniformLocation(program, "world_mat");
			glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
			glDrawElements(GL_TRIANGLES, cylinder->size, GL_UNSIGNED_INT, 0);


			//END RENDERING

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

