#pragma once
#include "scene_node.h"
#include "player_node.h"

namespace game
{
	class RootNode : public SceneNode
	{
	public:
		RootNode(const std::string name, const Resource *geometry, const Resource *material);
		~RootNode();

		void Update(float deltaTime);
		void Draw(Camera *camera);

		void DeleteChild(std::string name);
	};


}