#pragma once
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>

#include "GameObject.h"

#define MODEL_CUBE 0
#define MODEL_TORUS 1


typedef struct model {
	GLuint vbo; // OpenGL vertex buffer object
	GLuint ebo; // OpenGL element buffer object
	GLuint size; // Size of data to be drawn
} Model;



class GameManager
{
public:
	GameManager();
	~GameManager();

	//Set up and store Torus and Cube models
	void InitializeModels(void);
	void SetupShaders(void);

	void Add(GameObject* obj);
	void Add(std::vector<GameObject*> objSet);
	void Delete(int id);

	GameObject* GetObject(int index) { return objects.at(index); }
	std::vector<GameObject*> GetObjects(void) { return objects; }

	GameObject* GetPlayer(void) { return player; }
	Model* GetModel(int index) { return models.at(index); }

	void SetPlayer(GameObject* play) { player = play; }

	void UpdateObjects(float deltaTime);
	void RenderObjects(glm::mat4 view_matrix, glm::mat4 projection_matrix);
	void RenderPlayer(glm::mat4 view_matrix, glm::mat4 projection_matrix);

private:

	const char *source_vp;
	// Source code of fragment shader
	const char *source_fp;
	//Need to store a vector of models
	std::vector<Model*> models;
	std::vector<GameObject*> objects;
	GameObject* player;

	GLuint program;
	//Create new objects by passing in the index of said model.
	//Each object stores it's matrix info & shit


	Model* CreateCube(void);
	Model* CreateTorus(float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30);
};
