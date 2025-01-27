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
float camera_fov_g = 20.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 camera_position_g(0.5, 0.5, 10.0);
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

	// Load material to be applied to particles
	std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/particle");
	resman_.LoadResource(Material, "ParticleMaterial", filename.c_str());
	// Load material to be applied to flamethrower
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/fire");
	resman_.LoadResource(Material, "FireMaterial", filename.c_str());
	// Load material to be applied to ring effect
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/ring");
	resman_.LoadResource(Material, "RingMaterial", filename.c_str());

	// Create particles for explosion
	resman_.CreateSphereParticles("SphereParticles");
	// Create particles for explosion
	resman_.CreateFireParticles("FireParticles");
	// Create particles for explosion
	resman_.CreateRingParticles("RingParticles");

	//Flame effect texture
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/flame4x4orig.png");
	resman_.LoadResource(Texture, "Flame", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

    // Create particles
	game::SceneNode *fireworks1 = CreateInstance("FireworksInstance1", "SphereParticles", "ParticleMaterial");
	game::SceneNode *fireworks2 = CreateInstance("FireworksInstance2", "SphereParticles", "ParticleMaterial");
	game::SceneNode *fireworks3 = CreateInstance("FireworksInstance3", "SphereParticles", "ParticleMaterial");

	fireworks3->SetScale(glm::vec3(0.0));
	fireworks1->SetScale(glm::vec3(0.0));
	fireworks2->SetScale(glm::vec3(0.0));


	game::SceneNode *fire1 = CreateInstance("FireInstance1", "FireParticles", "FireMaterial", "Flame");
	fire1->SetBlending(true);
	fire1->SetScale(glm::vec3(0.0));
			
	game::SceneNode *ring1 = CreateInstance("RingInstance1", "RingParticles", "RingMaterial");
	ring1->SetBlending(true);
	ring1->SetScale(glm::vec3(0.0));
}

void Game::ResetFirework(SceneNode* node, float current)
{
	float x = -1 + 2.0 * (float)rand() / RAND_MAX;
	float y = -1 + 2.0 * (float)rand() / RAND_MAX;
	float z = 0 + 1.0 * (float)rand() / RAND_MAX;
	node->SetPosition(glm::vec3(x, y, z));
	node->SetStart(current);
	node->SetEnd(current + 2.0);
	x = -1 + 2.0 * (float)rand() / RAND_MAX;
	y = -1 + 2.0 * (float)rand() / RAND_MAX;
	z = 0 + 1.0 * (float)rand() / RAND_MAX;
	node->SetMomentum(glm::vec3(x, y, z));
	node->SetColorAtt(glm::vec3(abs(x), abs(y), abs(z)));
}

void Game::ToggleFireworks(bool state)
{
	SceneNode* node1 = scene_.GetNode("FireworksInstance1");
	SceneNode* node2 = scene_.GetNode("FireworksInstance2");
	SceneNode* node3 = scene_.GetNode("FireworksInstance3");

	if (!state)
	{
		node1->SetScale(glm::vec3(0.0));
		node2->SetScale(glm::vec3(0.0));
		node3->SetScale(glm::vec3(0.0));
	}
	else
	{
		node1->SetScale(glm::vec3(1.0));
		node2->SetScale(glm::vec3(1.0));
		node3->SetScale(glm::vec3(1.0));
	}
}
void Game::ToggleFlamethrower(bool state)
{
	SceneNode* node = scene_.GetNode("FireInstance1");
	if (!state)
	{
		node->SetScale(glm::vec3(0.0));
	}
	else
	{
		node->SetScale(glm::vec3(1.0));
	}
}
void Game::ToggleRing(bool state)
{
	SceneNode* node = scene_.GetNode("RingInstance1");
	if (!state)
	{
		node->SetScale(glm::vec3(0.0));
	}
	else
	{
		node->SetScale(glm::vec3(1.0));
	}
}
void Game::MainLoop(void){

    // Loop while the user did not close the window
	float current = glfwGetTime();
	float last_time = glfwGetTime();
	float start1 = 0.0;
	float start2 = 0.7;
	float start3 = 1.4;
	
    while (!glfwWindowShouldClose(window_)){
		current = glfwGetTime();
		float deltaTime = current - last_time;
		start1 += deltaTime;
		start2 += deltaTime;
		start3 += deltaTime;

		if (start1 >= 2.0)
		{
			SceneNode* node = scene_.GetNode("FireworksInstance1");
			ResetFirework(node, current);
			start1 = 0.0;
		}
		if (start2 >= 2.0)
		{
			SceneNode* node = scene_.GetNode("FireworksInstance2");
			ResetFirework(node, current);
			start2 = 0.0;
		}
		if (start3 >= 2.0)
		{
			SceneNode* node = scene_.GetNode("FireworksInstance3");
			ResetFirework(node, current);
			start3 = 0.0;
		}

        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();

		last_time = current;
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

    // Stop animation if space bar is pressed
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        game->animating_ = (game->animating_ == true) ? false : true;
    }

    // View control
    float rot_factor(glm::pi<float>() / 180);
    float trans_factor = 1.0;
    if (key == GLFW_KEY_UP){
        game->camera_.Pitch(rot_factor);
    }
    if (key == GLFW_KEY_DOWN){
        game->camera_.Pitch(-rot_factor);
    }
    if (key == GLFW_KEY_LEFT){
        game->camera_.Yaw(rot_factor);
    }
    if (key == GLFW_KEY_RIGHT){
        game->camera_.Yaw(-rot_factor);
    }
    if (key == GLFW_KEY_S){
        game->camera_.Roll(-rot_factor);
    }
    if (key == GLFW_KEY_X){
        game->camera_.Roll(rot_factor);
    }
    if (key == GLFW_KEY_A){
        game->camera_.Translate(game->camera_.GetForward()*trans_factor);
    }
    if (key == GLFW_KEY_Z){
        game->camera_.Translate(-game->camera_.GetForward()*trans_factor);
    }
    if (key == GLFW_KEY_J){
        game->camera_.Translate(-game->camera_.GetSide()*trans_factor);
    }
	if (key == GLFW_KEY_L) {
		game->camera_.Translate(game->camera_.GetSide()*trans_factor);
	}
	if (key == GLFW_KEY_I) {
		game->camera_.Translate(game->camera_.GetUp()*trans_factor);
	}
	if (key == GLFW_KEY_K) {
		game->camera_.Translate(-game->camera_.GetUp()*trans_factor);
	}

	if (key == GLFW_KEY_1) {
		game->ToggleFireworks(true);
		game->ToggleFlamethrower(false);
		game->ToggleRing(false);
	}
	if (key == GLFW_KEY_2) {
		game->ToggleFireworks(false);
		game->ToggleFlamethrower(true);
		game->ToggleRing(false);
	}
	if (key == GLFW_KEY_3) {
		game->ToggleFireworks(false);
		game->ToggleFlamethrower(false);
		game->ToggleRing(true);
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
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}


SceneNode *Game::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name){

    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }

    Resource *tex = NULL;
    if (texture_name != ""){
        tex = resman_.GetResource(texture_name);
        if (!tex){
            throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
        }
    }

    SceneNode *scn = scene_.CreateNode(entity_name, geom, mat, tex);
    return scn;
}

} // namespace game
