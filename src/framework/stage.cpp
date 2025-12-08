#include "stage.h"
#include "game/game.h"
#include "framework/input.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include <GL/glew.h>
#include <iostream>
#include "game/world.h"
#include "game/player.h"
#include "framework/audio.h"

// ---------------------------------------------------------
// MENU STAGE IMPLEMENTATION
// ---------------------------------------------------------

MenuStage::MenuStage() {
    Material bg_mat;
    bg_mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    bg_mat.diffuse = Texture::Get("data/menustage.png");
    background = new EntityUI({ 400, 300 }, { 800, 600 }, bg_mat, UI_BUTTON_UNDEFINED, "menu_bg");

    Material play_mat;
    play_mat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    play_mat.diffuse = Texture::Get("data/play_bottom.png");

    play_button = new EntityUI({ 400, 500 }, { 300, 75 }, play_mat, UI_BUTTON_PLAY, "play_btn");

    ui_elements.push_back(background);
    ui_elements.push_back(play_button);
}

void MenuStage::onEnter() {
    std::cout << "[MenuStage] Entering menu...\n";

    //Audio::Init();
    //Audio::Play("data/audios/c418.wav", 0.25f, 0);

    init2DCamera(Game::instance->window_width, Game::instance->window_height);
}

void MenuStage::update(double dt) {
    for (auto ui : ui_elements)
        ui->update((float)dt);
}

void MenuStage::render(Camera* world_camera) {
    glDisable(GL_DEPTH_TEST);


    for (auto ui : ui_elements)
        ui->render(&camera2D);

    glEnable(GL_DEPTH_TEST);
}

// ---------------------------------------------------------
// PLAY STAGE IMPLEMENTATION
// ---------------------------------------------------------

PlayStage::PlayStage() {

}

// En stage.cpp

void PlayStage::onEnter() {
    std::cout << "[PlayStage] Start!\n";

    Audio::Init();
    Audio::Play("data/audios/HowItsDone.wav", 0.20f, 0);
    
    if (!World::instance) {
        World::instance = new World();
        World::instance->init();
    }
    world = World::instance;

    init2DCamera(Game::instance->window_width, Game::instance->window_height);

    landing_platform = nullptr;
    
    
    world->player->animator.playAnimation("data/animations/fall.skanim");

    // Si reseteamos partida, restaurar valores
    if (world->player) {
        world->hearts = 3;
        world->coins = 0;
        world->player->has_jetpack = false;
        world->player->is_jetpack_active = false;
        // Resetear posición jugador arriba
        world->player->model.setTranslation(0, 10, 0);
        world->player->velocity = Vector3(0, 0, 0);
    }

    if (world->myscene) {
        // Recorremos TODOS los hijos por si hay duplicados
        for (auto child : world->myscene->children) {
           
            // Buscamos por nombre (cualquiera que contenga 'blue')
            if (child->name.find("platform_6x6x1_blue") != std::string::npos) {
                

                EntityMesh* plat = (EntityMesh*)child;
                //plat->culling = true;
                landing_platform = plat; // Guardamos referencia para la lógica de Win

                Vector3 currentPos = plat->model.getTranslation();
                plat->model.setTranslation(currentPos.x, -2500.0f, currentPos.z);

                // Usamos la ruta que subiste:
                Texture* tex = Texture::Get("data/scene/platform_6x6x1_blue.014/platformer_texture.png");
                if (tex) {
                    plat->material.diffuse = tex;
                    plat->material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
                }

                // Esto es vital: si es estático, a veces hay que forzar update
                EntityCollider* col = dynamic_cast<EntityCollider*>(plat);
                if (col) {
                    col->model.setTranslation(currentPos.x, -2500.0f, currentPos.z + 750);
                    col->model.scale(1, 1, 1);
                }

                std::cout << "PLATAFORMA ARREGLADA Y MOVIDA A -2500: " << child->name << std::endl;
            }
        }
    }

    if (!landing_platform) {
        std::cout << "ERROR CRITICO: No encuentro la plataforma azul en la escena." << std::endl;
    }
}
void PlayStage::update(double dt) {
    animation_timer.update(dt);
    
    World* world = World::instance;
    if (!world || !world->player) return;

    Player* p = world->player;

    // Updates generales
    world->update(dt);
    for (auto ui : hud_elements) ui->update((float)dt);

    if (world->hearts <= 0) {
        std::cout << "GAME OVER" << std::endl;
        Game::instance->setStage(LOSESTAGE);
        return;
    }

    if (p->has_jetpack && !p->is_jetpack_active) {
        if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
            p->activateJetpack();
        }
    }

    if (landing_platform) {
        Vector3 pPos = p->model.getTranslation();
        Vector3 platPos = landing_platform->model.getTranslation();

        // Distancia total al centro de la plataforma
        float distTotal = pPos.distance(platPos);

        if (distTotal < 50.0f) {
            if (p->is_jetpack_active) {
                std::cout << "WIN!" << std::endl;
                Game::instance->setStage(WINSTAGE);
            }
            else {
                std::cout << "LOSE: Crash!" << std::endl;
                Game::instance->setStage(LOSESTAGE);
            }
        }

        if (pPos.y < (platPos.y - 100.0f)) {
            std::cout << "LOSE: Void!" << std::endl;
            Game::instance->setStage(LOSESTAGE);
        }
    }

    /*std::cout << "TIMER:" << animation_timer.time_left << std::endl;

    if (animation_timer.time_left <= 5.0f && animation_changed==false) {
        world->player->animator.playAnimation("data/animations/fly.skanim");
        animation_changed = true;
    }*/
}

