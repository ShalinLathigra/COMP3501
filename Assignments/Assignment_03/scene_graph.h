#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "root_node.h"
#include "player_node.h"
#include "resource.h"
#include "camera.h"

namespace game {

    // Class that manages all the objects in a scene
    class SceneGraph {

        private:
            // Background color
            glm::vec3 background_color_;

            // Scene nodes to render
            //std::vector<SceneNode *> node_;
			RootNode *root_;

        public:
            // Constructor and destructor
            SceneGraph(void);
            ~SceneGraph();

            // Background color
            void SetBackgroundColor(glm::vec3 color);
            glm::vec3 GetBackgroundColor(void) const;
            
            // Create a scene node from the specified resources
            SceneNode *CreateNode(std::string node_name, Resource *geometry, Resource *material);
            // Add an already-created node
			void AddNode(SceneNode *node);
			void AddRoot(RootNode *node);
            // Find a scene node with a specific name
            SceneNode *GetNode(std::string node_name) const;
            // Get node const iterator
            std::vector<SceneNode *>::const_iterator begin() const;
            std::vector<SceneNode *>::const_iterator end() const;

            // Draw the entire scene
            void Draw(Camera *camera);

			// Update entire scene
			void Update(float deltaTime, bool checkCollisions, glm::vec3 origin, glm::vec3 dir);


			// Update entire scene
			void CalculateRayCollisions(glm::vec3 ray_origin, glm::vec3 ray_dir);
			bool CheckRayCollision(glm::vec3 ray_dir, glm::vec3 node_dir, float rad);

    }; // class SceneGraph

} // namespace game

#endif // SCENE_GRAPH_H_
