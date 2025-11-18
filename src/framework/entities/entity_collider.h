#pragma once

#include "entity_mesh.h"
#include "framework/collision.h"




class EntityCollider : public EntityMesh
{
	bool is_dynamic = false;
public:



	int layer = eCollisionFilter::SCENARIO;
	bool isInstanced = false;

	EntityCollider() {};
	EntityCollider(Mesh* mesh, const Material& material, const std::string& name = "");


	// Methods overwritten from base class	
	void render(Camera* camera) override;
	void update(float delta_time) override;

	// Collision helpers
	void getCollisionWithModel(const Matrix44& m, const Vector3& target_position,
		std::vector<sCollisionData>& collisions,
		std::vector<sCollisionData>& ground_collisions);

	void getCollisions(const Vector3& target_position,
		std::vector<sCollisionData>& collisions,
		std::vector<sCollisionData>& ground_collisions,
		eCollisionFilter filter);

	int getLayer() { return layer; };
	void setLayer(int new_layer) { layer = new_layer; };

};