void PlayStage::render(Camera* camera) {
    if (!world) return;

    world->render();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    for (auto ui : hud_elements) ui->render(&camera2D);

    if (world) {
        float screenW = (float)Game::instance->window_width;
        float screenH = (float)Game::instance->window_height;

        drawText(20, 40, "HEARTS: " + std::to_string(world->hearts), Vector3(1, 0, 0), 3.0f);
        drawText(20, 80, "COINS: " + std::to_string(world->coins), Vector3(1, 1, 0), 3.0f);

        Player* p = world->player;
        if (p) {
            if (p->has_jetpack && !p->is_jetpack_active) {
                std::string msg = "PRESS SPACE TO FLY!";
                float textX = screenW * 0.5f - 180.0f;
                float textY = screenH - 80.0f; // 80 pixeles desde abajo

                drawText(textX, textY, msg, Vector3(0, 1, 0), 3.0f);
            }

            if (p->is_jetpack_active) {
                std::string msg = "JETPACK ACTIVE";
                float textX = screenW * 0.5f - 140.0f;
                float textY = screenH - 80.0f;

                drawText(textX, textY, msg, Vector3(0, 1, 1), 3.0f);
            }

            if (landing_platform) {
                float distY = p->model.getTranslation().y - landing_platform->model.getTranslation().y;
                if (distY < 800.0f && distY > 0.0f) {
                    drawText(screenW * 0.5f - 200.0f, screenH * 0.3f, "!! LANDING ZONE !!", Vector3(1, 0, 0), 4.0f);
                }
            }
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
// ---------------------------------------------------------
// WIN STAGE IMPLEMENTATION
// ---------------------------------------------------------
void WinStage::onEnter() {
    std::cout << "--- WIN STAGE ---" << std::endl;
    // Aquí podrías reproducir música de victoria
}

void WinStage::update(double dt) {
    // Si pulsamos ENTER, volvemos al menú
    if (Input::wasKeyPressed(SDL_SCANCODE_RETURN)) {
        // Opcional: Reiniciar el mundo aquí si es necesario
        // World::instance->init(); 
        Game::instance->setStage(MENUSTAGE);
    }
}

void WinStage::render(Camera* camera) {
    // Fondo simple (puedes poner una imagen si quieres)

    float centerX = Game::instance->window_width * 0.5f;
    float centerY = Game::instance->window_height * 0.5f;

    // Texto Grande
    drawText(centerX - 200, centerY - 50, "MISSION ACCOMPLISHED!", Vector3(0, 1, 0), 4.0f);

    // Instrucción
    drawText(centerX - 200, centerY + 50, "Press ENTER to return to Menu", Vector3(1, 1, 1), 2.0f);
}

// ---------------------------------------------------------
// LOSE STAGE IMPLEMENTATION
// ---------------------------------------------------------
void LoseStage::onEnter() {
    std::cout << "--- LOSE STAGE ---" << std::endl;
}

void LoseStage::update(double dt) {
    if (Input::wasKeyPressed(SDL_SCANCODE_RETURN)) {
        Game::instance->setStage(MENUSTAGE);
    }
}

void LoseStage::render(Camera* camera) {
    float centerX = Game::instance->window_width * 0.5f;
    float centerY = Game::instance->window_height * 0.5f;

    drawText(centerX - 120, centerY - 50, "GAME OVER", Vector3(1, 0, 0), 5.0f);

    drawText(centerX - 200, centerY + 50, "Press ENTER to return to Menu", Vector3(1, 1, 1), 2.0f);
}
