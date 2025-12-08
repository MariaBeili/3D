#include "object_spawner.h"
#include "game/world.h"
#include "game/player.h"
#include <iostream>

ObjectSpawner::ObjectSpawner() {}

void ObjectSpawner::init() {

    // Afegim objectes segons el document de disseny
    object_types.push_back(SpawnableObject("jetpack", "data/jetpack/jetpack.obj", "data/jetpack/texture.png", 4.0f, 0.7f, 600.0f));
    object_types.push_back(SpawnableObject("coin", "data/coin/coin.obj", "data/coin/coinC_2.png", 2.0f, 0.8f, 600.0f));
    object_types.push_back(SpawnableObject("box", "data/box/box.obj", "data/box/Wooden_Box_Crate_DefaultMaterial_BaseColor.png", 3.0f, 0.9f, 600.0f));
}

void ObjectSpawner::update(float dt, const Vector3& player_pos) {
    if (object_types.empty()) return;
    if (!World::instance || !World::instance->player) return;
    
    Player* player = World::instance->player;

    Vector3 p = player->model.getTranslation();

    for (SpawnableObject& type : object_types) {
        type.spawn_timer += dt;

        if (type.name == "jetpack" && p.y > -1500.0f)
            continue;

        if (type.spawn_timer >= type.spawn_interval) {
            type.spawn_timer = 0.0f;

            if ((float)rand() / RAND_MAX <= type.probability) {
                spawnObject(type, player_pos);
            }
        }
    }

    // Neteja els objectes que el jugador ja ha passat
    cleanupPassedObjects(player_pos);
}

void ObjectSpawner::spawnObject(const SpawnableObject& type, const Vector3& player_pos) {
    Vector3 spawn_pos = player_pos;
    spawn_pos.x += (rand() % 20 - 10);
    spawn_pos.y -= type.spawn_distance-450 - (rand() % 20 - 10);;
    spawn_pos.z += type.spawn_distance-560+ (rand() % 20 - 10);

    EntityCollider* obj = new EntityCollider(type.mesh, type.material, type.name);
    obj->model.setTranslation(spawn_pos);
    if(obj->name == "box")
    {
        obj->model.scale(1.75);
    }
    if (obj->name == "jetpack")
    {
        obj->model.scale(2.5);
    }

    obj->destroy_timer = -1.0f; // not yet destroyed
    obj->picked = false;

    World::instance->myscene->addChild(obj);
    active_objects.push_back(obj);

    std::cout << "[SPAWN] Object " << type.name << " at ("
        << spawn_pos.x << ", " << spawn_pos.y << ", " << spawn_pos.z << ")\n";
}


void ObjectSpawner::cleanupPassedObjects(const Vector3& player_pos) {
    std::vector<EntityCollider*>& vec = active_objects; // guarda tots els objectes generats pel spawner que encara existeixen a escena.
	vec.erase(std::remove_if(vec.begin(), vec.end(), [&](EntityCollider* obj) { // Lambda per eliminar objectes passats
        if (!obj) return true;

        if (obj->name == "coin" && obj->parent == nullptr)
            return true; 
        float obj_y = obj->model.getTranslation().y;

        // Si l’objecte ja està per sobre del jugador (el jugador l’ha passat)
        if (obj_y > player_pos.y+15) {
            std::cout << "[CLEANUP] Eliminant objecte " << obj->name << " (y=" << obj_y << ")\n";
            World::instance->destroyEntity(obj);
            return true;
        }
        return false;
        }), vec.end());
}