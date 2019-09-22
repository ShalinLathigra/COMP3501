#pragma once

#include "Includes.h"

typedef struct modelStruct {
	GLuint vbo; // OpenGL vertex buffer object
	GLuint ebo; // OpenGL element buffer object
	GLuint size; // Size of data to be drawn
} Model;

class Entity
{
public:
	Entity();
	Entity(glm::vec3 pos, glm::vec3 scale);
	~Entity();

	//Place to put Torus/Cube functions
	static Model* CreateTorus(float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30);
	static Model* CreateCube(void);

	//Update function
	void Update(float deltaTime);

	//Setters
	inline void SetPosition(glm::vec3 pos) { position = pos; }
	inline void SetDirection(glm::vec3 dir) { direction = dir; }
	inline void SetScale(glm::vec3 scal) { scale = scal; }
	inline void SetAxis(glm::vec3 axis) { axisOfRotation = axis; }
	inline void SetRotSpeed(float speed) { rotSpeed = speed; }
	inline void SetSpeed(float sp) { speed = sp; }
	inline void SetMass(float m) { mass = m; }
	
	//Getters
	inline glm::vec3 GetPosition() { return position; }
	inline glm::vec3 GetVelocity() { return speed * direction; }
	inline glm::vec3 GetScale() { return scale; }
	inline glm::quat GetOrientation() { return orientation; }

	//Function to generate the transformation matrix for cubes
	glm::mat4 Entity::GenerateMatrix();

	//used by player now, might use it for cubes If I decide to
	void Fall();

	//Used by everything all the time.
	void Move(glm::vec3 dir, float deltaTime);
	inline bool CanMove() { return canMove; }

	//Used by cubes all the time
	inline void SetLifeSpan(float time) { live = true; lifeSpan = time; }
	bool IsAlive();


private:
	float fallSpeed;
	float speed;
	bool canMove;
	float maxBound;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 scale;
	glm::quat orientation; // Orientation of tester
	glm::vec3 axisOfRotation;
	float rotSpeed;
	float mass;
	bool live;
	float lifeSpan;
};

