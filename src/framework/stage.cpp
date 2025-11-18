#include "stage.h"
#include "game/game.h"
#include "framework/input.h"
#include "game/world.h"



// ---------------------------------------------------------------------
// MENU STAGE
// ---------------------------------------------------------------------
MenuStage::MenuStage() {

}

void MenuStage::render() {
	World::instance->render();

	if (shader_cube)
	{
		// Enable shader
		shader_cube->enable();

		// Upload uniforms
		shader_cube->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader_cube->setUniform("u_viewprojection", World::instance->camera->viewprojection_matrix);
		shader_cube->setUniform("u_texture", texture_cube, 0);
		shader_cube->setUniform("u_model", m_cube);
		shader_cube->setUniform("u_time", time);


		// Do the draw call
		mesh_cube->render(GL_TRIANGLES);

		// Disable shader
		shader_cube->disable();
	}


}

void MenuStage::update(double seconds_elapsed) {

	World::instance->update(seconds_elapsed);
}

void MenuStage::onEnter()
{
    std::cout << "Entering MenuStage\n";
    texture_cube = Texture::Get("data/textures/StandardCubeMap.tga");

    mesh_cube = Mesh::Get("data/meshes/cubemap.obj");

    shader_cube = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

}

void MenuStage::onExit() 
{
    std::cout << "Exiting MenuStage\n";
}

// -------------------------------------------------------------------------
// PLAY STAGE
// -------------------------------------------------------------------------
PlayStage::PlayStage() {


}

void PlayStage::onEnter() {
    std::cout << "Entering PlayStage\n";

}

void PlayStage::onExit() {
    std::cout << "Leaving PlayStage\n";


}

void PlayStage::update(double seconds_elapsed) {


}


void PlayStage::render(Camera* camera) {


}


// ----------------------------------------------------------------------
// END STAGE
// ----------------------------------------------------------------------
/*EndStage::EndStage() {


}*/

void EndStage::render(Camera* camera) {


}

void EndStage::update(double seconds_elapsed) {


}

void EndStage::onEnter()
{
    std::cout << "Entering EndStage\n";

}

void EndStage::onExit()
{
    std::cout << "Leaving EndStage\n";
}
