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
const glm::vec3 viewport_background_color_g(0.0, 0.25, 0.0);
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

	// Create a torus
	resman_.CreateTorus("TorusMesh");

	// Create a Sphere
	resman_.CreateSphere("SphereMesh");

	// Create geometry of the "wall"
	resman_.CreateWall("WallMesh");

	// Create geometry of the "Cylinder"
	resman_.CreateCylinder("CylinderMesh");

	// Load a cube from an obj file
	std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/cube.obj");
	resman_.LoadResource(Mesh, "CubeMesh", filename.c_str());


	// Load material to be applied to torus
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/three-term_shiny_blue");
	resman_.LoadResource(Material, "ShinyBlueMaterial", filename.c_str());

	// Load texture to be applied to the cube
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/checker.png");
	resman_.LoadResource(Texture, "Checker", filename.c_str());

	// Load texture to be applied to the cube
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/Argyle.png");
	resman_.LoadResource(Texture, "Argyle", filename.c_str());

	// Load material to be applied to the cube
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material");
	resman_.LoadResource(Material, "TexturedMaterial", filename.c_str());

	// Load material to be applied to the Toon Objects
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/toon_material");
	resman_.LoadResource(Material, "ToonMaterial", filename.c_str());

    // Load material to be used for normal mapping
	filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map");
    resman_.LoadResource(Material, "NormalMapMaterial", filename.c_str());

    // Load texture to be used in normal mapping
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map2.png");
    resman_.LoadResource(Texture, "NormalMap", filename.c_str());
}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

    // Create an instance of the wall
    game::SceneNode *wall = CreateInstance("WallInstance1", "WallMesh", "NormalMapMaterial", "NormalMap");
	wall->Scale(glm::vec3(.4f, .4f, .4f));
	wall->Translate(glm::vec3(0.0f, -0.5f, 0.0f));

	// Create an instance of the torus mesh
	game::SceneNode *torus1 = CreateInstance("TorusInstance1", "TorusMesh", "ShinyBlueMaterial");
	game::SceneNode *torus2 = CreateInstance("TorusInstance2", "TorusMesh", "ToonMaterial");
	// Scale the instance
	torus1->Scale(glm::vec3(.75, .75, .75));
	torus1->Translate(glm::vec3(-1.7, 0.5, 0.0));
	torus2->Scale(glm::vec3(.75, .75, .75));
	torus2->Translate(glm::vec3(-1.1, -0.5, 0.0));

	// Create an instance of the textured cube
	game::SceneNode *checker1 = CreateInstance("CubeInstance1", "CubeMesh", "TexturedMaterial", "Checker");
	// Adjust the instance
	checker1->Scale(glm::vec3(0.5, 0.5, 0.5));
	glm::quat rotation = glm::angleAxis(-45.0f * -glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
	checker1->Rotate(rotation);
	checker1->Translate(glm::vec3(1.4, 0.6, 0.0));

	// Create an instance of the textured cube
	game::SceneNode *checker2 = CreateInstance("CylinderInstance1", "CylinderMesh", "TexturedMaterial", "Checker");
	// Adjust the instance
	checker2->Scale(glm::vec3(0.75, 0.75, 0.75));
	rotation = glm::angleAxis(-45.0f * -glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
	checker2->Rotate(rotation);
	checker2->Translate(glm::vec3(1.4, -0.6, 0.0));



	game::SceneNode *cannon_0 = CreateInstance("CannonInstance0", "CylinderMesh", "TexturedMaterial", "Checker");
	cannon_0->Scale(glm::vec3(0.3, .75, 0.3));
	cannon_0->Translate(glm::vec3(0.0, 0.5, 0.0));

	game::SceneNode *cannon_1 = CreateInstance("CannonInstance1", "CylinderMesh", "TexturedMaterial", "Argyle");
	cannon_1->Scale(glm::vec3(0.6, 0.4, 0.6));
	cannon_1->Translate(glm::vec3(0.0, 0.975, 0.0));
	cannon_1->Rotate(rotation);

	game::SceneNode *cannon_2 = CreateInstance("CannonInstance2", "CylinderMesh", "TexturedMaterial", "Argyle");
	cannon_2->Scale(glm::vec3(0.1, 0.75, 0.1));
	cannon_2->SetJoint(glm::vec3(0.0, -0.375, 0.0));

	game::SceneNode *cannon_3 = CreateInstance("CannonInstance3", "CylinderMesh", "TexturedMaterial", "Checker");
	cannon_3->Scale(glm::vec3(0.05, 0.5, 0.05));
	cannon_3->Translate(glm::vec3(0.0, 0.5, 0.0));

}


void Game::MainLoop(void){

    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){
        // Animate the scene
		static double last_time = 0;
		double current_time = glfwGetTime();
		if ((current_time - last_time) > 0.01) {
			float deltaTime = current_time - last_time;
			//scene_.Update();
			if (front)
			{
				camera_.Translate(camera_.GetForward() * 5.0f * deltaTime);
			}
			if (back)
			{
				camera_.Translate(-camera_.GetForward() * 5.0f  * deltaTime);
			}
			if (left)
			{
				camera_.Translate(camera_.GetSide() * 5.0f  * deltaTime);
			}
			if (right)
			{
				camera_.Translate(-camera_.GetSide() * 5.0f  * deltaTime);
			}
			if (up)
			{
				camera_.Translate(camera_.GetUp() * 5.0f  * deltaTime);
			}
			if (down)
			{
				camera_.Translate(-camera_.GetUp() * 5.0f  * deltaTime);
			}

			if (animating_) {
				// Animate the wall
				SceneNode *node = scene_.GetNode("WallInstance1");
				glm::quat rotation = glm::angleAxis(glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
				node->Rotate(rotation);

				node = scene_.GetNode("TorusInstance1");
				rotation = glm::angleAxis(glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
				node->Rotate(rotation);

				node = scene_.GetNode("TorusInstance2");
				rotation = glm::angleAxis(glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
				node->Rotate(rotation);

				// Animate the cube
				node = scene_.GetNode("CubeInstance1");
				rotation = -glm::angleAxis(glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
				node->Rotate(rotation);

				// Animate the Cylinder
				node = scene_.GetNode("CylinderInstance1");
				rotation = glm::angleAxis(glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
				node->Rotate(rotation);

				node = scene_.GetNode("CannonInstance1");
				rotation = glm::angleAxis(glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
				node->Rotate(rotation);
				glm::mat4 mat = node->ComposeMatrix();

				//// Animate the Cannon
				node = scene_.GetNode("CannonInstance2");
				float amount = sin(current_time) / 8.0f;
				rotation = glm::angleAxis(amount * glm::pi<float>() + glm::pi<float>() / 2.0f, glm::vec3(0.0, 0.0, 1.0));
				node->SetOrbit(rotation);
				node->SetP(mat);
				mat = node->ComposeMatrix();

				node = scene_.GetNode("CannonInstance3");
				amount = sin(current_time * 2.0f) * 0.25f + 0.25f;
				node->SetPosition(glm::vec3(0.0, 1.0, 0.0) * amount);
				node->SetP(mat);

			}
			last_time = current_time;
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

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_A) {
			game->front = true;
		}
		if (key == GLFW_KEY_Z) {
			game->back = true;
		}
		if (key == GLFW_KEY_J) {
			game->right = true;
		}
		if (key == GLFW_KEY_L) {
			game->left = true;
		}
		if (key == GLFW_KEY_I) {
			game->up = true;
		}
		if (key == GLFW_KEY_K) {
			game->down = true;
		}
	}
	if (action == GLFW_RELEASE)
	{
		game->front = false;
		game->back = false;
		game->right = false;
		game->left = false;
		game->up = false;
		game->down = false;
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
