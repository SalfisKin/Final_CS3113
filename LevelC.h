#include "Scene.h"

class LevelC : public Scene {
public:
    int ENEMY_COUNT = 3;
    bool splitted = 0;//deterimine if the slime has splitted
    ~LevelC();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
}; 
