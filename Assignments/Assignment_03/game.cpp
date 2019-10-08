#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 60.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 camera_position_g(0.0, 0.0, 800.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;


Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

    // Set variables
    animating_ = true;
}

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // Set up camera
    // Set current view
    camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Create a simple sphere to represent the asteroids
    resman_.CreateSphere("SimpleSphereMesh", 1.0, 10, 10);

	resman_.CreateTorus("SimpleTorusMesh", 1.0, .5, 4, 4);

	resman_.CreateCylinder("SimpleCylinderMesh", .5, 5);

    // Load material to be applied to asteroids
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
    resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);
	
	player_ = CreatePlayerInstance();
    // Create asteroid field
    CreateAsteroidField();
}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){
		
		ProcessKeyInput();

        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.05){
				deltaTime = current_time - last_time;
                scene_.Update(deltaTime);
                last_time = current_time;
            }
        }

        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

    // Quit game if 'q' is pressed
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    //// Stop animation if space bar is pressed
    //if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
    //    game->animating_ = (game->animating_ == true) ? false : true;
    //}

	switch (key)
	{
	case(GLFW_KEY_UP):	//All these can be held down
	case(GLFW_KEY_DOWN):
	case(GLFW_KEY_LEFT):
	case(GLFW_KEY_RIGHT):
	case(GLFW_KEY_S):
	case(GLFW_KEY_X):
	case(GLFW_KEY_A):
	case(GLFW_KEY_Z):
	case(GLFW_KEY_J):
	case(GLFW_KEY_L):
	case(GLFW_KEY_I):
	case(GLFW_KEY_K):
	case(GLFW_KEY_LEFT_SHIFT):
		if (action == GLFW_PRESS)
		{
			game->key_map_[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			game->key_map_[key] = false;
		}
		break;
	case(GLFW_KEY_LEFT_CONTROL):	//Only trigger on release
		if (action == GLFW_RELEASE)
			game->player_->ToggleView();
		break;
	case(GLFW_KEY_SPACE):	//Must press space to fire
		if (action == GLFW_PRESS)
		{
			game->key_map_[key] = true;
		}
		else
		{
			game->key_map_[key] = false;
		}
	default:
		break;
	}
}



