#pragma once
#include "game/spawnableobject.h"
#include <vector>
#include <random>

// Gestiona tots els objectes i la seva aparició segons la probabilitat i la posició del jugador


class ObjectSpawner {
public:
	std::vector<SpawnableObject> object_types; // Tipus d'objectes que es poden aparèixer
	std::vector<EntityMesh*> active_objects; // Objectes actualment actius a l'escena

    ObjectSpawner();
    void init();
    void update(float dt, const Vector3& player_pos);
    void spawnObject(const SpawnableObject& type, const Vector3& player_pos);
    void cleanupPassedObjects(const Vector3& player_pos);



};

