#include "player.h"
#include <iostream>
#include "framework/input.h"
#include "framework/camera.h"
#include "world.h"
#include "game.h"
#include "graphics/mesh.h"
#include "framework/collision.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "game/scene_parser.h"
#include "framework/collision.h"

Player* Player::instance = nullptr;

Player::Player(Mesh* mesh, const Material& material, const std::string& name)
{

    if (!mesh) {
		std::cout << "WARNING: Could not load player mesh, creating fallback..." << std::endl;
    }
    else {
        this->mesh = mesh;
    }

    if (material.shader == nullptr) {
		std::cout << "WARNING: No material shader provided, creating default material..." << std::endl;
    }
    else {
        this->material = material;
    }

    this->name = name;
    this->model.scale(2.0f, 2.0f, 2.0f);
}

#define RENDER_DEBUG
void Player::render(Camera* camera)
{
    Shader* shader = material.shader;
    if (!shader) return;




    if (!camera) {
        return;
    }




    EntityMesh::render(camera);

    /*#ifdef RENDER_DEBUG
        // Render sphere using a basic.vs + flat.fs shader
        shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
        Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");

        shader->enable();

        Matrix44 m;
        m.setTranslation(getGlobalMatrix().getTranslation());
        //m.translate(0.0f, World::instance->height, 0.0f);
        m.translate(0.0f, 0.0f, 1.0f);

        m.scale(World::instance->sphere_radius, World::instance->sphere_radius, World::instance->sphere_radius);

        shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
        shader->setUniform("u_model", m);

        mesh->render(GL_LINES);

        shader->disable();
    #endif RENDER_DEBUG*/
}

void Player::update(float delta_time)
{

    if (!World::instance) {
        return;
    }

    Vector3 position = model.getTranslation();
    velocity = { 0.0f, 0.0f, 0.0f };

    Camera* camera = World::instance->camera;
    if (!camera) {
        return;
    }


    Vector3 move_dir = getMovementDirection();
    float current_speed = speed;
    if (Input::isKeyPressed(SDL_SCANCODE_LALT)) current_speed = walk_speed;
    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) current_speed = run_speed;

    move_dir *= current_speed;
    velocity += move_dir;


    float foot_offset = 0.1f;
    Vector3 floor_position;

    /*if (isGrounded(position, foot_offset, floor_position)) {
        //std::cout << "Player is grounded at: " << floor_position.x << ", " << floor_position.y << ", " << floor_position.z << std::endl;
        if (position.y < (floor_position.y + foot_offset)) {
            position.y = floor_position.y;
        }
    }
    else {
        //std::cout << "Player is NOT grounded, applying gravity" << std::endl;
        position -= Vector3(0.0f, 10.0f * delta_time, 0.0f); // Apply gravity if not grounded
    }*/

    
    Vector3 new_position = position + velocity * delta_time;
    new_position -= Vector3(0.0f, 20.0f * delta_time, -5.0f * delta_time); // Apply gravity if not grounded
    //std::cout << "Checking if can move to new position..." << std::endl;
    if (canMove(new_position)) {
        position = new_position;
        
        //std::cout << "Movement allowed to new position" << std::endl;
    }
    else {
        position.x = new_position.x;
        position.z = new_position.z;
        //std::cout << "Movement blocked to new position" << std::endl;
    }

    model.setTranslation(position);

    //if (velocity.length() > 0.0f) {
        //<last_camera_yaw = lerpAngleRad(last_camera_yaw, camera->yaw, delta_time * 20.0f);
    //}
    //model.rotate(last_camera_yaw, Vector3::UP);
    model.rotate(-PI / 3, Vector3(1, 0, 0));
    model.scale(2.0);
    //angle_y = atan2f(move_dir.x, move_dir.z);
    //model.rotate(angle_y, Vector3::UP);

    //model.rotate(camera->yaw, Vector3::UP);
    //std::cout << "Last_camera-yaw:"<< last_camera_yaw << std::endl;


    //std::cout << "Calling EntityMesh::update..." << std::endl;
    //isAnimated = true;
    //animator.playAnimation("data/animations/fall.skanim");
    EntityMesh::update(delta_time);
    //std::cout << "Player::update() completed" << std::endl;
    /*if (World::instance->last_collision_entity) {
        EntityCollider* e = dynamic_cast<EntityCollider*>(World::instance->last_collision_entity);
        if (e) {
            if (e->name == "coin") {
                // Pick up coin
                World::instance->coins++;
                std::cout << "Picked coin! Coins = " << World::instance->coins << std::endl;

                // Remove from spawner active objects safely
                auto& active = World::instance->spawner->active_objects;
                active.erase(std::remove(active.begin(), active.end(), e), active.end());

                // Mark for deletion
                World::instance->destroyEntity(e);

                // Reset collision
                World::instance->last_collision_entity = nullptr;
            }
            else if (e->name == "box") {
                // Hit box
                World::instance->hearts--;
                std::cout << "Hit box! Hearts = " << World::instance->hearts << std::endl;

                // Knockback player
                float knockback_dist = 5.0f;
                Vector3 rand_dir = Vector3((float)(rand() % 200 - 100) / 100.f, 0, (float)(rand() % 200 - 100) / 100.f).normalize();
                model.setTranslation(model.getTranslation() + rand_dir * knockback_dist);

                // Optionally remove the box
                auto& active = World::instance->spawner->active_objects;
                active.erase(std::remove(active.begin(), active.end(), e), active.end());
                World::instance->destroyEntity(e);

                // Reset collision
                World::instance->last_collision_entity = nullptr;
            }
        }
    }*/
}

