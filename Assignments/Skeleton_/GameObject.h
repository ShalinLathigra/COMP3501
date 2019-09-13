#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class GameObject

{
public:
	GameObject(int index, int iD, glm::vec3 pos, glm::vec3 AxisOfRotation, glm::vec3 scale);
	~GameObject();

	void Update(float deltaTime);

	inline int GetModelIndex(void) { return model_index; }
	inline int GetID(void) { return id; }
	inline float GetRotSpeed(void) { return rotSpeed; }
	inline float GetAngle(void) { return angle; }
	inline glm::vec3 GetAcceleration(void) { return acceleration; }
	inline glm::vec3 GetVelocity(void) { return velocity; }
	inline glm::vec3 GetPosition(void) { return position; }
	inline glm::vec3 GetAxisOfRotation(void) { return axisOfRotation; }
	inline glm::vec3 GetScale(void) { return scale; }

	inline void SetModelIndex(int index) { model_index = index; }
	inline void SetRotSpeed(float rot) { rotSpeed = rot; }
	inline void SetAcceleration(glm::vec3 acc) { acceleration = acc; }
	inline void SetVelocity(glm::vec3 vel) { velocity = vel; }
	inline void SetPosition(glm::vec3 pos) { position = pos; }
	inline void SetAxisOfRotation(glm::vec3 axi) { axisOfRotation = axi; }
	inline void SetScale(glm::vec3 sca) { scale = sca; }

private:
	int model_index;
	int id;

	float rotSpeed;
	float angle;

	glm::vec3 acceleration;
	glm::vec3 velocity;
	glm::vec3 position;
	glm::vec3 axisOfRotation;
	glm::vec3 scale;
};