void Game::ProcessKeyInput(void)
{

	// View control

	// compose transformation matrix
	if (animating_)
	{
	glm::vec3 acc_vec(0);

	float pitch = 0, yaw = 0, roll = 0;

	float rot_factor = glm::pi<float>() / 90.0f;

	if (key_map_[GLFW_KEY_UP]) {
		//camera_.Pitch(rot_factor);
		pitch = 1.0;
		// std::cout << "up" << std::endl;
	}
	if (key_map_[GLFW_KEY_DOWN]) {
		//camera_.Pitch(-rot_factor);
		pitch = -1.0;
		// std::cout << "down" << std::endl;
	}
	if (key_map_[GLFW_KEY_LEFT]) {
		//camera_.Yaw(rot_factor);
		yaw = -1;
		// std::cout << "left" << std::endl;
	}
	if (key_map_[GLFW_KEY_RIGHT]) {
		//camera_.Yaw(-rot_factor);
		yaw = 1;
		// std::cout << "right" << std::endl;
	}
	if (key_map_[GLFW_KEY_S]) {
		//camera_.Roll(-rot_factor);
		roll = 1.0;
		// std::cout << "s" << std::endl;
	}
	if (key_map_[GLFW_KEY_X]) {
		//camera_.Roll(rot_factor);
		roll = -1;
		// std::cout << "x" << std::endl;
	}

	if (key_map_[GLFW_KEY_A]) {
		//camera_.Translate(camera_.GetForward()*trans_factor);
		acc_vec += -player_->GetForward();
		// std::cout << "a" << std::endl;
	}
	if (key_map_[GLFW_KEY_Z]) {
		//camera_.Translate(-camera_.GetForward()*trans_factor);
		acc_vec += player_->GetForward();
		// std::cout << "z" << std::endl;
	}
	if (key_map_[GLFW_KEY_J]) {
		//camera_.Translate(-camera_.GetSide()*trans_factor);
		acc_vec += -player_->GetSide();
		// std::cout << "j" << std::endl;
	}
	if (key_map_[GLFW_KEY_L]) {
		//camera_.Translate(camera_.GetSide()*trans_factor);
		acc_vec += player_->GetSide();
		// std::cout << "l" << std::endl;
	}
	if (key_map_[GLFW_KEY_I]) {
		//camera_.Translate(camera_.GetUp()*trans_factor);
		acc_vec += -player_->GetUp();
		// std::cout << "i" << std::endl;
	}
	if (key_map_[GLFW_KEY_K]) {
		//camera_.Translate(-camera_.GetUp()*trans_factor);
		acc_vec += player_->GetUp();
		// std::cout << "k" << std::endl;
	}
	if (key_map_[GLFW_KEY_LEFT_SHIFT]) {
		//camera_.Translate(-camera_.GetUp()*trans_factor);
		if (glm::length(player_->GetVelocity()) > 0)
		{
			acc_vec += -glm::normalize(player_->GetVelocity());
		}
		// std::cout << "LShift" << std::endl;
	}
	if (key_map_[GLFW_KEY_LEFT_CONTROL]) {
		player_->ToggleView();
	}


	if (key_map_[GLFW_KEY_SPACE]) {
		player_->SetFiring(true);
	}

	if (acc_vec != glm::vec3(0.0))
		acc_vec = glm::normalize(acc_vec);


	player_->SetAcceleration(acc_vec);
	player_->Pitch(pitch * deltaTime);
	player_->Yaw(yaw * deltaTime);
	player_->Roll(roll * deltaTime);




	//if (key_map_[GLFW_KEY_LEFT_SHIFT]) {
	//	player_->SetVelocity(glm::vec3(0.0));
	//}
	//
	//if (key_map_[GLFW_KEY_LEFT_CONTROL]) {
	//	player_->SetOrientation(glm::quat());
	//}
	}

}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
}


Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    scene_.AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){

    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleSphereMesh", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
		ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float)rand() / RAND_MAX), glm::vec3(((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX), ((float)rand() / RAND_MAX)))));

		//randomize scales
		int min_size = 15;
		int max_size = 40;
		float scale = (min_size + (float)(rand() % (max_size - min_size))) / 10.0f;
		ast->SetScale(glm::vec3(scale));
    }
}

PlayerNode *Game::CreatePlayerInstance()
{
	// Get resources
	Resource *torus_geom = resman_.GetResource("SimpleTorusMesh");
	if (!torus_geom) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleTorusMesh" + std::string("\"")));
	}

	Resource *cylinder_geom = resman_.GetResource("SimpleCylinderMesh");
	if (!torus_geom) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleCylinderMesh" + std::string("\"")));
	}

	std::string material_name = "ObjectMaterial";
	Resource *obj_mat = resman_.GetResource(material_name);
	if (!obj_mat) {
		throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
	}
	Resource *laser_mat = resman_.GetResource(material_name);
	if (!laser_mat) {
		throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
	}

	// Create asteroid instance
	PlayerNode *player = new PlayerNode("Player", torus_geom, obj_mat, &camera_);

	SceneNode *player_laser = new PlayerNode("PlayerLaser", cylinder_geom, laser_mat, &camera_);

	Asteroid *player_engine_L = new Asteroid("PlayerEngineL", torus_geom, obj_mat);
	Asteroid *player_engine_R = new Asteroid("PlayerEngineR", torus_geom, obj_mat);

	player->SetLaser(player_laser);

	player->AddChild(player_engine_L);
	player->AddChild(player_engine_R);

	player_laser->SetScale(glm::vec3(.5f, 10.0f, .5f));
	player_laser->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	player_engine_L->SetScale(glm::vec3(.5f));
	player_engine_L->SetPosition(glm::vec3(-2.0f, 0.0, 2.0f));
	player_engine_L->SetAngM(glm::angleAxis(glm::pi<float>() / 8.0f, glm::vec3(0.0f, 1.0f, 0.0f)));

	player_engine_R->SetScale(glm::vec3(.5f));
	player_engine_R->SetPosition(glm::vec3(2.0f, 0.0, 2.0f));
	player_engine_R->SetAngM(glm::angleAxis(-glm::pi<float>() / 8.0f, glm::vec3(0.0f, 1.0f, 0.0f)));

	scene_.AddNode(player);
	return player;
}

} // namespace game
