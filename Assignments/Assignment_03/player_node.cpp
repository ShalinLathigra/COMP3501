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
		float seconds_per_rot = 4.0f;
		f_rot_speed_ = glm::pi<float>() / seconds_per_rot;
		t_rot_speed_ = glm::pi<float>() / seconds_per_rot;
		acc_ = glm::vec3();
		vel_ = glm::vec3();

		max_vel_ = 100.0f;

		camera_y_ = 4.0f;
		camera_z_ = 30.0f;

		pitch_dir_ = 0.0f;
		yaw_dir_ = 0.0f;
		roll_dir_ = 0.0f;
	}
	PlayerNode::~PlayerNode()
	{

	}

	void PlayerNode::Draw(glm::mat4 p)
	{
		if (!first_person_)
		{
			// Select proper material (shader program)
			glUseProgram(material_);

			// Set geometry to draw
			glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

			// Set globals for camera
			camera_->SetupShader(CalculateMatrix(p), material_);

			// Set world matrix and other shader input variables
			SetupShader(material_);

			// Draw geometry
			if (mode_ == GL_POINTS) {
				glDrawArrays(mode_, 0, size_);
			}
			else {
				glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);
			}
		}
	}

	void PlayerNode::DrawChildren()
	{
		for (std::vector<SceneNode*>::iterator iter = children_.begin(); iter != children_.end(); iter++)
		{
			std::string current = (*iter)->GetName();
			if (current.find("Laser") != std::string::npos)
			{
				((LaserNode *)(*iter))->LaserNode::Draw(matrix_, camera_);
			}
			else
			{
				(*iter)->Draw(matrix_, camera_);
				(*iter)->DrawChildren(camera_);
			}
		}
	}

	void PlayerNode::Update(float deltaTime)
	{

		// Physics
		vel_ += acc_ * acc_speed_ * deltaTime;

		if (glm::length(vel_) > max_vel_)
		{
			vel_ = glm::normalize(vel_) * max_vel_;
		}

		Translate(vel_ * deltaTime);
		// Rotate about each axis
		float rot_speed = GetRotSpeed();
		Pitch(rot_speed * pitch_dir_ * deltaTime);
		Yaw(rot_speed * yaw_dir_ * deltaTime);
		Roll(rot_speed * roll_dir_ * deltaTime);

		// Update Children
		for (std::vector<SceneNode*>::iterator iter = children_.begin(); iter != children_.end(); iter++)
		{
			std::string current = (*iter)->GetName();
			if (current.find("Engine") != std::string::npos)
			{
				((Asteroid *)(*iter))->Asteroid::Update(glm::length(vel_) * deltaTime);
			}
			else if (current.find("Laser") != std::string::npos)
			{
				((LaserNode *)(*iter))->LaserNode::Update(glm::length(vel_) * deltaTime);
			}
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
		return (first_person_) ? f_rot_speed_ : t_rot_speed_;
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

	void PlayerNode::SetPitch(float dir)
	{
		pitch_dir_ = dir;
	}
	void PlayerNode::SetYaw(float dir)
	{
		yaw_dir_ = dir;
	}
	void PlayerNode::SetRoll(float dir)
	{
		roll_dir_ = dir;
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
}