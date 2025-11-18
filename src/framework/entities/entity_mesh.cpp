#include "entity_mesh.h"
#include "graphics/shader.h"
#include "framework/camera.h"
#include "graphics/texture.h"
#include "framework/includes.h"



EntityMesh::EntityMesh(Mesh* m, const Material& mat, const std::string& name)
{
    this->mesh = m;
    this->material = mat;
    this->name = name;
}


void EntityMesh::render(Camera* camera)
{
    if (!mesh) {
        Entity::render(camera);
        return;
    }



    if (!material.shader) {
        if (isInstance && !instanceModels.empty()) {
            material.shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
        }
        else {
            material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
        }
    }
    
    Shader* shader = material.shader;
    if (!shader) return;

    // Set OpenGL flags
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


    // Enable shader and pass uniforms 
    shader->enable();

    shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
    shader->setUniform("u_camera_position", camera->eye);

    if (material.diffuse) //Texturas para los objetos que no tengan mtl
    {
		material.diffuse->bind();
        //std::cout << "[Debug] Binding texture: " << material.diffuse->filename << std::endl;
        shader->setTexture("u_texture", material.diffuse, 0);
        //std::cout << "[Warning] material.diffuse is NULL for entity: " << name << std::endl;
        shader->setUniform("u_color", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else
    {
        // Fallback to blue color if no texture
        shader->setUniform("u_color", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        //std::cout << "[Warning] No texture for entity: " << name << std::endl;
    }

    if (isInstance && !instanceModels.empty()) {
        // Use instanced rendering
        mesh->renderInstanced(GL_TRIANGLES, instanceModels.data(), instanceModels.size());
    }
    else {
        /*
        //ESTA OPTIMIZACIÓN NO SIRVE SI HACEMOS INSTANCING, con instanci hay que hacerlo con cada mdel 
        float distance = 10.0f;
        const Matrix44& globalMatrix = getGlobalMatrix();
        BoundingBox box = transformBoundingBox(
            globalMatrix, //con model lo puedep usar porquè no tengo padre
            mesh->box
        );

        Vector3 center = box.center;

        // Discard objects far away from the camera
        if (camera->eye.distance(center) > distance)
            if (culling == true)
                return;

        //FRUSTUM

        // Testing sphere: using halfsize length as  
        // radius
        float sphere_radius = box.halfsize.length();

        // Discard objects whose bounding sphere 
        // is not inside the camera frustum
        if (camera->testSphereInFrustum(center,
            sphere_radius) != CLIP_INSIDE)
            if (culling == true)
                return;

        shader->setUniform("u_model", model);

        mesh->render(GL_TRIANGLES);
        */

    }
    shader->setUniform("u_model", model);

    mesh->render(GL_TRIANGLES);


    // Disable shader after finishing rendering
    shader->disable();

    Entity::render(camera);
}

void EntityMesh::update(float delta_time)
{
    Entity::update(delta_time);
}

