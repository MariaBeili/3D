#pragma once

#include "framework/utils.h"
#include "game/object_spawner.h"
#include "framework/entities/entity_collider.h"


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
			return instance;
		}

	World();
	void init();

	// Core scene objects
	Camera* camera = nullptr;
	Entity* myscene = nullptr;
	EntityMesh* skybox = nullptr;
	Player* player = nullptr;
	ObjectSpawner* spawner = nullptr;
	

	// runtime / debug
	float camera_speed = 2.0f;
	bool free_camera = true;
	bool free_camera_just_enabled = false;
	float mouse_speed = 0.2f;

	bool wallDetected = false;

	float height = 3.0f;
	float sphere_radius = 0.75f;
	float spehre_ground_radius = 0.75f;


	void render();
	void update(double delta_time);



	void addEntity(Entity* entity);
	void destroyEntity(Entity* entity);

	std::vector<Entity*> entities_to_destroy;

	int hearts = 3;
	int coins = 0;
	
	std::vector<EntityCollider*> dynamic_objects;
	//void handleDynamicCollision(EntityCollider * obj);
	Entity* last_collision_entity = nullptr;

};
