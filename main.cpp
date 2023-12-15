#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f
#define LEVEL1_DOWN_EDGE -3.75f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "main_menu.h"
#include "Effects.h"

// 每每每每每 CONSTANTS 每每每每每 //
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;


const float MILLISECONDS_IN_SECOND = 1000.0;


// 每每每每每 GLOBAL VARIABLES 每每每每每 //
enum Game {
    GameStart,
    Win,
    Lose
};
Game currgamestate = GameStart;
Scene* g_current_scene;
Mainmenu* g_main;
LevelA* g_levelA;
LevelB* g_levelB;
LevelC* g_levelC;


Effects* g_effects;
Scene* g_levels[4];

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

const char V_SHADER_PATH[] = "shaders/vertex_lit.glsl",
F_SHADER_PATH[] = "shaders/fragment_lit.glsl";

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;

// 每每每每每 GENERAL FUNCTIONS 每每每每每 //
void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Final Assignment!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_main = new Mainmenu();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();

    g_levels[0] = g_main;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    // Start at level A
    switch_to_scene(g_levels[0]);

    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    //g_effects->start(SHRINK, 2.0f);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                // Jump

                break;
            case SDLK_RETURN:
                if (g_current_scene == g_main) {
                    g_current_scene->m_state.next_scene_id = 1;
                }
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_state.player->move_left();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_state.player->move_right();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }
    else if (key_state[SDL_SCANCODE_UP])
    {
        g_current_scene->m_state.player->move_up();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->UP];
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        g_current_scene->m_state.player->move_down();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->DOWN];
    }


    if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
    }
}

void update()
{
    //if (currgamestate != GameStart) {
    //    return;
    //}
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);        

        g_is_colliding_bottom = g_current_scene->m_state.player->m_collided_bottom;

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);

    if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE&& g_current_scene->m_state.player->get_position().y < LEVEL1_DOWN_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, -g_current_scene->m_state.player->get_position().y, 0));
    }
    else if (g_current_scene->m_state.player->get_position().y < LEVEL1_DOWN_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, -g_current_scene->m_state.player->get_position().y, 0));
    }
    else if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
    }
    else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
 
    
    //when the player reaches the gate of scene A, switch to scene A
    if (g_current_scene == g_levelA && g_current_scene->m_state.player->get_position().y <-2.95f
        && g_current_scene->m_state.player->get_position().y > -4.95f&& g_current_scene->m_state.player->get_position().x > 7.95f) switch_to_scene(g_levelB);
    //the player could return to scene A from the left door of scene B
    if (g_current_scene == g_levelB && g_current_scene->m_state.player->get_position().y <-2.95f
        && g_current_scene->m_state.player->get_position().y > -4.95f && g_current_scene->m_state.player->get_position().x < 1.05f)
    {
        g_levelA->start_or_return = 1;//mark sceneA as "return"
        switch_to_scene(g_levelA);
    }
    //when the player reaches the bottom gate of scene B,switch to scene C
    if (g_current_scene == g_levelB && g_current_scene->m_state.player->get_position().x <14.95f
        && g_current_scene->m_state.player->get_position().x > 12.95f && g_current_scene->m_state.player->get_position().y < -9.95f) switch_to_scene(g_levelC);
    //The player could return to scene B from the top door of scene C
    if (g_current_scene == g_levelC && g_current_scene->m_state.player->get_position().x <4.05f
        && g_current_scene->m_state.player->get_position().x > 2.05f && g_current_scene->m_state.player->get_position().y > -1.05f)
    {
        g_levelB->start_or_return = 1;//mark sceneB as "return"
        switch_to_scene(g_levelB);
    }

    if (g_current_scene->m_state.player->get_active_state() == false) {
        currgamestate = Lose;
        g_effects->start(SHAKE, 1.0f);
        Mix_PlayChannel(-1, g_current_scene->m_state.die_sfx, 0);
        g_current_scene->m_state.die_sfx = NULL;
        g_current_scene->set_game_state(-1);
    }
    if (g_current_scene == g_levelC && g_current_scene->m_state.player->get_position().y <-2.95f
        && g_current_scene->m_state.player->get_position().y > -4.95f && g_current_scene->m_state.player->get_position().x > 7.95f) {
        currgamestate = Win;
        g_current_scene->set_game_state(1);
    }

    g_view_matrix = glm::translate(g_view_matrix, g_effects->m_view_offset);
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);
    g_effects->render();
    g_shader_program.SetLightPosition(g_current_scene->m_state.player->get_position());
    SDL_GL_SwapWindow(g_display_window);

}

void shutdown()
{
    SDL_Quit();

    delete g_levelA;
    delete g_levelB;
    delete g_effects;
}

// 每每每每每 DRIVER GAME LOOP 每每每每每 //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();

        if (g_current_scene->m_state.next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id]);

        render();
    }

    shutdown();
    return 0;
}