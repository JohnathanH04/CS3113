/**
* Author: Johnathan Huang
* Assignment: Lunar Lander
* Date due: 2025-10-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "base/Entity.h"

struct GameState
{
    Entity *bat;
    Entity *tiles;
    Entity *blocks;
};

// Global Constants
constexpr int SCREEN_WIDTH  = 2000,
              SCREEN_HEIGHT = 1500,
              FPS           = 120;

constexpr char    BG_COLOUR[]      = "#C0897E";
constexpr char    BACKGROUND_FP[] = "assets/caveRemasteredV7.png",
                  WINNER_FP[] = "assets/MissionWin.png",
                  NO_FUEL_FP[] = "assets/noFuel.png",
                  BOUNDS_FP[] = "assets/outofBounds.png",
                  LOSE_FP[] = "assets/crashFail.png";

constexpr Vector2 ORIGIN           = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  ATLAS_DIMENSIONS = { 7, 5 };

constexpr int   NUMBER_OF_TILES         = 25,
                NUMBER_OF_BLOCKS        = 4;
constexpr float TILE_DIMENSION          = 100.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 8.0f,
                DECELERATION_OF_X       = 0.3f,
                FIXED_TIMESTEP          = 1.0f / 60.0f,
                END_GAME_THRESHOLD      = 1400.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;
int fuel = 1000;
bool out_of_bounds = false;
GameState gState;
Texture2D gBackgroundTexture,
          gWinTexture,
          gLoseTexture,
          gBoundTexture,
          gFuelTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void reset();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");

    gBackgroundTexture = LoadTexture(BACKGROUND_FP);
    gWinTexture = LoadTexture(WINNER_FP);
    gLoseTexture = LoadTexture(LOSE_FP);
    gBoundTexture = LoadTexture(BOUNDS_FP);
    gFuelTexture = LoadTexture(NO_FUEL_FP);

    /*
        ----------- bat -----------
    */
    std::map<Direction, std::vector<int>> batAnimationAtlas = {
        {DOWN,  {  0,  1,  2,  3,}},
    };

    float sizeRatio  = 48.0f / 64.0f;

    gState.bat = new Entity(
        {ORIGIN.x - 300.0f, ORIGIN.y - 200.0f}, // position
        {150.0f * sizeRatio, 150.0f},           // scale
        "assets/Bat_Sprite_Sheet.png",                 // texture file address
        ATLAS,                                  // single image or atlas?
        ATLAS_DIMENSIONS,                       // atlas dimensions
        batAnimationAtlas,                  // actual atlas
        PLAYER                                  // entity type
    );

    gState.bat->setColliderDimensions({
        gState.bat->getScale().x / 1.2f,
        gState.bat->getScale().y / 1.2f
    });
    gState.bat->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    /*
        ----------- TILES -----------
    */
    gState.tiles = new Entity[NUMBER_OF_TILES];

    // Compute the left‑most x coordinate so that the entire row is centred
    float leftMostX = ORIGIN.x - (NUMBER_OF_TILES * TILE_DIMENSION) / 2.0f;

    for (int i = 0; i < NUMBER_OF_TILES; i++) 
    {
        gState.tiles[i].setTexture("assets/saw_trap.png");
        gState.tiles[i].setEntityType(PLATFORM);
        gState.tiles[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
        gState.tiles[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION});
        gState.tiles[i].setPosition({
            leftMostX + i * TILE_DIMENSION, 
            ORIGIN.y + 300.0f + TILE_DIMENSION
        });
    }

    /*
        ----------- BLOCKS -----------
    */
    gState.blocks = new Entity[NUMBER_OF_BLOCKS];

    for (int i = 0; i < NUMBER_OF_BLOCKS; i++) 
    {
        gState.blocks[i].setTexture("assets/platform.png");
        gState.blocks[i].setEntityType(BLOCK);
        gState.blocks[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
        gState.blocks[i].setColliderDimensions(
            {TILE_DIMENSION, TILE_DIMENSION});
        gState.blocks[i].setSpeed(1.0f);
    }

    gState.blocks[0].setPosition(
        {ORIGIN.x - TILE_DIMENSION * 6, ORIGIN.y - TILE_DIMENSION});
    gState.blocks[1].setPosition(
        {ORIGIN.x, ORIGIN.y - TILE_DIMENSION * 4.0f});
    gState.blocks[2].setPosition(
        {ORIGIN.x + TILE_DIMENSION * 8, ORIGIN.y - TILE_DIMENSION + 200.0f});
    gState.blocks[3].setPosition(
        {ORIGIN.x + TILE_DIMENSION, ORIGIN.y - TILE_DIMENSION + 200.0f});

    gState.blocks[1].setMovement({1.0f, 0});
    gState.blocks[2].setMovement({-1.0f,0});


    SetTargetFPS(FPS);
}

