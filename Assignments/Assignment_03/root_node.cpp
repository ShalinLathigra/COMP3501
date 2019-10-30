#include "root_node.h"
#include <iostream>
namespace game
{
	RootNode::RootNode(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material) {
	}


	RootNode::~RootNode()
	{
	}
	void RootNode::Update(float deltaTime) 
	{
		for (int i = 0; i < children_.size(); i++) {
			if (children_[i]->GetName() == "Player")
			{
				((PlayerNode *)children_[i])->PlayerNode::Update(deltaTime);
			}
			else
			{
				children_[i]->Update(deltaTime);
			}
		}
	}

	void RootNode::Draw(Camera *camera)
	{
		SceneNode::Draw(glm::mat4(1.0), camera);
		// Draw all scene nodes
		for (int i = 0; i < children_.size(); i++) {
			if (children_[i]->GetName() == "Player")
			{
				((PlayerNode *)children_[i])->PlayerNode::Draw(glm::mat4(1.0));
				((PlayerNode *)children_[i])->PlayerNode::DrawChildren();
			}
			else
			{
				children_[i]->Draw(glm::mat4(1.0), camera);
				children_[i]->DrawChildren(camera);
			}
		}
	}

	void RootNode::DeleteChild(std::string name)
	{
		int i = 0;
		bool found = false;
		for (std::vector<SceneNode *>::iterator iter = children_.begin(); iter != children_.end(); iter++, i++)
		{
			if ((*iter)->GetName().compare(name) == 0)
			{
				//i = index of deleted element
				found = true;
				break;
			}
		}
		if (found)
		{
			children_.erase(children_.begin() + i);
		}
	}
}