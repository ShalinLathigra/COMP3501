#pragma once
#include "scene_node.h"
#include <vector>
namespace game
{
	class ParticleNode :
		public SceneNode
	{
	public:
		ParticleNode(const std::string name, const Resource *geometry, const  Resource *material);
		~ParticleNode();

		std::vector<glm::vec4> colours;
		std::vector<glm::vec3> positions;
		int instances;
	};
}