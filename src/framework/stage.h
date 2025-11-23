#pragma once

#include <vector>


#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
//#include "game/game.h"
#include "framework/camera.h"

#include "framework/entities/entity_ui.h"

// FORWARD DECLARE
class World;
class Game;


// ---------------------------------------------------------
// ENUM FOR STAGE IDs
// ---------------------------------------------------------

enum eStage {
    MENUSTAGE,
    PLAYSTAGE,
    WINSTAGE,
    LOSESTAGE
};

// ---------------------------------------------------------
// BASE STAGE CLASS
// ---------------------------------------------------------

class Stage {
public:
    World* world = nullptr;
    Camera camera2D;

    virtual ~Stage() {}

    virtual void onEnter() {}
    virtual void onExit() {}

    virtual void update(double dt) {}
    virtual void render(Camera* world_camera) {}

    void init2DCamera(int width, int height) {
        camera2D.view_matrix.setIdentity();
        camera2D.setOrthographic(0, width, height, 0, -1, 1);
    }
};

// ---------------------------------------------------------
// MENU STAGE
// ---------------------------------------------------------

class MenuStage : public Stage {
public:
    std::vector<EntityUI*> ui_elements;

    MenuStage();

    void onEnter() override;
    void onExit() override {}
    void update(double dt) override;
    void render(Camera* world_camera) override;
};

// ---------------------------------------------------------
// PLAY STAGE
// ---------------------------------------------------------

class PlayStage : public Stage {
public:
    std::vector<EntityUI*> hud_elements;

    PlayStage();

    void onEnter() override;
    void onExit() override {}
    void update(double dt) override;
    void render(Camera* camera) override;
};

// ---------------------------------------------------------
// END STAGE (WIN / LOSE)
// ---------------------------------------------------------

class EndStage : public Stage {
public:
    EntityUI* result_panel = nullptr;
    bool win_screen = false;

    EndStage(bool win_screen = false);

    void onEnter() override;
    void onExit() override {}
    void update(double dt) override;
    void render(Camera* camera) override;
};
