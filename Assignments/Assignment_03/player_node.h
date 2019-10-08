#pragma once
#include "scene_node.h"
#include "asteroid.h"


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
		void Update(float deltaTime);

		void Draw(glm::mat4 p);

		// Perform relative transformations of camera
		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);

		//Toggle First/Third Person
		void ToggleView(void);

		//Manipulate Acceleration. 
		void SetAcceleration(glm::vec3 acc);
		void SetVelocity(glm::vec3 vel);

		glm::vec3 GetVelocity(void) const;
		glm::vec3 GetForward(void) const;
		glm::vec3 GetSide(void) const;
		glm::vec3 GetUp(void) const;

		float GetRotSpeed(void) const;

	private:
		Camera *camera_;

		//Physics Attributes
		glm::vec3 acc_;
		glm::vec3 vel_;

		float acc_speed_;
		float max_vel_;

		glm::vec3 forward_;	//initial side/forward To be used for ACC/Vel calcs & Camera View shit
		glm::vec3 side_;	//initial side

		//3rd Person Attributes		
		bool first_person_;
		float f_rot_speed_;
		float t_rot_speed_;
		float camera_y;
		float camera_z;

		//Functions to set Camera Attributes
		void SetCameraAttributes();
		//Functions to Update Transform Attributes

		//Functions to Update 3rd Person Attributes


	};
}
