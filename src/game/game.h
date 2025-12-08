/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#pragma once

#include "framework/includes.h"
#include "framework/stage.h"

#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/framework.h"
#include "framework/entities/entity.h"


// FORWARD DECLARATIONS
class World;
class Camera;

class Game
{
public:
	static Game* instance;

	SDL_Window* window;
	int window_width;
	int window_height;
	bool must_exit = false;

	//some globals
	long frame;
	float time;
	float elapsed_time;
	int fps;

	bool render_debug = false;

	//some vars
	//Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)
	float mouse_speed = 0.25f;
	//Entity* myscene = nullptr;

	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	void setMouseLocked(bool must_lock);

	

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

	void setStage(eStage new_stage);
	// ---- new stage system ----
	Stage* stages[4] = { nullptr };
	Stage* current_stage = nullptr;
	eStage current_stage_id = MENUSTAGE;

	// global camera for 3D world rendering
	Camera world_camera;

};