#pragma once

#ifndef COMPLEXNODE_H_
#define COMPLEXNODE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"

namespace game {

	// Abstraction of an ComplexNode
	class ComplexNode : public SceneNode {

	public:
		// Create ComplexNode from given resources
		ComplexNode(const std::string name, const Resource *geometry, const Resource *material);

		// Destructor
		~ComplexNode();

		// Get/set attributes specific to ComplexNodes
		glm::quat GetAngM(void) const;
		void SetAngM(glm::quat angm);

		float GetOrbM(void) const;
		void SetOrbM(float orbm);
		glm::vec3 GetOrbAx(void) const;
		void SetOrbAx(glm::vec3 orba);
		glm::vec3 GetOrbJ(void) const;
		void SetOrbJ(glm::vec3 orbj);

		float GetTransS(void) const;
		void SetTransS(float transs);
		glm::vec3 GetTransD(void) const;
		void SetTransD(glm::vec3 transd);

		// Update geometry configuration
		void Update(float deltaTime);
		void Draw(glm::mat4 p, Camera *camera);
		void DrawChildren(Camera *camera);

	private:
		bool rot_;
		glm::quat angm_;	// Angular momentum of ComplexNode

		bool orb_;
		float orbm_;	// Orbital speed of ComplexNode
		glm::vec3 orbj_;	// Orbit joint of ComplexNodee
		glm::vec3 orbax_;	// Orbit joint of ComplexNode
		float orbam_;
		
		bool trans_;
		float transs_;		// Speed of cyclic translation
		glm::vec3 transd_;	// Direction of cyclic translation
		float switch_timer_;	//When to toggle betwen in and out
		float max_switch_timer_;
		float state_;		// Current state (in or out);

		glm::quat orbit_amount_;
		void Orbit(float amount);

		glm::mat4 CalculateMatrix(glm::mat4 p);
	}; // class ComplexNode

} // namespace game

#endif // ComplexNode_H_
