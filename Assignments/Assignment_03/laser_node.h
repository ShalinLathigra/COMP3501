#pragma once
#include "scene_node.h"
#include <algorithm>    // std::max

namespace game
{
	class LaserNode :
		public SceneNode
	{
	public:
		LaserNode(const std::string name, const Resource *geometry, const Resource *material);
		~LaserNode();

		void Draw(glm::mat4 p, Camera *camera);
		void Update(float deltaTime);
		void AdvanceTimers(float deltaTime);

		void FireLaser();
		void StartCooldown();

		//glm::vec3 GetForward(void) const;
		void SetJoint(glm::vec3);

		glm::mat4 CalculateMatrix(glm::mat4 p);
	private:
		bool active_;

		glm::vec3 forward_;	//initial up

		glm::vec3 joint_;
		glm::quat orbit_amount_;

		float max_duration_timer_;
		float max_cd_timer_;

		float duration_timer_;
		float cd_timer_;
	};	
}
