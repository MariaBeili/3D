#pragma once

#include "framework/entities/entity.h"

#include "graphics/material.h"

#include "framework/entities/entity_mesh.h"

#include <map>

class SceneParser {

	std::map<std::string, sRenderData> meshes_to_load;

public:
	bool parse(const char* filename, Entity* root);
};