#pragma once
#include "scene_node.h"


/*
	What does this class do>

	The PlayerNode is a companion to the camera

	In first person, the Camera will use normal PlayerNode information

	In 3rd Person, it will use the values calculated other info in PlayerNode
		i.e. angle/axis + distance from player to calculate orbit.
		Shit to make it look directly at player
		etc.

	The Pitch/Yaw & shit will refer to PlayerNode instead, and the attributes will be calculated within this one.
*/
namespace game 
{
	class PlayerNode :
		public SceneNode
	{
	public:
		PlayerNode(const std::string name, const Resource *geometry, const Resource *material, Camera *camera);
		~PlayerNode();

		//Update from Physics
		//void Update(float deltaTime);


		// Perform relative transformations of camera
		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);


	private:
		Camera *camera_;

		//Physics Attributes
		glm::vec3 acc_;
		glm::vec3 vel_;

		glm::vec3 forward_;	//initial side/forward To be used for ACC/Vel calcs & Camera View shit
		glm::vec3 side_;	//initial side

		//3rd Person Attributes		


		//Functions to set Camera Attributes
		void SetCameraAttributes();
		//Functions to Update Transform Attributes

		//Functions to Update 3rd Person Attributes


	};
}
