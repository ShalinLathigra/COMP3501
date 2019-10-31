#include "complex_node.h"
#include <glm/gtc/matrix_transform.hpp>

namespace game {

	ComplexNode::ComplexNode(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material) {
		state_ = 1.0f;
		max_switch_timer_ = 1.0f;
	}


	ComplexNode::~ComplexNode() {
	}


	glm::quat ComplexNode::GetAngM(void) const {

		return angm_;
	}


	void ComplexNode::SetAngM(glm::quat angm) {

		angm_ = angm;
		rot_ = true;
	}

	float ComplexNode::GetOrbM(void) const
	{
		return orbm_;
	}
	void ComplexNode::SetOrbM(float orbm)
	{
		orbm_ = orbm;
		orbam_ = 0.0f;
		orb_ = true;
	}
	glm::vec3 ComplexNode::GetOrbAx(void) const
	{
		return orbax_;
	}
	void ComplexNode::SetOrbAx(glm::vec3 orba)
	{
		orbax_ = orba;
	}
	glm::vec3 ComplexNode::GetOrbJ(void) const
	{
		return orbj_;
	}
	void ComplexNode::SetOrbJ(glm::vec3 orbj)
	{
		orbj_ = orbj;
	}

	float ComplexNode::GetTransS(void) const
	{
		return transs_;
	}
	void ComplexNode::SetTransS(float transs)
	{
		transs_ = transs;
	}
	glm::vec3 ComplexNode::GetTransD(void) const
	{
		return transd_;
	}
	void ComplexNode::SetTransD(glm::vec3 transd)
	{
		transd_ = transd;
		orb_ = true;
	}


	void ComplexNode::Update(float deltaTime) 
	{
		if (rot_)
			Rotate(angm_ * deltaTime);
		if (orb_)
		{
			orbam_ += deltaTime;
			Orbit(sin(orbam_) / 8.0f* glm::pi<float>() + glm::pi<float>() / 2.0f);
		}
		if (trans_)
		{
			switch_timer_ = glm::max(switch_timer_ - deltaTime, 0.0f);
			if (switch_timer_ == 0.0f)
			{
				state_ *= -1.0f;
				switch_timer_ = max_switch_timer_;
			}

			Translate(state_ * transd_ * transs_ * deltaTime);
		}

		for (std::vector<SceneNode*>::iterator iter = children_.begin(); iter != children_.end(); iter++)
		{
			((ComplexNode*)(*iter))->Update(deltaTime);
		}
	}

	void ComplexNode::Orbit(float orbam)
	{
		orbit_amount_ = glm::angleAxis(orbam, orbax_);
	}

	void ComplexNode::Draw(glm::mat4 p, Camera *camera)
	{
		// Select proper material (shader program)
		glUseProgram(material_);

		// Set geometry to draw
		glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

		// Set globals for camera
		camera->SetupShader(ComplexNode::CalculateMatrix(p), material_);

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
	void ComplexNode::DrawChildren(Camera *camera)
	{

		for (std::vector<SceneNode*>::iterator iter = children_.begin(); iter != children_.end(); iter++)
		{
			((ComplexNode*)(*iter))->Draw(matrix_, camera);
			((ComplexNode*)(*iter))->DrawChildren(camera);
		}
	}

	glm::mat4 ComplexNode::CalculateMatrix(glm::mat4 p)
	{
		glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale_);
		glm::mat4 rotation = glm::mat4_cast(orientation_);
		glm::mat4 orbit = glm::mat4_cast(orbit_amount_) * glm::translate(glm::mat4(1.0), -orbj_);
		glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);

		matrix_ = p * translation * orbit * rotation;

		return matrix_ * scaling;
	}

} // namespace game
