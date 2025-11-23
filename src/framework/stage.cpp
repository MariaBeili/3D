#include "stage.h"
#include "game/game.h"
#include <GL/glew.h>
#include "game/world.h"

// ---------------------------------------------------------
// MENU STAGE IMPLEMENTATION
// ---------------------------------------------------------

MenuStage::MenuStage() {
    Material mat;
    //mat.diffuse = Texture::Get("data/textures/button.png");
    mat.diffuse = Texture::Get("data/textures/skybox/px.png");

    ui_elements.push_back(
        new EntityUI({ 400,300 }, { 300,100 }, mat, UI_BUTTON_PLAY, "play_btn")
    );
}

void MenuStage::onEnter() {
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
    Material mat;
    //mat.diffuse = Texture::Get("data/textures/healthbar.png");
    mat.diffuse = Texture::Get("data/textures/skybox/px.png");

    hud_elements.push_back(
        new EntityUI({ 100,50 }, { 200,40 }, mat, UI_BUTTON_UNDEFINED, "healthbar")
    );
}

void PlayStage::onEnter() {
    init2DCamera(Game::instance->window_width, Game::instance->window_height);
}

void PlayStage::update(double dt) {
    if (world)
        world->update(dt);

    for (auto ui : hud_elements)
        ui->update((float)dt);
}

void PlayStage::render(Camera* camera) {
    if (world)
        world->render();

    // HUD on top
    glDisable(GL_DEPTH_TEST);
    for (auto ui : hud_elements)
        ui->render(&camera2D);
    glEnable(GL_DEPTH_TEST);
}

// ---------------------------------------------------------
// END STAGE IMPLEMENTATION
// ---------------------------------------------------------

EndStage::EndStage(bool win_screen) {
    this->win_screen = win_screen;

    Material mat;
    mat.diffuse = Texture::Get(win_screen ?
        //"data/textures/you_win.png" :
        //"data/textures/you_lose.png");
        "data/textures/skybox/px.png" :
        "data/textures/skybox/px.png");


    result_panel = new EntityUI({ 400,300 }, { 600,200 }, mat,
        UI_BUTTON_UNDEFINED, "end_panel");
}

void EndStage::onEnter() {
    init2DCamera(Game::instance->window_width, Game::instance->window_height);
}

void EndStage::update(double dt) {
    // no logic needed
}

void EndStage::render(Camera* camera) {
    glDisable(GL_DEPTH_TEST);
    result_panel->render(&camera2D);
    glEnable(GL_DEPTH_TEST);
}
