//Updated Player class implementation
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



    // If no mesh provided, create a default one
    if (!mesh) {

        //this->mesh = Mesh::Get("data/meshes/character.obj"); // or any default mesh

        //if (!this->mesh) {
            // Create a simple cube as fallback
            //this->mesh = new Mesh();
            //this->mesh->createCube();

        //}
    }
    else {
        this->mesh = mesh;
    }

    // If no material provided, create a default one
    if (material.shader == nullptr) {
        //this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
        //this->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/character.fs");

        //this->material.diffuse = Texture::Get("data/textures/player_texture.png");
        //this->material.diffuse = Texture::Get("data/character/character.mtl");
        //this->material.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f); // Red color
    }
    else {
        this->material = material;
    }

    this->name = name;

    // Set initial position
    //model.setTranslation(0.0f, 0.0f, 0.0f);
    
    //previous_position = model.getTranslation();


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

#ifdef RENDER_DEBUG
    // Render sphere using a basic.vs + flat.fs shader
    shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
    Mesh* mesh = Mesh::Get("data/meshes/sphere.obj");

    shader->enable();

    Matrix44 m;
    m.setTranslation(getGlobalMatrix().getTranslation());
    //m.translate(0.0f, World::instance->height, 0.0f);
    m.translate(0.0f, 0.0f, 1.5f);
    
    m.scale(World::instance->sphere_radius, World::instance->sphere_radius, World::instance->sphere_radius);

    shader->setUniform("u_color", Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
    shader->setUniform("u_model", m);

    mesh->render(GL_LINES);

    shader->disable();
#endif RENDER_DEBUG
}

void Player::update(float delta_time)
{

    // Check World instance
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


    //Manage floor collisions
    float foot_offset = 0.1f; // Small offset to avoid getting stuck in the ground
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

    // Check collisions with the next position
    
    Vector3 new_position = position + velocity * delta_time;
    new_position -= Vector3(0.0f, 10.0f * delta_time, 0.0f); // Apply gravity if not grounded
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

    //Update model position
    model.setTranslation(position);

    // Rotate player model based on camera yaw
    //if (velocity.length() > 0.0f) {
        //<last_camera_yaw = lerpAngleRad(last_camera_yaw, camera->yaw, delta_time * 20.0f);
    //}
    //model.rotate(last_camera_yaw, Vector3::UP);
    model.rotate(-PI/2, Vector3(1, 0, 0));

    //angle_y = atan2f(move_dir.x, move_dir.z);
    //model.rotate(angle_y, Vector3::UP);

    //model.rotate(camera->yaw, Vector3::UP);
    //std::cout << "Last_camera-yaw:"<< last_camera_yaw << std::endl;

    
    //std::cout << "Calling EntityMesh::update..." << std::endl;
    EntityMesh::update(delta_time);
    //std::cout << "Player::update() completed" << std::endl;
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

    // Get move direction based on input and current camera angles
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

    // Get final movement direction
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

    if (!World::instance) {
        std::cout << "CRITICAL ERROR: World::instance is null in canMove!" << std::endl;
        return false;
    }

    if (!World::instance->myscene) {
        std::cout << "ERROR: myscene is null in canMove!" << std::endl;
        return true; // Allow movement if no scene
    }

    World* world = World::instance;
    bool collided = false;

    //std::cout << "Checking collisions with " << world->myscene->children.size() << " scene children" << std::endl;

    // center of the player's collision sphere (world space)
    //Vector3 sphere_center = new_position + Vector3(0.0f, World::instance->height, 0.0f);

    Vector3 sphere_center = new_position + Vector3(0.0f, 0.0f, 1.5f);



    for (auto e : world->myscene->children)
    {
        if (!e) {
            std::cout << "WARNING: Null entity in scene children" << std::endl;
            continue;
        }

        // Use a vector for collisions (signature expected by TestEntitySphere)
        std::vector<sCollisionData> collisions;

        bool entity_collision = Collision::TestEntitySphere(e, World::instance->sphere_radius, sphere_center, collisions, eCollisionFilter::SCENARIO);
        if (entity_collision) {
            //std::cout << "Collision detected with entity: " << e->name << std::endl;
            collided = true;
            // opcional: puedes 'break' aquí si quieres bloquear en la primera colisión
            // break;
        }
    }

    //std::cout << "canMove result: " << (collided ? "BLOCKED" : "ALLOWED") << std::endl;
    return !collided;

}