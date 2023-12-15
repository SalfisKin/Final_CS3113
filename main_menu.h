#include "Scene.h"

class Mainmenu : public Scene {
public:
    int ENEMY_COUNT = 1;

    ~Mainmenu();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
#pragma once
