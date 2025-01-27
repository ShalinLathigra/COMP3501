#include "laser_node.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

namespace game
{
	LaserNode::LaserNode(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material)
	{
		active_ = false;

		width_ = .5f;
		forward_ = glm::vec3(0.0f, 0.0f, 1.0f);	//initial forward

		max_duration_timer_ = .25f;
		max_cd_timer_ = .25f;

		duration_timer_ = 0.0f;
		cd_timer_ = 0.0f;

		joint_ = glm::vec3(0.0f);
		orbit_amount_ = glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	LaserNode::~LaserNode()
	{
	}


	void LaserNode::Draw(glm::mat4 p, Camera *camera)
	{
		// Select proper material (shader program)
		glUseProgram(material_);

		// Set geometry to draw
		glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

		// Set globals for camera
		camera->SetupShader(CalculateMatrix(p), material_);

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
	void LaserNode::Update(float deltaTime)
	{
		AdvanceTimers(deltaTime);
	}

	//Advance Cooldowns and Laser Duration
	void LaserNode::AdvanceTimers(float deltaTime)
	{
		if (duration_timer_ > 0.0f)
		{
			duration_timer_ = std::max(0.0f, duration_timer_ - deltaTime);
		}
		else if (cd_timer_ > 0.0f && duration_timer_ == 0.0f)
		{
			cd_timer_ = std::max(0.0f, cd_timer_ - deltaTime);
		}

		if (duration_timer_ == 0.0f)
			active_ = false;
	}
	void LaserNode::Fire(void)
	{
		if (cd_timer_ == 0.0f && active_ == false)
		{
			duration_timer_ = max_duration_timer_;
			cd_timer_ = max_cd_timer_;
			active_ = true;

			GetForward();
		}
	}
	bool LaserNode::IsActive(void)
	{
		return active_;
	}

	float LaserNode::GetWidth(void) const
	{
		return width_;
	}

	glm::vec3 LaserNode::GetForward(void) const
	{
		return forward_; // Return -forward since the camera coordinate system points in the opposite direction
	}
	
	void LaserNode::SetJoint(glm::vec3 joint) 
	{
		joint_ = joint;
	}

	glm::mat4 LaserNode::CalculateMatrix(glm::mat4 p)
	{
		glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale_);
		glm::mat4 rotation = glm::mat4_cast(orientation_);
		glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);

		glm::mat4 orbit_rotation = glm::mat4_cast(orbit_amount_);
		glm::mat4 orbit_translation = glm::translate(glm::mat4(1.0), -joint_);

		matrix_ = p * translation * (orbit_rotation * orbit_translation) * rotation;

		return matrix_ * scaling;
	}
}