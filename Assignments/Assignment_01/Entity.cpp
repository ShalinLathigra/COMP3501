#include "Entity.h"


//Constructors

//Loosely bounded objects
Entity::Entity()
{
	maxBound = 10.0f;
	position = glm::vec3();
	direction = glm::vec3();
	scale = glm::vec3(.25f, .25f, .25f);
	axisOfRotation = glm::vec3(0.0f, 1.0f, 0.0f);
	rotSpeed = 0.0f;
	fallSpeed = 10.0f;
	speed = 0.0f;
	canMove = true;
}
//tightly bounded objects
Entity::Entity(glm::vec3 pos, glm::vec3 sin)
{
	canMove = true;
	maxBound = 1.0f;
	position = pos;
	direction = glm::vec3();
	scale = sin;
	fallSpeed = 10.0f;
	speed = .5f;
	rotSpeed = 0.0f;
	axisOfRotation = glm::vec3(0.0f, 1.0f, 0.0f);
}


Entity::~Entity()
{
}

//Restrict x-y movement if "Falling"
void Entity::Fall()
{
	direction.z = -fallSpeed;
	canMove = false;
}

//Update position based off vel & passed time
void Entity::Move(glm::vec3 vel, float deltaTime)
{
	if (canMove)
	{
		//Bound  x/y
		if (position.x > maxBound && vel.x > 0)
		{
			vel.x = 0;
		}
		else if (position.x < -maxBound && vel.x < 0)
		{
			vel.x = 0;
		}

		if (position.y > (maxBound * .8) && vel.y > 0)
		{
			vel.y = 0;
		}
		else if (position.y < -(maxBound * .8) && vel.y < 0)
		{
			vel.y = 0;
		}
	}
	else 
	{
		//Bound Z
		vel = glm::vec3(0.0f, 0.0f, vel.z);
		if (position.z < -8.0f)
		{
			direction.z *= -1;
			vel.z *= -1;
			position = glm::vec3(position.x, position.y, -8.0f);
		}
		else if (position.z >= 0)
		{
			position = glm::vec3(position.x, position.y, 0);
			direction.z = 0;
			canMove = true;
		}
	}
	position += vel * deltaTime;
}

bool Entity::IsAlive()
{
	return live;
}


//Update based off passed time from last frame
void Entity::Update(float deltaTime)
{
	Move(speed * direction, deltaTime);
	glm::quat rotation = glm::angleAxis(rotSpeed * deltaTime * glm::pi<float>() / 180.0f, axisOfRotation);
	orientation *= rotation;

	if (live)
	{
		lifeSpan -= deltaTime;
		if (lifeSpan < 0)
		{
			live = false;
		}
	}
}


// Create the geometry for a torus
Model* Entity::CreateTorus(float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples) {

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


// Create the geometry for a torus
Model* Entity::CreateCube(void) {
	// The construction does *not* use shared vertices
	// 9 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3)
	// Define Faces: 
	GLfloat vertex[] = {
		// First triangle
		 0.5,  0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,
		 0.5, -0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,
		-0.5, -0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,
		// Second triangle
		-0.5, -0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,
		-0.5,  0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,
		 0.5,  0.5,  -0.5,    0.0,  0.0,  -1.0,    1.0, 0.0, 0.0,

		 // 3rd triangle
		 0.5,   0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 0.5, 0.0,
		 0.5,   0.5,   0.5,    0.0,  0.0,  1.0,    1.0, 0.5, 0.0,
		 0.5,  -0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 0.5, 0.0,
		 // 4th triangle
		 0.5,   -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 0.5, 0.0,
		 0.5,  -0.5,  -0.5,    0.0,  0.0,  1.0,    1.0, 0.5, 0.0,
		 0.5,   0.5,   0.5,    0.0,  0.0,  1.0,    1.0, 0.5, 0.0,

		 // 5th triangle
		 -0.5,   0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
		 -0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
		  0.5,   0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
		  // 6th triangle
		   0.5,   0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
		  -0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,
		   0.5,  -0.5,  0.5,    0.0,  0.0,  1.0,    1.0, 1.0, 0.0,


		   // 7 triangle
		   -0.5,  0.5,  -0.5,    0.0,  0.0,  -1.0,    0.0, 1.0, 0.0,
		   -0.5, -0.5,  -0.5,    0.0,  0.0,  -1.0,    0.0, 1.0, 0.0,
		   -0.5,  0.5,   0.5,    0.0,  0.0,  -1.0,    0.0, 1.0, 0.0,
		   // 8 triangle					 -
		   -0.5,   0.5,  0.5,    0.0,  0.0,  -1.0,    0.0, 1.0, 0.0,
		   -0.5,  -0.5, -0.5,    0.0,  0.0,  -1.0,    0.0, 1.0, 0.0,
		   -0.5,  -0.5,  0.5,    0.0,  0.0,  -1.0,    0.0, 1.0, 0.0,

		   // 9 triangle
			0.5,  -0.5,  -0.5,    0.0,  -1.0,  0.0,    0.0, 1.0, 0.5,
		   -0.5,  -0.5,   0.5,    0.0,  -1.0,  0.0,    0.0, 1.0, 0.5,
		   -0.5,  -0.5,  -0.5,    0.0,  -1.0,  0.0,    0.0, 1.0, 0.5,
		   // 10 triangle				-1	   0
			0.5,  -0.5, -0.5,     0.0,  -1.0,  0.0,    0.0, 1.0, 0.5,
			0.5,  -0.5,  0.5,     0.0,  -1.0,  0.0,    0.0, 1.0, 0.5,
		   -0.5,  -0.5,  0.5,     0.0,  -1.0,  0.0,    0.0, 1.0, 0.5,

		   // 11 triangle
			0.5,  0.5, -0.5,      0.0,  1.0,  0.0,    0.0, 0.0, 1.0,
		   -0.5,  0.5,  0.5,      0.0,  1.0,  0.0,    0.0, 0.0, 1.0,
			0.5,  0.5,  0.5,      0.0,  1.0,  0.0,    0.0, 0.0, 1.0,
			// 12 triangle				1	  0
			-0.5,  0.5,  -0.5,    0.0,  1.0,  0.0,    0.0, 0.0, 1.0,
			-0.5,  0.5,   0.5,    0.0,  1.0,  0.0,    0.0, 0.0, 1.0,
			 0.5,  0.5,  -0.5,    0.0,  1.0,  0.0,    0.0, 0.0, 1.0,
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


glm::mat4 Entity::GenerateMatrix()
{
	return (glm::translate(glm::mat4(1), position) * glm::mat4_cast(orientation) * glm::scale(glm::mat4(1.0), scale));
}