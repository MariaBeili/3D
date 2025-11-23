#include "object_spawner.h"
#include "game/world.h"
#include <iostream>

ObjectSpawner::ObjectSpawner() {}

void ObjectSpawner::init() {

    // Afegim objectes segons el document de disseny
    //object_types.push_back(SpawnableObject("Jetpack", "data/jetpack/jetpack.obj", "data/jetpack/texture.png", 4.0f, 0.4f, 25.0f));
    object_types.push_back(SpawnableObject("Coin", "data/coin/coin.obj", "data/coin/coinC_2.png", 2.0f, 0.7f, 20.0f));
}

void ObjectSpawner::update(float dt, const Vector3& player_pos) {
    if (object_types.empty()) return;
    for (SpawnableObject& type : object_types) {
        type.spawn_timer += dt;

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
    // Apareix una mica més avall (en direcció de caiguda)
    Vector3 spawn_pos = player_pos;
    spawn_pos.y -= type.spawn_distance; // davant (avall) del jugador

    // Dispersió lateral aleatòria
    spawn_pos.x += (rand() % 20 - 10);
    spawn_pos.z += (rand() % 20 - 10);

    EntityMesh* obj = new EntityMesh(type.mesh, type.material, type.name);
    obj->model.setTranslation(spawn_pos);
    World::instance->myscene->addChild(obj);
    active_objects.push_back(obj);

    std::cout << "[SPAWN] Objecte " << type.name << " creat a ("
        << spawn_pos.x << ", " << spawn_pos.y << ", " << spawn_pos.z << ")\n";
}

void ObjectSpawner::cleanupPassedObjects(const Vector3& player_pos) {
    std::vector<EntityMesh*>& vec = active_objects; // guarda tots els objectes generats pel spawner que encara existeixen a escena.
	vec.erase(std::remove_if(vec.begin(), vec.end(), [&](EntityMesh* obj) { // Lambda per eliminar objectes passats
        if (!obj) return true;
        float obj_y = obj->model.getTranslation().y;

        // Si l’objecte ja està per sobre del jugador (el jugador l’ha passat)
        if (obj_y > player_pos.y) {
            std::cout << "[CLEANUP] Eliminant objecte " << obj->name << " (y=" << obj_y << ")\n";
            World::instance->destroyEntity(obj);
            return true;
        }
        return false;
        }), vec.end());
}