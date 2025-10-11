/**
* Author: Johnathan Huang
* Assignment: Ping Pong
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "base/start.h"

enum BALL_STATE {INACTIVE, ACTIVE};

// Global Constants
constexpr int SCREEN_WIDTH  = 1600,
              SCREEN_HEIGHT = 900,
              FPS           = 60,
              SIZE          = 500 / 2,
              SPEED         = 200;

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BASE_SIZE      = { (float) SIZE, (float) SIZE },
                  INIT_TENNIS_POS  = { ORIGIN.x, ORIGIN.y },
                  BOMBER_POS = { SIZE/2, SCREEN_HEIGHT / 2 },
                  CHUCK_POS = { SCREEN_WIDTH - SIZE/2, SCREEN_HEIGHT / 2};


constexpr char  BOMBER_FP[]  = "assets/bomber.png",
                CHUCK_FP[] = "assets/chuck.png",
                TENNISBALL_FP[] = "assets/Tennisball.png";


// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gChuckPosition = CHUCK_POS,
        gBomberPosition = BOMBER_POS,
        gTennisBallPosition1 = INIT_TENNIS_POS,
        gTennisBallPosition2 = INIT_TENNIS_POS,
        gTennisBallPosition3 = INIT_TENNIS_POS,

        // MOVEMENT
        gChuckMovement = {0.0f, 0.0f},
        gBomberMovement = {0.0f, 0.0f},
        gTennisBallMovement1 = {0.0f, 0.0f},
        gTennisBallMovement2 = {0.0f, 0.0f},
        gTennisBallMovement3 = {0.0f, 0.0f},


        //Scale
        gChuckScale = BASE_SIZE,
        gBomberScale = BASE_SIZE,
        gTennisBallScale1 = {BASE_SIZE.x / 5.0f, BASE_SIZE.y / 5.0f},
        gTennisBallScale2 = {BASE_SIZE.x / 5.0f, BASE_SIZE.y / 5.0f},
        gTennisBallScale3 = {BASE_SIZE.x / 5.0f, BASE_SIZE.y / 5.0f};
    
Texture2D gChuckTexture,
          gBomberTexture,
          gTennisBallTexture1,
          gTennisBallTexture2,
          gTennisBallTexture3;

bool gPause = true;
unsigned int startTime;
bool two_player_game = true;
BALL_STATE ball1 = INACTIVE;
BALL_STATE ball2 = INACTIVE;
BALL_STATE ball3 = INACTIVE;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 * 
 * @see 
 * 
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");

    startTime = time(NULL);
    gBomberTexture = LoadTexture(BOMBER_FP);
    gChuckTexture = LoadTexture(CHUCK_FP);
    gTennisBallTexture1 = LoadTexture(TENNISBALL_FP);
    gTennisBallTexture2 = LoadTexture(TENNISBALL_FP);
    gTennisBallTexture3 = LoadTexture(TENNISBALL_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (gPause){
        if (IsKeyPressed(KEY_ONE)){
            ball1 = ACTIVE;
            gPause = false;
        }
        else if (IsKeyPressed(KEY_TWO)){
            ball1 = ACTIVE;
            ball2 = ACTIVE;
            gPause = false;
        } 
        else if (IsKeyPressed(KEY_THREE)){
            ball1 = ACTIVE;
            ball2 = ACTIVE;
            ball3 = ACTIVE;
            gPause = false;
        }
        else{
            return;
        }
    }

    if (IsKeyPressed(KEY_T)){
        two_player_game = !two_player_game;
        gChuckMovement.y = 1;
    }

    gBomberMovement = {0.0f, 0.0f};
    if      (IsKeyDown(KEY_W)) gBomberMovement.y = -1;
    else if (IsKeyDown(KEY_S)) gBomberMovement.y =  1;

    if (two_player_game){
        gChuckMovement  = {0.0f, 0.0f};
        if (IsKeyDown(KEY_UP)) gChuckMovement.y = -1;
        else if (IsKeyDown(KEY_DOWN)) gChuckMovement.y = 1;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    if (gPause) return;
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gBomberPosition = {
        gBomberPosition.x,
        gBomberPosition.y + SPEED * gBomberMovement.y * deltaTime
    };

    if (gBomberPosition.y + gBomberScale.y / 2 >= SCREEN_HEIGHT){
            gBomberPosition.y = SCREEN_HEIGHT - gBomberScale.y / 2;
        }
    else if (gBomberPosition.y - gBomberScale.y /2 <= 0){
            gBomberPosition.y =  gBomberScale.y /2;
    }

    if (!two_player_game){
        if (gChuckPosition.y + gChuckScale.y / 2 >= SCREEN_HEIGHT){
            gChuckMovement.y = -1;
        }
        else if (gChuckPosition.y - gChuckScale.y /2 <= 0){
            gChuckMovement.y = 1;
        }
    }

    gChuckPosition = {
        gChuckPosition.x,
        gChuckPosition.y + SPEED * gChuckMovement.y * deltaTime
    };
    if (gChuckPosition.y + gChuckScale.y / 2 >= SCREEN_HEIGHT){
        gChuckPosition.y = SCREEN_HEIGHT - gChuckScale.y / 2;
    }
    else if (gChuckPosition.y - gChuckScale.y /2 <= 0){
        gChuckPosition.y =  gChuckScale.y /2;
    }

    // if (isColliding(
    // )) printf("Collision @ %us in game time.\n", (unsigned) time(NULL) - startTime);
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    // Render Bomber
    renderObject(&gBomberTexture, &gBomberPosition, &gBomberScale);

    // Render Chuck
    renderObject(&gChuckTexture, &gChuckPosition, &gChuckScale);

    //Render Tennis Balls



    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gBomberTexture);
    UnloadTexture(gChuckTexture);
    UnloadTexture(gTennisBallTexture1);
    UnloadTexture(gTennisBallTexture2);
    UnloadTexture(gTennisBallTexture3);
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