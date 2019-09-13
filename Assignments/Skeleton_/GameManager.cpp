#include "GameManager.h"
#include <iostream>

//*********************************************************************
//
//			Initializing Everything
//
//*********************************************************************
GameManager::GameManager()
{
	models = std::vector<Model*>();
	objects = std::vector<GameObject*>();

}


GameManager::~GameManager()
{
}


void GameManager::InitializeModels(void)
{
	models.push_back(CreateCube());
	models.push_back(CreateTorus(.5f, .05f, 7, 3));
}
void GameManager::SetupShaders(void)
{
	source_fp = "\n\
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
	source_vp = "#version 130\n\
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


	// Create a shader from vertex program source code
	vs = glCreateShader(GL_VERTEX_SHADER);
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
	fs = glCreateShader(GL_FRAGMENT_SHADER);
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

//*************************************************************
//
//		Interactions with Object List
//
//*************************************************************
void GameManager::Add(GameObject* obj)
{
	objects.push_back(obj);
}
void GameManager::Add(std::vector<GameObject*> objSet)
{

	for (std::vector<GameObject*>::iterator iter = objSet.begin(); iter != objSet.end(); iter++)
	{
		objects.push_back(*iter);
	}
}
void GameManager::Delete(int id)
{
	std::vector<GameObject*>::iterator iter;
	for (iter = objects.begin(); iter != objects.end(); iter++)
	{
		if ((*iter)->GetID() == id)
		{
			break;
		}
	}
	if (iter != objects.end())
	{
		objects.erase(iter);
	}
}


//*************************************************************
//
//		Update Objects
//
//*************************************************************

void GameManager::UpdateObjects(float deltaTime)
{
	player->Update(deltaTime);
	std::cout << "(" << player->GetPosition().x << ", " << player->GetPosition().y << ", " << player->GetPosition().z << ")" << std::endl;
	for (std::vector<GameObject*>::iterator obj = objects.begin(); obj != objects.end(); obj++)
	{
		(*obj)->Update(deltaTime);
	}
}

//*************************************************************
//
//		RenderObject
//
//*************************************************************

void GameManager::RenderObjects(glm::mat4 view_matrix, glm::mat4 projection_matrix)
{
	glUseProgram(program);
	// Set up vertex data buffer
	/*
	glBindBuffer(GL_ARRAY_BUFFER, models.at(MODEL_CUBE)->vbo);
	for (std::vector<GameObject*>::iterator obj = objects.begin(); obj != objects.end(); obj++)
	{

		// Set attributes for shader
		// Should be consistent with how we created the buffers for the model
		GLint vertex_att = glGetAttribLocation(program, "vertex");
		glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(vertex_att);

		GLint normal_att = glGetAttribLocation(program, "normal");
		glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normal_att);

		GLint color_att = glGetAttribLocation(program, "color");
		glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(color_att);

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), (*obj)->GetPosition());
		glm::mat4 orientation = glm::rotate(glm::mat4(1.0f), (*obj)->GetAngle(), (*obj)->GetAxisOfRotation());
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), (*obj)->GetScale());

		// Set world transformation matrix for square in shader
		glm::mat4 transf = translation * orientation * scale;
		GLint world_mat = glGetUniformLocation(program, "world_mat");
		glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

		// Draw square mesh (draw only vertex buffer)
		glDrawArrays(GL_TRIANGLES, 0, models.at(MODEL_CUBE)->size);
	}
	*/
	RenderPlayer(view_matrix, projection_matrix);

}
void GameManager::RenderPlayer(glm::mat4 view_matrix, glm::mat4 projection_matrix)
{
	// Set up data buffers
	glBindBuffer(GL_ARRAY_BUFFER, models.at(MODEL_TORUS)->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models.at(MODEL_TORUS)->ebo);

	// Set attributes for shader
	// Should be consistent with how we created the buffers for the model
	GLint vertex_att = glGetAttribLocation(program, "vertex");
	glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(vertex_att);

	GLint normal_att = glGetAttribLocation(program, "normal");
	glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(normal_att);

	GLint color_att = glGetAttribLocation(program, "color");
	glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(color_att);



	glm::mat4 translation = glm::translate(glm::mat4(1.0f), player->GetPosition());
	glm::mat4 orientation = glm::rotate(glm::mat4(1.0f), player->GetAngle(), player->GetAxisOfRotation());
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), player->GetScale());

	// Set world transformation matrix for torus in shader
	glm::mat4 transf = translation * orientation * scale;
	GLuint world_mat = glGetUniformLocation(program, "world_mat");
	glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

	// Draw torus mesh using both vertex and element buffers
	glDrawElements(GL_TRIANGLES, models.at(MODEL_TORUS)->size, GL_UNSIGNED_INT, 0);
}


//*************************************************************
//
//		MODEL CREATION
//
//*************************************************************
Model* GameManager::CreateCube(void)
{
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
			 0.5,  0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 0.0, 0.0
	};

	// Create model 
	Model *model;
	model = new Model;
	model->size = sizeof(vertex) / (sizeof(GLfloat) * 9);

	// Create OpenGL vertex buffer for model
	glGenBuffers(1, &model->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	return model;
}

Model* GameManager::CreateTorus(float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples) 
{
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

