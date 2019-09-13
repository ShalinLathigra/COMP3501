#include "Application.h"

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
glm::vec3 camera_position_g(0.0, 0.0, 3.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);


Application::Application()
{
}


Application::~Application()
{
}

void Application::Initialize(void)
{
	std::cout << "Initialize!" << std::endl;
	SetupWindow();
	SetupView();
	SetupCallbacks();

}

void Application::SetupWindow(void)
{
	std::cout << "Setup Window!" << std::endl;
	//Initialization stuff from Demos
	if (!glfwInit())
	{
		throw(std::runtime_error(std::string("Could not initialize GLFW library")));
	}

	window = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		throw(std::runtime_error(std::string("Could not Create Window")));
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
	}
}

void Application::SetupView(void)
{
	std::cout << "Setup View!" << std::endl;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	glViewport(0, 0, window_height_g, window_width_g);

	// Set current view 
	view_matrix = glm::lookAt(camera_position_g, camera_look_at_g, camera_up_g);

	// Set projection
	float top = tan((camera_fov_g / 2.0)*(glm::pi<float>() / 180.0))*camera_near_clip_distance_g;
	float right = top * window_width_g / window_height_g;
	projection_matrix = glm::frustum(-right, right, -top, top, camera_near_clip_distance_g, camera_far_clip_distance_g);

}

void Application::SetupCallbacks(void)
{
	std::cout << "Setup Callbacks!" << std::endl;
	glfwSetWindowUserPointer(window, (void *)&projection_matrix);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetFramebufferSizeCallback(window, ResizeCallback);
}

void Application::SetupGameManager(void)
{
	std::cout << "Setup Game Manager!" << std::endl;
	gm.InitializeModels();
}

void Application::SetupScene(void)
{
	std::cout << "Setup Scene!" << std::endl;
	gm.SetPlayer(new GameObject(MODEL_TORUS, id++, glm::vec3(), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
	gm.Add(GenerateFishSet(10));
}



//***************************************************************
//
//		Game Loop
//
//***************************************************************

void Application::StartGame(void)
{
	std::cout << "Start Game!" << std::endl;
	while (!glfwWindowShouldClose(window)) 
	{
		//Clear background
		glClearColor(viewport_background_color_g[0],
			viewport_background_color_g[1],
			viewport_background_color_g[2], 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static double last_time = 0;
		double current_time = glfwGetTime();
		float deltaTime = current_time - last_time;

		gm.UpdateObjects(deltaTime);

		last_time = current_time;

		gm.RenderObjects(view_matrix, projection_matrix);

		//Render Pipeline
		glfwPollEvents();

		//Push buffer drawn in background onto the display
		glfwSwapBuffers(window);
	}
}

//***************************************************************
//
//		Callbacks
//
//***************************************************************
// Callback for when a key is pressed
void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// Quit the program when pressing 'q'
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


// Callback for when the window is resized
void Application::ResizeCallback(GLFWwindow* window, int width, int height) {

	// Set OpenGL viewport based on framebuffer width and height
	glViewport(0, 0, width, height);

	// Update projection matrix
	void* ptr = glfwGetWindowUserPointer(window);
	glm::mat4 *projection_matrix = (glm::mat4 *) ptr;
	float top = tan((camera_fov_g / 2.0)*(glm::pi<float>() / 180.0))*camera_near_clip_distance_g;
	float right = top * width / height;
	(*projection_matrix) = glm::frustum(-right, right, -top, top, camera_near_clip_distance_g, camera_far_clip_distance_g);
}


//***************************************************************
//
//		Generate "Fish"
//
//***************************************************************
std::vector<GameObject*> Application::GenerateFishSet(int count)
{
	std::vector<GameObject*> retSet;
	for (int i = 0; i < count; i++)
	{
		retSet.push_back(GenerateFish());
	}

	return retSet;
}
 
GameObject* Application::GenerateFish(void)
{
	glm::vec3 pos, axi, sca;

	float x, y, z;
	x = -2.0f;
	y = -.5f + (float)(rand() % window_height_g) / (float)window_height_g;
	z = 1.0f + rand() % 5;
	pos = glm::vec3(x, y, z);

	float xA, yA, zA;
	xA = (rand() % 10) / 10.0f;
	yA = (rand() % 10) / 10.0f;
	zA = (rand() % 10) / 10.0f;
	axi = glm::vec3(xA, yA, zA);

	float s = 3.0f + (rand() % 7) / 10.0f;
	sca = glm::vec3(s, s, s);

	GameObject* newFish = new GameObject(MODEL_CUBE, id++, pos, axi, sca);
	return newFish;
}