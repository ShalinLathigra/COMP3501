#pragma once
#include "scene_node.h"
#include "complex_node.h"
#include "laser_node.h"


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
		void DrawChildren();

		// Perform relative transformations of camera
		void SetPitch(float dir);
		void SetYaw(float dir);
		void SetRoll(float dir);

		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);

		//Toggle First/Third Person
		void ToggleView(void);

		//Functions to interact with Laser Object
		void SetLaser(LaserNode *laser);
		void FireLaser(void);
		bool IsFiring(void);

		//Manipulate Acceleration. 
		void SetAcceleration(glm::vec3 acc);
		void SetVelocity(glm::vec3 vel);

		glm::vec3 GetVelocity(void) const;
		glm::vec3 GetForward(void) const;
		glm::vec3 GetSide(void) const;
		glm::vec3 GetUp(void) const;

		float GetRotSpeed(void) const;

		glm::vec3 GetLaserOrigin(void) const;
		glm::vec3 GetLaserDirection(void) const;
		float GetLaserWidth(void) const;

	private:
		Camera *camera_;

		//Physics Attributes
		glm::vec3 acc_;
		glm::vec3 vel_;

		float acc_speed_;
		float max_vel_;

		float pitch_dir_;
		float yaw_dir_;
		float roll_dir_;

		glm::vec3 forward_;	//initial side/forward To be used for ACC/Vel calcs & Camera View shit
		glm::vec3 side_;	//initial side

		//Laser Reference
		LaserNode *laser_;

		//3rd Person Attributes		
		bool first_person_;
		float f_rot_speed_;
		float t_rot_speed_;
		float camera_y_;
		float camera_z_;

		//Functions to set Camera Attributes
		void SetCameraAttributes();



	};
}
