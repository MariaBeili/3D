#pragma once

#include "input.h"
#include "graphics/shader.h"
#include "game/world.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"


//class World;

enum eStage {
	MENUSTAGE,
	PLAYSTAGE,
	WINSTAGE,
	LOSESTAGE
};

class Stage {
public:

	Mesh* mesh_cube = NULL;
	Texture* texture_cube = NULL;
	Shader* shader_cube = NULL;
	Matrix44 m_cube;

	virtual void onEnter() {};
	virtual void onExit() {};

	virtual void render(Camera* camera) {}; // Empty body
	virtual void update(double seconds_elapsed) {}; // Empty body
};



class MenuStage : public Stage {
public:

	MenuStage();


	void render();
	void update(double seconds_elapsed);
	virtual void onEnter();
	virtual void onExit();


};


class PlayStage : public Stage {
public:

	PlayStage();

	virtual void onEnter();
	virtual void onExit();

	void render(Camera* camera);
	void update(double seconds_elapsed);
};

class EndStage : public Stage {
public:

	//EndStage();

	virtual void onEnter();
	virtual void onExit();

	void render(Camera* camera);
	void update(double seconds_elapsed);
};
