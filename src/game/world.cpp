#include "world.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "game/scene_parser.h"
#include "framework/entities/entity_mesh.h"
#include "game/player.h"
#include "framework/camera.h"
#include <iostream>
#include "game/game.h"
#include "framework/collision.h"
#include "framework/entities/entity_collider.h"
#include "framework/audio.h"


World* World::instance = nullptr;

World::World() {
	camera = nullptr;
	myscene = nullptr;
	skybox = nullptr;
	player = nullptr;
	free_camera = true; // Default to free camera until player is ready

	camera_speed = 2.0f;
	mouse_speed = 10.0f;
	
 }
void World::init() {

	Audio::Init();
	Audio::Play("data/audios/HowItsDone.wav", 0.25f, 0);

	if (!Game::instance) {

		return;
	}

	Game* game = Game::instance;



	camera = new Camera();
	camera->lookAt(Vector3(0.f, 1.0f, 1.0f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(70.f, game->window_width / (float)game->window_height, 0.1f, 10000.f);

	// Rest of your initialization code...
	//std::cout << "Creating scene root..." << std::endl;
	myscene = new Entity();

	//std::cout << "Parsing scene file..." << std::endl;
	SceneParser parser;
	parser.parse("data/myscene.scene", myscene);

	// Create object spawner
	spawner = new ObjectSpawner();
	if (spawner)
		spawner->init();

	// Crear un nodo temporal para el personaje
	Entity* character_scene = new Entity();

	// Create player with proper mesh and material
	Mesh* player_mesh = Mesh::Get("data/fox/fox.obj"); // or any character mesh
	if (!player_mesh) {
		std::cout << "WARNING: Could not load player mesh, creating fallback..." << std::endl;
		player_mesh = new Mesh();
		player_mesh->createCube();
		player_mesh->name = "player_cube";
	}

	Material player_material;
	player_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	player_material.diffuse = Texture::Get("data/fox/Diffuse.png");



	player = new Player(player_mesh, player_material, "Player");

	// add player to scene so scene.render() will draw it:
	myscene->addChild(player);


	// Set initial player position
	//player->model.setTranslation(0.0f, 2.0f, 0.0f);
	// después de crear 'player' o asignar World::instance->player:
	player->model.setTranslation(0.0f, 250.0f, 0.0f);
	//player->previous_position = World::instance->player->model.getTranslation();
	player->velocity = Vector3(0.0f, 0.0f, 0.0f); // opcional: reset velocidad

	/*
	// JetPack
	Mesh* jet_mesh = Mesh::Get("data/jetpack/jetpack.obj");
	if (!jet_mesh) {
		std::cout << "WARNING: Could not load player mesh, creating fallback..." << std::endl;
		jet_mesh = new Mesh();
		jet_mesh->createCube();
		jet_mesh->name = "player_cube";
	}

	Material jet_material;
	jet_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	jet_material.diffuse = Texture::Get("data/jetpack/texture.png");

	jetpack = new EntityMesh(jet_mesh, jet_material, "JetPack");

	myscene->addChild(jetpack);

	jetpack->model.setTranslation(5.0f, 250.0f, 0.0f);
	*/
	// Skybox creation...
	Texture* cube_texture = new Texture();
	cube_texture->loadCubemap("landscape", {
		"data/textures/skybox/px.png",
		"data/textures/skybox/nx.png",
		"data/textures/skybox/ny.png",
		"data/textures/skybox/py.png",
		"data/textures/skybox/pz.png",
		"data/textures/skybox/nz.png"
		});

	Material cubemap_material;
	cubemap_material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cubemap.fs");
	cubemap_material.diffuse = cube_texture;

	skybox = new EntityMesh(Mesh::Get("data/meshes/cubemap.ASE"), cubemap_material);
	skybox->culling = false;

	//std::cout << "World::initialize() completed." << std::endl;
}

void World::render() {

	Camera::current = camera;
	// Set the camera as default
	if (!camera) {

		return;
	}


	camera->enable();

	/// Render skybox ONLY if it exists
	if (skybox) {

		glEnable(GL_DEPTH_TEST);
		skybox->render(camera);
	}

	// Draw the floor grid

	drawGrid();

	// Render players ONLY if player exists
	//if (player) {
		//player->render(camera);
	//}


	// Render all the scene
	if (myscene) {

		myscene->render(camera);
	}



}
void World::update(double delta_time) {
	if (this == nullptr) {

		return;
	}
	// Check critical pointers first
	if (!camera) {
		return;
	}

	if (spawner && player)
		spawner->update((float)delta_time, player->model.getTranslation());



	if (free_camera_just_enabled)
	{
		Vector3 pos = camera->eye;

		// Look forward in +Z, Y up (standard freecam orientation)
		Vector3 target = pos + Vector3(0, 0, 1);

		camera->lookAt(pos, target, Vector3(0, 1, 0));
		free_camera_just_enabled = false;
	}


	if (free_camera)
	{
			

		//std::cout << "Free camera mode active" << std::endl;
		float speed = delta_time * camera_speed;
		//std::cout << "Camera speed: " << speed << std::endl;

		//Mouse input to rotate the cam
		if (Input::isMousePressed(SDL_BUTTON_LEFT) || Game::instance->mouse_locked) {
			//std::cout << "Rotating camera with mouse input" << std::endl;
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
		}

		// Async input to move the camera around
		//std::cout << "Checking keyboard input for camera movement..." << std::endl;
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) {
			//std::cout << "Moving camera forward" << std::endl;
			camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		}
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) {
			//std::cout << "Moving camera backward" << std::endl;
			camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		}
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) {
			//std::cout << "Moving camera left" << std::endl;
			camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		}
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) {
			//std::cout << "Moving camera right" << std::endl;
			camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		}
	}
	else
	{
		//std::cout << "Player camera mode active" << std::endl;

	// Check player pointer before using it
		if (!player) {
			std::cout << "CRITICAL ERROR: player is null in player camera mode!" << std::endl;
			return;
		}
		//std::cout << "Player pointer is valid" << std::endl;

		{
			float height = 10.0f;  // How high the camera sits
			float offset = 0.0f;   // If you want slight forward offset use >0

			Vector3 player_position = player->model.getTranslation() + Vector3(0, height, 0);
			//std::cout << "Player position: " << player_position.x << ", " << player_position.y << ", " << player_position.z << std::endl;
			Vector3 eye = Vector3(0, player_position.y + height, 0);
			Vector3 center = Vector3(0, player_position.y, 0);

			// VERY IMPORTANT:
			// The camera looks DOWN, so the "up vector" must point forward
			camera->lookAt(eye, center, Vector3(0, 0, 1));

			// No rotation, no pitch, no mouse control.

    }

		if (!myscene) {
			std::cout << "ERROR: myscene is null in update!" << std::endl;
			return;
		}
		//std::cout << "Updating scene..." << std::endl;
		if (myscene) myscene->update(delta_time);
		//std::cout << "Scene update completed" << std::endl;

		if (!player) {
			std::cout << "ERROR: Player is null before player update!" << std::endl;
			return;
		}

		//std::cout << "Updating player..." << std::endl;
		player->update((float)delta_time);
		//std::cout << "Player update completed" << std::endl;
	}

	//Quitar entities que queden porque siempre se el padre
	if (!entities_to_destroy.empty()) {
		// debug: quantitats
		std::cout << "[WORLD] Destroying " << entities_to_destroy.size() << " entities\n";
	}

	for (Entity* e : entities_to_destroy) {
		if (!e) continue;
		// detach from parent if still attached (removeChild és segura)
		if (e->parent) {
			e->parent->removeChild(e);
		}
		// Clear any child->parent pointers for the entity children if needed
		// (optionally traverse e->children and null their parent pointers)
		// Delete entity safely
		delete e;
	}

	entities_to_destroy.clear();
	entities_to_destroy_set.clear();

	
	/*
	//Vacias papelera después de acabar el juego
	entities_to_destroy.clear();
	// Mover skybox con el jugador
	if (skybox && player) {
		Vector3 player_pos = player->model.getTranslation();
		skybox->model.setTranslation(player_pos);
	}
	*/
}

void World::addEntity(Entity* entity){
		myscene->addChild(entity);
}

void World::destroyEntity(Entity* entity){
	entities_to_destroy.push_back(entity);
}

