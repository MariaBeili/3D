#pragma once

#include "framework/entities/entity_mesh.h"
#include "framework/includes.h"

class Camera;
class Player : public EntityMesh {


public:
		float walk_speed = 0.0f;
		float run_speed = 10.0f;
		bool render_mesh = true;
		Vector3 velocity = Vector3(0.0f);
		float speed = 5.0f;

		float last_camera_yaw = 0.0f;
		float angle_y = 0.0f;

		const Vector3& getMovementDirection();
		Entity* character = nullptr;
		static Player* instance;

		bool has_jetpack = false;       // Inventario
		bool is_jetpack_active = false; // Estado de vuelo

		Player() {};
		Player(Mesh* mesh, const Material& material, const std::string& name = "");

		void update(float delta_time) override;
		void render(Camera* camera) override;

		void setRenderMesh(bool new_render_mesh) { render_mesh = new_render_mesh; }
		bool canMove(const Vector3& new_position);
		bool isGrounded(const Vector3& new_position, float max_ray_dist, Vector3& col_point);
		
		void activateJetpack();
};