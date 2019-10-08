#include "player_node.h"
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace game
{
	PlayerNode::PlayerNode(const std::string name, const Resource *geometry, const Resource *material, Camera *camera) : SceneNode(name, geometry, material)
	{
		camera_ = camera;
		SetPosition(camera_->GetPosition());
		SetOrientation(camera_->GetOrientation());
		first_person_ = false;

		forward_ = camera_->GetForward();
		side_ = camera_->GetSide();

		acc_speed_ = 37.5f;
		float rot_speed = 8.0f;
		f_rot_speed_ = glm::pi<float>() / (2 * rot_speed);
		t_rot_speed_ = glm::pi<float>() / (3 * rot_speed);
		acc_ = glm::vec3();
		vel_ = glm::vec3();

		max_vel_ = 75.0f;

		camera_y_ = 4.0f;
		camera_z_ = 30.0f;
		//camera_y_ = 0.0f;
		//camera_z_ = 50.0f;

		firing_ = false;
		max_laser_timer_ = .1f;
		laser_timer_ = 0.0f;
	}
	PlayerNode::~PlayerNode()
	{

	}

	void PlayerNode::Draw(glm::mat4 p)
	{
		if (first_person_)
		{
			//Do Nothing
		}
		else
		{
			SceneNode::Draw(p, camera_);
		}

		if (firing_)
		{
			std::cout << "Draw Laser!" << std::endl;
			laser_->Draw(p * matrix_ * laser_mat_, camera_);
		}
	}

	void PlayerNode::Update(float deltaTime)
	{
		// Increment Timer
		if (firing_)
		{
			laser_timer_ = glm::max(laser_timer_ - deltaTime, 0.0f);
			if (laser_timer_ == 0)
			{
				firing_ = false;
			}
		}

		// Physics
		vel_ += acc_ * acc_speed_ * deltaTime;

		if (glm::length(vel_) > max_vel_)
		{
			vel_ = glm::normalize(vel_) * max_vel_;
		}

		std::cout << glm::length(vel_) << std::endl;

		Translate(vel_ * deltaTime);

		// Update Children
		for (std::vector<SceneNode*>::iterator iter = children_.begin(); iter != children_.end(); iter++)
		{
			((Asteroid *)(*iter))->Asteroid::Update(glm::length(vel_) * deltaTime);
		}

		// Update Camera
		SetCameraAttributes();

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
	glm::vec3 PlayerNode::GetVelocity(void) const
	{
		return vel_;
	}

	float PlayerNode::GetRotSpeed(void) const
	{
		return f_rot_speed_;
	}


	void PlayerNode::Pitch(float angle)
	{
		glm::quat pitch;
		if (first_person_)
		{
			pitch = glm::angleAxis(angle * f_rot_speed_, GetSide());
		}
		else
		{
			pitch = glm::angleAxis(angle * t_rot_speed_, GetSide());
		}
		SetOrientation(glm::normalize(pitch * GetOrientation()));
	}
	void PlayerNode::Yaw(float angle)
	{
		glm::quat yaw;
		if (first_person_)
		{
			yaw = glm::angleAxis(angle * f_rot_speed_, GetUp());
		}
		else
		{
			yaw= glm::angleAxis(angle * t_rot_speed_, GetUp());
		}
		SetOrientation(glm::normalize(yaw * GetOrientation()));
	}
	void PlayerNode::Roll(float angle)
	{
		glm::quat roll;
		if (first_person_)
		{
			roll = glm::angleAxis(angle * f_rot_speed_, GetForward());
		}
		else
		{
			roll = glm::angleAxis(angle * t_rot_speed_, GetForward());
		}
		SetOrientation(glm::normalize(roll * GetOrientation()));
	}


	void PlayerNode::SetCameraAttributes()
	{
		if (first_person_)
		{
			camera_->SetPosition(GetPosition());
			camera_->SetOrbit(glm::mat4(1.0));
			camera_->SetOrientation(GetOrientation());
		}
		else
		{
			camera_->SetPosition(GetPosition() + GetForward() * camera_z_ - GetUp()* camera_y_);
			camera_->SetOrientation(GetOrientation());
		}
	}

	void PlayerNode::ToggleView(void)
	{
		first_person_ = !first_person_;
	}


	void PlayerNode::SetLaser(SceneNode* laser)
	{
		laser_ = laser;
	}
	void PlayerNode::SetFiring(bool firing)
	{
		firing_ = firing;
		laser_timer_ = max_laser_timer_;
		laser_mat_ = glm::mat4_cast(glm::angleAxis(- glm::pi<float>() / (2.0f), glm::vec3(1.0f, 0.0f, 0.0f))) * glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 5.0f, 0.0f)) * glm::mat4_cast(glm::angleAxis(glm::pi<float>()/2.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

}