void processInput() 
{
    gState.bat->resetMovement();

    float Y_accel_gravity = ACCELERATION_OF_GRAVITY;
    float X_accel = 0;

    // move left -> turn left
    if (IsKeyDown(KEY_A) && fuel > 0){
        fuel--;
        gState.bat->moveLeft();
        X_accel = gState.bat->getAcceleration().x;
    }
    // move right -> turn right
    else if (IsKeyDown(KEY_D) && fuel > 0){
        fuel--;
        gState.bat->moveRight();
        X_accel = gState.bat->getAcceleration().x;
    }
    else{
        X_accel = -DECELERATION_OF_X * gState.bat->getVelocity().x;
    }

    if (IsKeyDown(KEY_W)){
        fuel--;
        gState.bat->jump();
        Y_accel_gravity = gState.bat->getAcceleration().y;
    }

    gState.bat->setAcceleration({X_accel, Y_accel_gravity});

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    if (gState.bat->getGameStatus() == PLAYER_LOSE){
        return;
    }
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if (fuel == 0){
        gState.bat->setGStatus(PLAYER_LOSE);
    }

    Vector2 batPos = gState.bat->getPosition();

    if (batPos.x < 0 || batPos.x > SCREEN_WIDTH){
        out_of_bounds = true;
        gState.bat->setGStatus(PLAYER_LOSE);
    }

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gState.bat->update(FIXED_TIMESTEP, nullptr, gState.tiles, 
            NUMBER_OF_TILES, gState.blocks, NUMBER_OF_BLOCKS);

        for (int i = 0; i < NUMBER_OF_BLOCKS; i++){
            gState.blocks[i].update(FIXED_TIMESTEP, nullptr, nullptr, 0, 
                nullptr, 0);

            if (gState.blocks[i].getPosition().x < 0){
                gState.blocks[i].setPosition({30, gState.blocks[i].getPosition().y});
                gState.blocks[i].setVelocity({0,0});
                gState.blocks[i].setMovement({1.0f, 0.0f});
            }
            else if (gState.blocks[i].getPosition().x > 1900){
                gState.blocks[i].setPosition({1880, gState.blocks[i].getPosition().y});
                gState.blocks[i].setVelocity({0,0});
                gState.blocks[i].setMovement({-1.0f, 0.0f});
            }
        }
        
        for (int i = 0; i < NUMBER_OF_TILES; i++){ 
            gState.tiles[i].update(FIXED_TIMESTEP, nullptr, nullptr, 0, 
                nullptr, 0);
        }
        deltaTime -= FIXED_TIMESTEP;
    }

    if (gState.bat->getPosition().y > END_GAME_THRESHOLD) 
        gAppStatus = TERMINATED;
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    if (gState.bat->getGameStatus() == PLAYER_ALIVE){
        DrawTexture(gBackgroundTexture, 0, 0, WHITE);

        gState.bat->render();
        // generate text 
        DrawText(TextFormat("Fuel: %d", fuel), 20, 20, 60, WHITE);
        for (int i = 0; i < NUMBER_OF_TILES;  i++) gState.tiles[i].render();
        for (int i = 0; i < NUMBER_OF_BLOCKS; i++) gState.blocks[i].render();
    }
    else{
        if (gState.bat->getGameStatus() == PLAYER_WIN){
            DrawTexture(gWinTexture, 0, 0, WHITE);
        }
        else{// set game background after loss
            if (fuel <= 0){ // fuel fail
                DrawTexture(gFuelTexture, 0, 0, WHITE);
            }
            else if (out_of_bounds){ // out of bounds loss
                DrawTexture(gBoundTexture, 0, 0, WHITE);
            }
            else{ // crash loss
                DrawTexture(gLoseTexture, 0, 0, WHITE);
            }
        }
    }

    EndDrawing();
}

void shutdown() 
{
    delete   gState.bat;
    delete[] gState.tiles;
    delete[] gState.blocks;
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gWinTexture);
    UnloadTexture(gLoseTexture);
    UnloadTexture(gBoundTexture);
    UnloadTexture(gFuelTexture);
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
