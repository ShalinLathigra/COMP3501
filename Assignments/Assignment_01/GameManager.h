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
public:
	inline int GetCount() { return count; }
	void printVec3(glm::vec3 vec);
	void printVec2(glm::vec2 vec);
	Entity* GetEntity(int index);
	int count;
	void AddPlayer(Entity* play);
	float HorizontalDistBetween(glm::vec3 a, glm::vec3 b);
	void Update(float deltaTime);
	void AddEntity(Entity newEntity);
	void HandleInput(int key);
	void AddRandomEntity();
	GameManager();
	~GameManager();
};

