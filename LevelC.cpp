#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int LEVELC_DATA[] =
{
    2, 2, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelC::~LevelC()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.die_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelC::initialise()
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(3.0f, -1.5f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/player.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.75f);
    m_state.player->set_width(0.75f);

    // Jumping
    m_state.player->m_jumping_power = 5.0f;

    /**
     Enemies' stuff */


    m_state.enemies = new Entity[ENEMY_COUNT];


    /**/
    // Walking for enemy

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_ai_type(GUARD);
        m_state.enemies[i].set_ai_state(IDLE);
        m_state.enemies[i].set_speed(0.5f);
        m_state.enemies[i].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
        m_state.enemies[i].m_walking[m_state.enemies[i].LEFT] = new int[4]{ 1, 5, 9,  13 };
        m_state.enemies[i].m_walking[m_state.enemies[i].RIGHT] = new int[4]{ 3, 7, 11, 15 };
        m_state.enemies[i].m_walking[m_state.enemies[i].UP] = new int[4]{ 2, 6, 10, 14 };
        m_state.enemies[i].m_walking[m_state.enemies[i].DOWN] = new int[4]{ 0, 4, 8,  12 };
        m_state.enemies[i].m_animation_indices = m_state.enemies[i].m_walking[m_state.enemies[i].LEFT];  // start looking left
        m_state.enemies[i].m_animation_frames = 4;
        m_state.enemies[i].m_animation_index = 0;
        m_state.enemies[i].m_animation_time = 0.0f;
        m_state.enemies[i].m_animation_cols = 4;
        m_state.enemies[i].m_animation_rows = 4;
        
    }
    m_state.enemies[0].m_texture_id = Utility::load_texture("assets/pink_slime.png");
    m_state.enemies[1].m_texture_id = Utility::load_texture("assets/pink_slime_smaller.png");
    m_state.enemies[2].m_texture_id = Utility::load_texture("assets/pink_slime_smaller.png");
    m_state.enemies[0].set_position(glm::vec3(5.0f, -4.0f, 0.0f));
    m_state.enemies[1].set_position(glm::vec3(6.0f, -5.0f, 0.0f));
    m_state.enemies[2].set_position(glm::vec3(6.0f, -3.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.2f, 0.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.2f, 0.0f, 0.0f));
    m_state.enemies[2].set_movement(glm::vec3(0.2f, 0.0f, 0.0f));
    m_state.enemies[0].set_height(0.75f);
    m_state.enemies[0].set_width(0.75f);
    m_state.enemies[1].set_height(0.5f);
    m_state.enemies[1].set_width(0.5f);
    m_state.enemies[2].set_height(0.5f);
    m_state.enemies[2].set_width(0.5f);

    //Bullet
    m_state.bullet = new Entity();
    m_state.bullet->set_entity_type(PROJECTILE);
    m_state.bullet->set_position(m_state.player->get_position());
    m_state.bullet->set_movement(glm::vec3(2.0f, 0.0f, 0.0f));
    m_state.bullet->set_speed(2.0f);
    m_state.bullet->set_acceleration(glm::vec3(0.0f));
    m_state.bullet->set_master(m_state.player);
    m_state.bullet->m_texture_id = Utility::load_texture("assets/bullet.png");
    m_state.enemies->set_height(0.25f);
    m_state.enemies->set_width(0.25f);
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/Background.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    m_state.die_sfx = Mix_LoadWAV("assets/Defeated.wav");
}

void LevelC::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }
    m_state.bullet->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    if (m_state.player->get_position().y < -10.0f) m_state.next_scene_id = 1;
    if (m_state.enemies[0].get_active_state() == false) {
        splitted = true;
    }
}

void LevelC::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    if (m_state.enemies[0].get_active_state()) {
        m_state.enemies[0].render(program);
    }
    if (splitted) {
        if (m_state.enemies[1].get_active_state()) {
            //m_state.enemies[1].set_position(glm::vec3(m_state.enemies[0].get_position().x+1, m_state.enemies[0].get_position().y - 2.0f, 0.0f));
            m_state.enemies[1].render(program);
        }
        if (m_state.enemies[2].get_active_state()) {
            //m_state.enemies[2].set_position(glm::vec3(m_state.enemies[0].get_position().x, m_state.enemies[0].get_position().y - 1.0f, 0.0f));
            m_state.enemies[2].render(program);
        }
    }
    if (game_state == 1) {
        GLuint font_texture_idA = Utility::load_texture("assets/font1.png");
        Utility::draw_text(program, font_texture_idA, "win", 1.0f, 0.1f, glm::vec3(4.0f, -3.0f, 0.0f));
    }
    if (game_state == -1) {
        GLuint font_texture_idA = Utility::load_texture("assets/font1.png");
        Utility::draw_text(program, font_texture_idA, "lose", 1.0f, 0.1f, glm::vec3(4.0f, -3.0f, 0.0f));
    }
    m_state.bullet->render(program);

}
