#pragma once
#include "scene_node.h"

namespace game
{
	class OrbitNode : public SceneNode
	{
	public:
		OrbitNode(const std::string name, const Resource *geometry, const Resource *material,glm::vec3 joint,glm::vec3 pos );
		~OrbitNode();


		glm::quat GetAngM(void) const;
		void SetAngM(glm::quat angm);
	};


}