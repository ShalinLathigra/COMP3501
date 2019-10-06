#include "player_node.h"
#include <iostream>
#define GLM_FORCE_RADIANS

namespace game
{
	PlayerNode::PlayerNode(const std::string name, const Resource *geometry, const Resource *material, Camera *camera) : SceneNode(name, geometry, material)
	{
		camera_ = camera;
		SetPosition(camera_->GetPosition());
		SetOrientation(camera_->GetOrientation());
		first_person_ = true;

		forward_ = camera_->GetForward();
		side_ = camera_->GetSide();

		acc_speed_ = 10.0f;
		rot_speed_ = glm::pi<float>() / 180.0f;
	}
	PlayerNode::~PlayerNode()
	{

	}

	void PlayerNode::Draw(glm::mat4 p)
	{
		if (!first_person_)
		{
			//Do Nothing
		}
		else
		{
			SceneNode::Draw(p, camera_);
		}
	}

	void PlayerNode::Update(float deltaTime)
	{
		vel_ += acc_ * acc_speed_ * deltaTime;

		Translate(vel_ * deltaTime);
		Rotate(ang_ * deltaTime);


		if (first_person_)
		{
			//std::cout << "HERE!" << std::endl;
			camera_->SetPosition(GetPosition());
			camera_->SetOrientation(GetOrientation());
		}
		else
		{
			// Do Third Person Stuff Here
		}
	}


	glm::vec3 PlayerNode::GetForward(void) const
	{
		glm::vec3 current_forward = GetOrientation() * forward_;
		return -current_forward; // Return -forward since the camera coordinate system points in the opposite direction

	}
	glm::vec3 PlayerNode::GetSide(void) const
	{
		glm::vec3 current_side = GetOrientation() * side_;
		return current_side;
	}
	glm::vec3 PlayerNode::GetUp(void) const
	{
		glm::quat orientation = GetOrientation();
		glm::vec3 current_forward = orientation * forward_;
		glm::vec3 current_side = orientation * side_;
		glm::vec3 current_up = glm::cross(current_forward, current_side);
		current_up = glm::normalize(current_up);
		return current_up;
	}


	void PlayerNode::SetAcceleration(glm::vec3 acc)
	{
		acc_ = acc;
	}

	void PlayerNode::SetVelocity(glm::vec3 vel)
	{
		vel_ = vel;
	}
	void PlayerNode::SetAngular(glm::quat ang)
	{
		ang_ = ang;
	}

	float PlayerNode::GetRotSpeed(void) const
	{
		return rot_speed_;
	}
}