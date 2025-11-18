#pragma once

#include "entity.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "framework/camera.h"

class Camera;
class Mesh;

class EntityMesh : public Entity {
	public:
		//Attributes of the derived class
		Mesh* mesh = nullptr;
		//Check your framework's Material class
		Material material;

		// Constructores
		EntityMesh() {}
		EntityMesh(Mesh* m, const Material& mat, const std::string& name = "");

		bool culling = true;

		std::vector<Matrix44> models; 

		bool isInstance = false;
		std::vector<Matrix44> instanceModels;

		void setAsInstance(const std::vector<Matrix44>& models) {
			isInstance = true;
			instanceModels = models;
		}

		void render(Camera* camera) override;
		void update(float delta_time) override;
};
