#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


namespace game {

    // Abstraction of a camera
    class Camera {

        public:
            Camera(void);
            ~Camera();
 
            // Get global camera attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;

            // Set global camera attributes
            void SetPosition(glm::vec3 position);
			void SetOrientation(glm::quat orientation);
			void SetOrbit(glm::mat4 orbit);
            
            // Get relative attributes of camera
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;
			
            // Set the view from camera parameters: initial position of camera,
            // point looking at, and up vector
            // Resets the current orientation and position of the camera
            void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);
            // Set projection from frustum parameters: field-of-view,
            // near and far planes, and width and height of viewport
            void SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h);
            // Set all camera-related variables in shader program
            void SetupShader(glm::mat4 world_matrix, GLuint program);


			// Create view matrix from current camera parameters
			//void SetupViewMatrix(glm::vec3 forward, glm::vec3 side_, glm::mat4 position, glm::quat orientation);

        private:
            glm::vec3 position_; // Position of camera		 
            glm::quat orientation_; // Orientation of camera 
			glm::mat4 orbit_;	//Orbit of camera. For Third Person primarily
            glm::vec3 forward_; // Initial forward vector	 
            glm::vec3 side_; // Initial side vector			 
            glm::mat4 view_matrix_; // View matrix
            glm::mat4 projection_matrix_; // Projection matrix

            // Create view matrix from current camera parameters
            void SetupViewMatrix(void);	// To be removed

    }; // class Camera

} // namespace game

#endif // CAMERA_H_
