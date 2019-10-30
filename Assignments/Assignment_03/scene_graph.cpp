#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

namespace game {

	SceneGraph::SceneGraph(void) {

		background_color_ = glm::vec3(0.0, 0.0, 0.0);
	}


	SceneGraph::~SceneGraph() {
	}


	void SceneGraph::SetBackgroundColor(glm::vec3 color) {

		background_color_ = color;
	}


	glm::vec3 SceneGraph::GetBackgroundColor(void) const {

		return background_color_;
	}


	SceneNode *SceneGraph::CreateNode(std::string node_name, Resource *geometry, Resource *material) {

		// Create scene node with the specified resources
		SceneNode *scn = new SceneNode(node_name, geometry, material);

		// Add node to the scene
		//node_.push_back(scn);
		root_->AddChild(scn);

		return scn;
	}


	void SceneGraph::AddNode(SceneNode *node) {

		//node_.push_back(node);
		root_->AddChild(node);
	}
	void SceneGraph::AddRoot(RootNode *node)
	{
		root_ = node;
	}



	SceneNode *SceneGraph::GetNode(std::string node_name) const {

		std::vector<SceneNode *> nodes = root_->GetChildren();
		// Find node with the specified name
		for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i]->GetName() == node_name) {
				return nodes[i];
			}
		}
		return NULL;

	}


	std::vector<SceneNode *>::const_iterator SceneGraph::begin() const {
		return root_->GetChildren().begin();
	}


	std::vector<SceneNode *>::const_iterator SceneGraph::end() const {

		return root_->GetChildren().end();
	}


	void SceneGraph::Draw(Camera *camera) {

		// Clear background
		glClearColor(background_color_[0],
			background_color_[1],
			background_color_[2], 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		root_->Draw(camera);
	}


	void SceneGraph::Update(float deltaTime, bool checkCollisions, glm::vec3 origin, glm::vec3 dir) 
	{
		root_->Update(deltaTime);

		if (checkCollisions)
			CalculateRayCollisions(origin, dir);
	}

	void SceneGraph::CalculateRayCollisions(glm::vec3 ray_origin, glm::vec3 ray_dir)
	{
		std::vector<std::string> names = std::vector<std::string>();
		std::vector<SceneNode *> nodes = root_->GetChildren();
		for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i]->GetName().find("Asteroid") != std::string::npos)
			{
				glm::vec3 node_dir = ray_origin - nodes[i]->GetPosition();

				if (CheckRayCollision(ray_dir, node_dir, nodes[i]->GetScale().x))
				{
					names.push_back(nodes[i]->GetName());
				}

			}
		}

		for (std::vector<std::string>::iterator iter = names.begin(); iter != names.end(); iter++)
		{
			root_->DeleteChild((*iter));
		}
	}
	bool SceneGraph::CheckRayCollision(glm::vec3 ray_dir, glm::vec3 node_dir, float rad)
	{

		double det, b;
		b = glm::dot(node_dir, ray_dir);
		det = b * b - glm::dot(node_dir, node_dir) + std::pow(rad, 2);
		if (det < 0)
		{
			//std::cout << "No HIT A!" << std::endl;
			return false;
		}
		else
		{
			double ans = b - det;
			if (ans < 0)
			{
				//std::cout << "No HIT B!" << std::endl;
				return false;
			}
			else
			{
				std::cout << "HIT!!!" << std::endl;
				return true;
			}
		}
	}

} // namespace game
