#pragma once
#include "framework/entities/entity_mesh.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

// Aquesta classe conté la configuració de cada tipus d’objecte (freqüència, probabilitat, etc.)
struct SpawnableObject {
	std::string name; // Nom de l'objecte
	std::string mesh_path; // Ruta del mesh obj.
	std::string texture_path; // Ruta de la textura
	float spawn_interval; // Interval de temps entre intents d’aparició
	float probability; // Probabilitat d’aparició en cada intent
	float spawn_distance; // Distància davant del jugador per aparèixer
	float spawn_timer = 0.0f; // Temporitzador intern
    Mesh* mesh = nullptr; 
    Material material;

    SpawnableObject(const std::string& n, const std::string& mesh, const std::string& tex,
        float interval, float prob, float distance)
        : name(n), mesh_path(mesh), texture_path(tex),
        spawn_interval(interval), probability(prob), spawn_distance(distance) {

        this->mesh = Mesh::Get(mesh_path.c_str());
        this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
        this->material.diffuse = Texture::Get(texture_path.c_str());
        this->material.color = Vector4(1, 1, 1, 1);
    }
};