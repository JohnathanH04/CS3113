#include "base/LevelC.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 4;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

LevelMenu *gLevelMenu = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;

//life tracker
int lives_left = 2000;
bool winner;

// Function Declarations
void switchToScene(Scene *scene, int lives);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene, int lives)
{   
    //unload previous level 
    if (gCurrentScene != nullptr)
        gCurrentScene->shutdown();

    gCurrentScene = scene;
    gCurrentScene->setLives(lives);
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rise");
    InitAudioDevice();

    //push back all created levels
    gLevelMenu = new LevelMenu(ORIGIN, "#000000");
    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#011627");
    gLevelC = new LevelC(ORIGIN, "#0a1579ff");

    gLevels.push_back(gLevelMenu);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    switchToScene(gLevels[0], 2000);

    SetTargetFPS(FPS);
}

void processInput() 
{
    // check for start/end menu keys --> make sure scene cannot call other functions until it is a playable level
    if (gCurrentScene == gLevels[0] || gCurrentScene == gLevels[4]){
        if (gCurrentScene == gLevels[0] && IsKeyPressed(KEY_ENTER)){
            switchToScene(gLevels[3], 2000);
        }

        if (gCurrentScene == gLevels[4] && IsKeyPressed(KEY_R)){
            switchToScene(gLevels[gCurrentScene->getState().nextSceneID], 2000);
        }

        if (IsKeyPressed(KEY_Q) || WindowShouldClose()) {
            gAppStatus = TERMINATED;
            }
        return;
    }
    gCurrentScene->getState().rabbit->resetMovement();

    if      (IsKeyDown(KEY_A)) gCurrentScene->getState().rabbit->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().rabbit->moveRight();
    else if (IsKeyDown(KEY_W)) gCurrentScene->getState().rabbit->moveUp();
    else if (IsKeyDown(KEY_S)) gCurrentScene->getState().rabbit->moveDown();

    if (IsKeyPressed(KEY_SPACE)){
        if (!gCurrentScene->getState().ammoQueue.empty()){
            int index = gCurrentScene->getState().ammoQueue.front();
            Entity& ammo = gCurrentScene->getState().ammo[index];
            if (!ammo.isActive()){
                gCurrentScene->getState().ammoQueue.pop();           
                ammo.setPosition(gCurrentScene->getState().rabbit->getPosition());
                
                Direction ammo_movement = gCurrentScene->getState().rabbit->getDirection();
                
                if (ammo_movement == RIGHT){
                    ammo.setMovement({2.0f, 0.0f});
                    ammo.setAngle(0.0f);
                }
                else if (ammo_movement == LEFT){
                    ammo.setMovement({-2.0f, 0.0f});
                    ammo.setAngle(180.0f);
                }
                else if (ammo_movement == DOWN){
                    ammo.setMovement({0.0f, 2.0f});
                    ammo.setAngle(90.0f);
                }
                else if (ammo_movement == UP){
                    ammo.setMovement({0.0f, -2.0f});
                    ammo.setAngle(-90.0f);
                }
                ammo.activate();
            }
        }
    }

    if (GetLength(gCurrentScene->getState().rabbit->getMovement()) > 1.0f) 
        gCurrentScene->getState().rabbit->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    // only use camera in level scenes
    if (gCurrentScene != gLevels[0] && gCurrentScene != gLevels[4]){
        BeginMode2D(gCurrentScene->getState().camera);

        gCurrentScene->render();
        EndMode2D();
        gCurrentScene->renderUI();
    } else {
        gCurrentScene->render();
    }
    EndDrawing();
}

void shutdown() 
{

    delete gLevelMenu;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;


    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        if (lives_left > 0){
            lives_left = gCurrentScene->getState().cur_lives;

            if (gCurrentScene->getState().reset_scene){
                int id = gCurrentScene->getState().curSceneID;
                //reset scene
                switchToScene(gLevels[id], lives_left);
            }

            if (gCurrentScene->getState().next_scene){
                int id = gCurrentScene->getState().nextSceneID;
                //switch to next scene
                switchToScene(gLevels[id], lives_left);
            }
        }
        else if (lives_left == 0){
            lives_left = 2000;
            winner = false;
            switchToScene(gLevels[0], 2000);
        }
        render();
    }

    shutdown();
    return 0;
}