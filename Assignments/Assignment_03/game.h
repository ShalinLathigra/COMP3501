#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#include <map>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "complex_node.h"
#include "laser_node.h"
#include "player_node.h"

namespace game {

    // Exception type for the game
    class GameException: public std::exception
    {
        private:
            std::string message_;
        public:
            GameException(std::string message) : message_(message) {};
            virtual const char* what() const throw() { return message_.c_str(); };
            virtual ~GameException() throw() {};
    };

    // Game application
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();
            // Call Init() before calling any other method
            void Init(void); 
            // Set up resources for the game
            void SetupResources(void);
            // Set up initial scene
            void SetupScene(void);
            // Run the game: keep the application active
            void MainLoop(void); 

        private:
            // GLFW window
            GLFWwindow* window_;

            // Scene graph containing all nodes to render
            SceneGraph scene_;

            // Resources available to the game
            ResourceManager resman_;

            // Camera abstraction
            Camera camera_;

			// Player Node Shit
			PlayerNode* player_;

			// Map to handle Keyboard Input
			std::map<int, bool> key_map_;

			// deltaTime
			float deltaTime;
            // Flag to turn animation on/off
            bool animating_;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);
 
            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);

			void ProcessKeyInput(void);

            // Asteroid field
            // Create instance of one asteroid
            ComplexNode *CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name);
            // Create entire random asteroid field
            void CreateAsteroidField(int num_asteroids = 1500);
			//Create and Add Player
			PlayerNode *CreatePlayerInstance();
			//Create and add Ground
			SceneNode *CreateGroundInstance(std::string entity_name, std::string object_name, std::string material_name);
			//Create Cannon
			SceneNode *CreateCannonInstance(std::string entity_name, std::string object_name, std::string material_name);


    }; // class Game

} // namespace game

#endif // GAME_H_
