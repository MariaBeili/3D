#pragma once

#include "entity_mesh.h"
#include "framework/includes.h"
#include "framework/framework.h"
#include "framework/camera.h"

enum eUIButtonID {
	UI_BUTTON_UNDEFINED,
	UI_BUTTON_PLAY,
	UI_BUTTON_START,
	UI_BUTTON_OPTIONS,
	UI_BUTTON_EXIT
};


class EntityUI : public EntityMesh {

public:
	EntityUI(Vector2 size, const Material& material);
	EntityUI(Vector2 pos, Vector2 size, const Material& material, eUIButtonID button_id = UI_BUTTON_UNDEFINED, const std::string& name = "");

	Vector2 position;
	Vector2 size;

	float mask = 1.0f;

	bool is3D = false;
	bool visible = true;
	Vector3 pos3d;

	eUIButtonID button_id = UI_BUTTON_UNDEFINED;

	void render(Camera* camera);
	void update(float delta_time);
	void update3D(const Vector3& position3d);
};