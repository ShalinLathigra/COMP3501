#pragma once

#include "Includes.h"
#include "Entity.h"

class GameManager
{
private:
	Entity* player;
	Entity* entities; //Array. Size = sizeof(Entity) * maxCount
	glm::vec2 corners[8];
	int maxCount;
	float spawnTimer;
	float maxSpawnTimer;
	int pointTotal;
	int hardMode;
	int spawnDistance;
	float minSpawnSpeed;
	float maxSpawnSpeed;
	int maxDepth;
	float boxDuration;
	float maxRotSpeed;
	int count;

public:
	//Getters
	inline int GetCount() { return count; }
	Entity* GetEntity(int index);

	//Utility Functions
	void printVec3(glm::vec3 vec);
	void printVec2(glm::vec2 vec);

	//Update + Functino to calculate xy dist between two vectors
	float HorizontalDistBetween(glm::vec3 a, glm::vec3 b);
	void Update(float deltaTime);

	//Add player/Entities to collections
	void AddPlayer(Entity* play);
	void AddEntity(Entity newEntity);
	void AddRandomEntity();

	//pretty self explanatory. Handles the input key
	void HandleInput(int key);
	GameManager();
	~GameManager();
};

