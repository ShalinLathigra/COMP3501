#include "GameObject.h"



GameObject::GameObject(int index, int iD, glm::vec3 pos, glm::vec3 axi, glm::vec3 sca)
{
	model_index = index;
	id = iD;

	rotSpeed = 0.0f;
	angle = 0.0f;

	position = pos;
	axisOfRotation = axi;
	scale = sca;
	acceleration = glm::vec3();
	velocity = glm::vec3();
}


GameObject::~GameObject()
{
}


void GameObject::Update(float deltaTime)
{
	//Position
	velocity += acceleration * deltaTime;
	position += velocity * deltaTime;

	angle += rotSpeed * deltaTime;
}
