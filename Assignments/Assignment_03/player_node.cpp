#include "player_node.h"

namespace game
{
	PlayerNode::PlayerNode(const std::string name, const Resource *geometry, const Resource *material, Camera *camera) : SceneNode(name, geometry, material)
	{
		camera_ = camera;
	}
	PlayerNode::~PlayerNode()
	{

	}
}