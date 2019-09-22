#include "GameManager.h"


void GameManager::Update(float deltaTime)
{
	//Handle Adding a new entity
	if (count < maxCount)
	{
		if (spawnTimer > 0)
		{
			spawnTimer -= deltaTime;
		}
		else
		{
			AddRandomEntity();
			//Spawn faster in hard mode
			spawnTimer = (pointTotal < hardMode) ? maxSpawnTimer : maxSpawnTimer * .25f;
		}
	}

	//Update Entities  + Handle Collisions/Points
	player->Update(deltaTime);

	for (int i = 0; i < count; i++)
	{
		entities[i].Update(deltaTime);

		//Clear Entities That have timed out. "Hard Mode" Deducts points for timed out enemies
		if (entities[i].IsAlive() == false)
		{
			if (pointTotal > hardMode)
			{
				pointTotal -= 50;
				std::cout << pointTotal << std::endl;
			}
			entities[i] = entities[count - 1];
			count--;
			i--;
		}

		float dist = HorizontalDistBetween(player->GetPosition(), entities[i].GetPosition());
		float deltaZ = abs(abs(player->GetPosition().z) - abs(entities[i].GetPosition().z));
		//std::cout << dist << " " << deltaZ << std::endl;
		// if deltaZ is small
		//if dist is small
		if (deltaZ < .05f)
		{
			if (dist < .3f * .3f && player->GetVelocity().z < 0)
			{
				pointTotal += 250;
				std::cout << pointTotal << std::endl;

				entities[i] = entities[count - 1];
				count--;
				i--;
			}
			else if (dist < .5f * .5f)
			{
				//Calculate new direction * 3 for MORE SPEED
				glm::vec3 newDir = entities[i].GetPosition() - player->GetPosition();
				newDir = newDir / (sqrt(newDir.x * newDir.x + newDir.y * newDir.y + newDir.z * newDir.z));
				entities[i].SetDirection(newDir * 3.0f);
			}
		}
	}
}

//Calculate X Y dist between 2 points
float GameManager::HorizontalDistBetween(glm::vec3 a, glm::vec3 b)
{
	glm::vec3 c = b - a;
	return c.x*c.x + c.y*c.y;
}
//Add an Entity to the Game
void GameManager::AddEntity(Entity newEntity)
{
	if (count < maxCount)
	{
		entities[count++] = newEntity;
	}
}

//Handle Input, Set player dir accordingly
void GameManager::HandleInput(int key)
{
	glm::vec3 vel(0);
	if (player->CanMove())
	{
		if (key == GLFW_KEY_SPACE)
		{
			player->Fall();
			return;
		}
		if (key == GLFW_KEY_W)
		{
			vel.y = 1;
		}
		else if (key == GLFW_KEY_S)
		{
			vel.y = -1;
		}
		else if (key == GLFW_KEY_A)
		{
			vel.x = -1;
		}
		else if (key == GLFW_KEY_D)
		{
			vel.x = 1;
		}
		player->SetDirection(vel);
	}

}

//Retrieve entity at index
Entity* GameManager::GetEntity(int index)
{
	if (index >= count)
	{
		return player;
	}
	else
	{
		return &entities[index];
	}
}


//Generate a random entity, add it to the array
void GameManager::AddRandomEntity()
{
	//std::cout << "AddRandomEntity!" << std::endl;
	//need to create entity, set model to Torus, set position, vel
	//Need to pick a starting point

	//0,1,2 -1 = -1,0,1
	float x = (((rand() % 20) / 10.0f) - 1.0f) * .8f; //*.8f to restrict
	float y = (((rand() % 20) / 10.0f) - 1.0f) * .8f * .8f; //*.8f to restrict, *.8f to scale fitting to x


		
	glm::vec3 startPos(x, y, -(float)(1 + rand() % maxDepth));
	//Give random direction (Sin theta, cos theta).
	//range = -1/8 to 1/8
	float a = (float)(rand() % 10)/5.0f + 15.0f;

	//Entities all move Generally to the right, with some z-ward movement
	float theta = (a/8.0f)*glm::pi<GLfloat>();
	glm::vec3 dir = glm::vec3(cos(theta), sin(theta), (rand() % 100 - 50) / 10.0f);

	float speed = minSpawnSpeed + (maxSpawnSpeed - minSpawnSpeed) * (float)((rand() % 10) / 10.0f);

	Entity entity = Entity();
	entity.SetPosition(startPos);
	entity.SetDirection(dir);
	entity.SetSpeed(speed);
	entity.SetAxis(glm::vec3(rand()%2-1, rand() % 2 - 1, rand() % 2 - 1));
	entity.SetRotSpeed(90.0f + (float)(rand()%180));
	entity.SetLifeSpan(boxDuration);

	AddEntity(entity);
}
//Add Player GameObject
void GameManager::AddPlayer(Entity* play)
{
	player = play;
}

void GameManager::printVec3(glm::vec3 vec)
{
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}
void GameManager::printVec2(glm::vec2 vec)
{
	std::cout << "(" << vec.x << ", " << vec.y << ")";
}

//Constructor
GameManager::GameManager()
{
	count = 0;
	maxCount = 25;
	maxSpawnTimer = 2.0f;
	spawnTimer = maxSpawnTimer;
	pointTotal = 0;
	minSpawnSpeed = 0.1f;
	maxSpawnSpeed = 0.5f;
	boxDuration = 8.5f;
	hardMode = 750;

	entities = new Entity[maxCount * sizeof(Entity*)]; //Array. Size = sizeof(Entity) * maxCount

	maxDepth = 5;

}


GameManager::~GameManager()
{
}
