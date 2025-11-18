#include "entity_collider.h"
#include "framework/collision.h"
#include "game/world.h"
#include "game/game.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include <iostream>
#include <game/game.h>
#include "game/player.h"

// ------------------------------------------------------
// CONSTRUCTOR
// ------------------------------------------------------
EntityCollider::EntityCollider(Mesh* mesh, const Material& material, const std::string& name)
{
	this->mesh = mesh;
	this->material = material;
	this->name = name;
	this->layer = eCollisionFilter::SCENARIO; // Default layer
	this->is_dynamic = false;
}

// ------------------------------------------------------
// RENDER
// ------------------------------------------------------
void EntityCollider::render(Camera* camera)
{
	if (!mesh || !camera)
		return;

	EntityMesh::render(camera);


}


// ------------------------------------------------------
// UPDATE
// ------------------------------------------------------
void EntityCollider::update(float delta_time)
{
	if (!is_dynamic) return;

	Vector3 position = model.getTranslation();
	Vector3 velocity = Vector3(0, -9.8f * delta_time, 0); // simple gravity

	Vector3 new_position = position + velocity * delta_time;

	// Ground collision
	std::vector<sCollisionData> collisions;
	std::vector<sCollisionData> ground_collisions;

	getCollisions(new_position, collisions, ground_collisions, eCollisionFilter::SCENARIO);

	if (!ground_collisions.empty())
	{
		// push up to ground
		position.y = ground_collisions[0].col_point.y;
		velocity.y = 0;
	}

	model.setTranslation(position);
	EntityMesh::update(delta_time);
}

void EntityCollider::getCollisionWithModel(const Matrix44& m, const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions)
{
	Vector3 collision_point;
	Vector3 collision_normal;
	Vector3 center = target_position;

	float sphere_radius = World::instance->sphere_radius;
	float sphere_ground_radius = World::instance->spehre_ground_radius;
	float player_height = World::instance->height;

	//floor collisions 
	Vector3 floor_sphere_center = center + Vector3(0.0f, sphere_ground_radius, 0.0f);

	if (mesh->testSphereCollision(m, floor_sphere_center, sphere_radius, collision_point, collision_normal)) {
		collisions.push_back({ collision_point, collision_normal.normalize(), floor_sphere_center.distance(collision_point) });
	}
	Vector3 character_center = center + Vector3(0.0f, player_height, 0.0f);
	if (mesh->testSphereCollision(m, character_center, sphere_radius, collision_point, collision_normal)) {
		World::instance->wallDetected = true;
		collisions.push_back({ collision_point, collision_normal.normalize(), character_center.distance(collision_point) });
	}
	if (mesh->testRayCollision(m, character_center, Vector3(0, -1, 0), collision_point, collision_normal, player_height + 0.01f)) {
		ground_collisions.push_back({ collision_point, collision_normal.normalize(), character_center.distance(collision_point) });
	}

}

void EntityCollider::getCollisions(const Vector3& target_position, std::vector<sCollisionData>& collisions, std::vector<sCollisionData>& ground_collisions, eCollisionFilter filter)
{
	if (!(layer & filter))
		return;

	if (!isInstanced) {
		getCollisionWithModel(model, target_position, collisions, ground_collisions);
	}
	else {
		for (int i = 0; i < models.size(); i++) {
			getCollisionWithModel(models[i], target_position, collisions, ground_collisions);
		}
	}
}