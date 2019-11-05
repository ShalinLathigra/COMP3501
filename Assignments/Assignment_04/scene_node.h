#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "camera.h"

namespace game {

    // Class that manages one object in a scene 
    class SceneNode {

        public:
            // Create scene node from given resources
            SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

            // Destructor
            ~SceneNode();
            
            // Get name of node
            const std::string GetName(void) const;

            // Get node attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;
            glm::vec3 GetScale(void) const;
			glm::mat4 ComposeMatrix(void) ;

            // Set node attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
			void SetScale(glm::vec3 scale);
			void SetJoint(glm::vec3 joint);
			void SetOrbit(glm::quat orbit);
			void SetP(glm::mat4 p);
            
            // Perform transformations on node
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);
            void Scale(glm::vec3 scale);

            // Draw the node according to scene parameters in 'camera'
            // variable
            virtual void Draw(Camera *camera);

            // Update the node
            virtual void Update(void);

            // OpenGL variables
            GLenum GetMode(void) const;
            GLuint GetArrayBuffer(void) const;
            GLuint GetElementArrayBuffer(void) const;
            GLsizei GetSize(void) const;
			GLuint GetMaterial(void) const;


        private:
            std::string name_; // Name of the scene node
            GLuint array_buffer_; // References to geometry: vertex and array buffers
            GLuint element_array_buffer_;
            GLenum mode_; // Type of geometry
            GLsizei size_; // Number of primitives in geometry
            GLuint material_; // Reference to shader program
            GLuint texture_; // Reference to texture resource
            glm::vec3 position_; // Position of node
			glm::quat orientation_; // Orientation of node
			glm::vec3 scale_; // Scale of node
			glm::quat orbit_; // Orbit of node
			glm::vec3 joint_; // Joint of node
			glm::mat4 transf_; // Composed Transformations


			glm::mat4 p_; // Parent Transform

            // Set matrices that transform the node in a shader program
            void SetupShader(GLuint program);

    }; // class SceneNode

} // namespace game

#endif // SCENE_NODE_H_
