#include "entity.h"
#include "framework/camera.h"

#include <algorithm>

void Entity::render(Camera* camera)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->render(camera);
	}
}

void Entity::update(float delta_time)
{
	for (int i = 0; i < children.size(); ++i) {
		children[i]->update(delta_time);
	}
}

void Entity::addChild(Entity* child)
{
	if (child->parent) {
		std::cerr << "Child has already a parent, remove it first!" << std::endl;
		return;
	}

	// Checks if it's already a child
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end()) {
		std::cerr << "Entity is already one of the children!" << std::endl;
		return;
	}

	child->parent = this;
	children.push_back(child);
}

void Entity::removeChild(Entity* child)
{
	// Checks if it's a child
	if (!child) return;
	// find the child in the vector
	auto it = std::find(children.begin(), children.end(), child);
	if (it == children.end()) {
		// already removed / not a child -> nothing to do
		return;
	}
	// detach safely
	child->parent = nullptr;
	children.erase(it);
}

Matrix44 Entity::getGlobalMatrix()
{
	if (parent)
		return model * parent->getGlobalMatrix();
	return model;
}

float Entity::distance(Entity* e)
{
	return model.getTranslation().distance(e->model.getTranslation());
}

/*
void Entity::destroy() {

	s_to_destroy.push_back(this);

	// Propagate destruction
	for (int i = 0; i < children.size(); i++)
		children[i]->destroy();
}
}

// Per frame:

for (int i = 0; i < s_to_destroy.size(); i++)
	delete s_to_destroy[i];
}

s_to_destroy.empty();
*/