const Vector3& Player::getMovementDirection()
{
    //std::cout << "Player::getMovementDirection() called" << std::endl;

    if (!World::instance) {
        static Vector3 zero_vec(0, 0, 0);
        return zero_vec;
    }

    Vector3 position = model.getTranslation();
    velocity = { 0.0f, 0.0f, 0.0f };

    Vector3 front, right, move_dir;
    Camera* camera = World::instance->camera;

    if (!camera) {
        static Vector3 zero_vec(0, 0, 0);
        return zero_vec;
    }

    Matrix44 mYaw;
    mYaw.setRotation(camera->yaw, Vector3(0, 1, 0));
    front = mYaw.frontVector();
    right = -mYaw.rightVector();

    //std::cout << "Camera yaw: " << camera->yaw << ", Front: " << front.x << ", " << front.y << ", " << front.z << std::endl;

    if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) move_dir += front;
    if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) move_dir -= front;
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) move_dir -= right;
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) move_dir += right;

    Vector3 result = move_dir.normalize();


    return result;
}

bool Player::isGrounded(const Vector3& new_position, float max_ray_dist, Vector3& col_point)
{


    if (!World::instance) {

        return false;
    }

    if (!World::instance->myscene) {

        return false;
    }

    sCollisionData data;
    Vector3 from = new_position + Vector3(0, World::instance->height, 0);
    Vector3 to = new_position - Vector3(0, max_ray_dist, 0);
    Vector3 dir = (to - from);

    //std::cout << "Testing ray from: " << from.x << ", " << from.y << ", " << from.z
        //<< " to: " << to.x << ", " << to.y << ", " << to.z << std::endl;

    bool collision_result = Collision::TestSceneRay(World::instance->myscene->children, from, dir.normalize(), data,
        eCollisionFilter::ALL, true, dir.length());

    if (collision_result) {
        col_point = data.col_point;

        return true;
    }


    return false;
}

bool Player::canMove(const Vector3& new_position)
{


    //std::cout << "Player::canMove() called. New position: " << new_position.x << ", " << new_position.y << ", " << new_position.z << std::endl;

    if (!World::instance || !World::instance->myscene)
        return true;

    World* world = World::instance;
    //world->last_collision_entity = nullptr;
    bool collided = false;

    //std::cout << "Checking collisions with " << world->myscene->children.size() << " scene children" << std::endl;

    //Vector3 sphere_center = new_position + Vector3(0.0f, World::instance->height, 0.0f);

    Vector3 sphere_center = new_position + Vector3(0.0f, 0.0f, 1.0f);
    float radius = world->sphere_radius;


    for (auto e : world->myscene->children)
    {
        if (!e) continue;

        std::vector<sCollisionData> col;
        bool hit = Collision::TestEntitySphere(
            e, radius, sphere_center, col, eCollisionFilter::SCENARIO
        );

        if (hit)
        {
            collided = true;
            world->last_collision_entity = e;
            return false;
        }
    }
    return !collided;
}
/*
    if (world->spawner)
    {
        for (EntityCollider* obj : world->spawner->active_objects) {
            if (!obj || obj->picked) continue; // skip already picked
            std::vector<sCollisionData> col;
            bool hit = Collision::TestEntitySphere(obj, radius, sphere_center, col, eCollisionFilter::ALL);

            if (hit) {
                world->last_collision_entity = obj;

                if (obj->name == "coin") {
                    obj->picked = true;      // mark as collected
                    world->coins += 1;
                    std::cout << "Picked coin! Coins = " << world->coins << std::endl;

                    // start destroy timer instead of deleting now
                    obj->destroy_timer = 0.1f;
                    return true; // allow movement through coin
                }

                if (obj->name == "box") {
                    collided = true;
                    // start destroy timer if you want
                    obj->destroy_timer = 0.1f;
                    return false; // block movement
                }

                collided = true;
                return false;
            }
        }

    }

    return !collided;*/



void Player::activateJetpack() {
    if (!has_jetpack || is_jetpack_active) return;

    is_jetpack_active = true;

    Mesh* m = Mesh::Get("data/jetpack/jetpack.obj");
    Texture* t = Texture::Get("data/jetpack/texture.png");

    Material mat;
    mat.diffuse = t;
    mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

    EntityMesh* jetpackInfo = new EntityMesh(m, mat, "JetpackVisual");

    Matrix44 transform;
    transform.setTranslation(0.0f, 0.6f, -0.25f);
    transform.scale(0.3f, 0.3f, 0.3f);

    jetpackInfo->model = transform;

    this->addChild(jetpackInfo);

    std::cout << "Jetpack visual activado!" << std::endl;
}