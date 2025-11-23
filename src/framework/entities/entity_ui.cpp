#include "entity_ui.h"
#include "game/game.h"
#include "graphics/shader.h"
#include "framework/camera.h"
#include "game/world.h"
#include "framework/utils.h"
#include "framework/includes.h"
#include "framework/input.h"

EntityUI::EntityUI(Vector2 new_size, const Material& material) {

	size = new_size;

	this->material = material;

	if (!this->material.shader) {
		this->material.shader = Shader::Get("data/shader/basic.vs", material.diffuse ? "data/shader/texture.fs": "data/shader/caiguda.fs");
	}



}

EntityUI::EntityUI(Vector2 new_pos, Vector2 new_size, const Material& material, eUIButtonID new_button_id, const std::string& name) {

	position = new_pos;
	size = new_size;
	button_id = new_button_id;

	mesh = new Mesh();
	mesh->createQuad(position.x, position.y, size.x, size.y, true);


	this->material = material;
	if (!this->material.shader) {
		this->material.shader = Shader::Get("data/shader/basic.vs", material.diffuse ? "data/shader/texture.fs" : "data/shader/caiguda.fs");
	}



}

void EntityUI::render(Camera* camera2d) {

	if (!visible) {
		return;
	}

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	material.shader->enable();

	Matrix44 viewproj = camera2d->viewprojection_matrix;

	material.shader->setUniform("u_viewprojection", viewproj);
	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_mask", mask);

	if(material.diffuse) {
		material.shader->setUniform("u_texture", material.diffuse, 0);
	}


	mesh->render(GL_TRIANGLES);
	material.shader->disable();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	Entity::render(camera2d);
}




void EntityUI::update(float delta_time) {

	Vector2 mouse_pos = Input::mouse_position;

	if (button_id != UI_BUTTON_UNDEFINED &&
		mouse_pos.x > (position.x - size.x * 0.5f) &&
		mouse_pos.x < (position.x + size.x * 0.5f) &&
		mouse_pos.y >(position.y - size.y * 0.5f) &&
		mouse_pos.y < (position.y + size.y * 0.5f)) {

		// The mouse is over the button
		material.color = Vector4(1.5f); // Change color to indicate hover

		if (Input::wasMouseReleased(SDL_BUTTON_LEFT)) {
			// Button clicked
			switch (button_id) {
			case UI_BUTTON_PLAY:
				Game::instance->setStage(PLAYSTAGE);
				break;
			case UI_BUTTON_EXIT:
				break;
			default:
				break;
			}

		}
		else if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
			// Button is being pressed
			material.color = Vector4(0.9f, 0.9f, 0.9f, 1.0f ); // Change color to indicate press
		}
	}
	else {
		material.color = Vector4::WHITE; // Default color
		Entity::update(delta_time);
	}
}



void EntityUI::update3D(const Vector3& position3D )
{
	pos3d = position3D;

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	visible = true;

	Vector3 pos = Camera::current->project(position3D, width, height);
	if (pos.z < 0.0f || pos.z > 1.0f) {
		visible = false;

	}
	else {
		pos.y = height - pos.y;
		position = Vector2(pos.x, pos.y);
	}

}


