#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GameManager.h"



class Application
{
public:
	Application(void);
	~Application();

	// MUST RUN BEFORE EVERYTHING ELSE
	void Initialize(void);
	//Set up Game Manager (Initialize Models, Create Player object, etc.)
	void SetupGameManager(void);
	//Set up Scene + Possibility for new levels
	void SetupScene(void);
	//Start the actual game loop
	void StartGame(void);

private:
	int id=0;

	//Key and Resize Callbacks
	GLFWwindow * window;

	//Game Manager
	GameManager gm;
	
	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;

	void SetupWindow(void);
	void SetupView(void);
	void SetupCallbacks(void);

	//Callback Functions
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void ResizeCallback(GLFWwindow* window, int width, int height);
	

	//Create a Single CubeFish
	GameObject* GenerateFish();
	//Create Multiple CubeFish
	std::vector<GameObject*> GenerateFishSet(int count);
};

