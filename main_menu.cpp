#include "main_menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int Main_DATA[] =
{
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 1, 1,1, 1, 1, 1, 1, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

Mainmenu::~Mainmenu()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.die_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Mainmenu::initialise()
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, Main_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    if (start_or_return == 0) {
        m_state.player->set_position(glm::vec3(1.0f, -3.0f, 0.0f));
    }
    else {
        m_state.player->set_position(glm::vec3(7.0f, -3.0f, 0.0f));
    }

    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/player.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->LEFT];  // start looking left
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
        m_state.enemies[i].set_ai_type(WALKER);
        m_state.enemies[i].set_speed(0.5f);
        m_state.enemies[i].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
        m_state.enemies[i].m_texture_id = Utility::load_texture("assets/blue_slime.png");
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
        m_state.enemies[i].set_height(0.75f);
        m_state.enemies[i].set_width(0.75f);
    }
    m_state.enemies[0].set_position(glm::vec3(4.0f, -2.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f, -0.3f, 0.0f));
    m_state.enemies[1].set_position(glm::vec3(5.0f, -3.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.3f, 0.0f, 0.0f));

    //Bullet
    m_state.bullet = new Entity();
    m_state.bullet->set_entity_type(PROJECTILE);
    m_state.bullet->set_position(m_state.player->get_position());
    m_state.bullet->set_movement(glm::vec3(2.0f, 0.0f, 0.0f));
    m_state.bullet->set_speed(2.0f);
    m_state.bullet->set_acceleration(glm::vec3(0.0f));
    m_state.bullet->set_master(m_state.player);
    m_state.bullet->m_texture_id = Utility::load_texture("assets/bullet.png");
    m_state.enemies->set_height(0.5f);
    m_state.enemies->set_width(0.5f);
    /**
     BGM and SFX
     */
     //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

     //m_state.bgm = Mix_LoadMUS("assets/dooblydoo.mp3");
     //Mix_PlayMusic(m_state.bgm, -1);
     //Mix_VolumeMusic(0.0f);

     //m_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
}

void Mainmenu::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, m_state.bullet, 1, m_state.map);
    }
    m_state.bullet->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    if ((-4.5f < m_state.player->get_position().y < -2.5f) && (m_state.player->get_position().x > 7.0f)) m_state.next_scene_id = 1;
}

void Mainmenu::render(ShaderProgram* program)
{
    m_state.map->render(program);
    GLuint main_texture_id = Utility::load_texture("assets/font1.png");
    Utility::draw_text(program, main_texture_id, "Dungeon", 1.0f, 0.5f, glm::vec3(1.0f, -3.0f, 0.0f));
    Utility::draw_text(program, main_texture_id, "Walker", 1.0f, 0.5f, glm::vec3(1.0f, -4.0f, 0.0f));

}
