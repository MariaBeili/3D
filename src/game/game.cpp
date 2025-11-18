#include "game.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "framework/scene_parser.h"
#include "framework/entities/entity_mesh.h"
#include <cmath>
#include "world.h"

World* world = World::getInstance();
Game* Game::instance = NULL;
//some globals
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;



//Entity* myscene = nullptr;



//EntityMesh* skybox = nullptr;
EntityMesh* i_object = nullptr;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	Game::instance = this;
	std::cout << "Game instance set" << std::endl;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;


	mouse_locked = false;
	// OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	// Initialize the world (root, player, skybox, etc.)
	// 
	//world = new World();
	

	World* world = World::getInstance();

	world->init(); // Call the separate initialization


	
	//world->init();
		
	// Hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse




}

//what to do when the image has to be draw
void Game::render(void)
{
	// Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	world->render();
	//World::getInstance()->addEntity(world->myscene->children);

	// Render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	// Swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}



void Game::update(double seconds_elapsed)
{
	world->update(seconds_elapsed);

}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
		case SDLK_F2: render_debug = !render_debug; break;
		case SDLK_TAB:
		{
			world->free_camera = !world->free_camera;
			Game::instance->setMouseLocked(world->free_camera);
			break;
		}
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(mouse_locked));
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{

}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1f : 0.9f;
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	world->camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

void Game::setMouseLocked(bool must_lock)
{
	SDL_ShowCursor(!must_lock);

	SDL_SetRelativeMouseMode((SDL_bool)must_lock);

	mouse_locked = must_lock;
}


void Game::setStage(eStage stage_id)
{
	Stage* next_stage = stages[stage_id];
	if (!next_stage)
	{
		return;
	}
	//Leave the current stage
	if (current_stage)
	{
		current_stage->onExit();
	}
	//Enter the new one
	next_stage->onEnter(); //current_stage
	//Update current stage
	current_stage = next_stage;
}