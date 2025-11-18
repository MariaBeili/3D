#pragma once

#include "framework/utils.h"

class Camera;
class Player;
class EntityMesh;
class Entity;



class World {

	public:
		static World* instance;

		static World* getInstance() {
			if (instance != nullptr)
				return instance;

			instance = new World();
			//instance->init();  // ? THIS IS THE PROBLEM!
			return instance;
		}

	World();
	void init();

	// Core scene objects
	Camera* camera = nullptr;
	Entity* myscene = nullptr;
	//EntityMesh* skybox = nullptr;
	Player* player = nullptr;
	

	// runtime / debug
	float camera_speed = 2.0f;
	bool free_camera = true;
	float mouse_speed = 2.0f;

	bool wallDetected = false;

	float height = 1.0f;
	float sphere_radius = 0.5f;
	float spehre_ground_radius = 0.5f;

	void render();
	void update(double delta_time);



	void addEntity(Entity* entity);
	void destroyEntity(Entity* entity);

	// deferred destroy queue
	std::vector<Entity*> entities_to_destroy;
	
};
