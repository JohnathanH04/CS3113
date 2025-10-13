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
              SCREEN_HEIGHT = 798,
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
                TENNISBALL_FP[] = "assets/Tennisball.png",
                BACKGROUND_FP[] = "assets/background.png",
                PLAYER1_WIN_FP[] = "assets/Player1Win.png",
                PLAYER2_WIN_FP[] = "assets/Player2Win.png";


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
        gTennisBallMovement1 = {-1.0f, 1.0f},
        gTennisBallMovement2 = {1.0f, 1.0f},
        gTennisBallMovement3 = {-1.0f, -1.0f},


        //Scale
        gChuckScale = {200.0f, 250.f},
        gBomberScale = {200.0f, 250.f},
        gTennisBallScale1 = {BASE_SIZE.x / 5.0f, BASE_SIZE.y / 5.0f},
        gTennisBallScale2 = {BASE_SIZE.x / 5.0f, BASE_SIZE.y / 5.0f},
        gTennisBallScale3 = {BASE_SIZE.x / 5.0f, BASE_SIZE.y / 5.0f};
    
Texture2D gChuckTexture,
          gBomberTexture,
          gTennisBallTexture1,
          gTennisBallTexture2,
          gTennisBallTexture3,
          gWinTexture1,
          gWinTexture2,
          gBackgroundTexture;

unsigned int startTime;
bool two_player_game = true;
int winner = 0;
bool winner_found = false;
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
    gBackgroundTexture = LoadTexture(BACKGROUND_FP);
    gWinTexture1 = LoadTexture(PLAYER1_WIN_FP);
    gWinTexture2 = LoadTexture(PLAYER2_WIN_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (IsKeyPressed(KEY_ONE)){
        ball1 = ACTIVE;
    }
    else if (IsKeyPressed(KEY_TWO)){
        ball1 = ACTIVE;
        ball2 = ACTIVE;
    } 
    else if (IsKeyPressed(KEY_THREE)){
        ball1 = ACTIVE;    
        ball2 = ACTIVE;
        ball3 = ACTIVE;
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
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gBomberPosition = {
        gBomberPosition.x,
        gBomberPosition.y + SPEED * gBomberMovement.y * deltaTime
    };

    // bound bomber
    if (gBomberPosition.y + gBomberScale.y / 2 >= SCREEN_HEIGHT){
            gBomberPosition.y = SCREEN_HEIGHT - gBomberScale.y / 2;
        }
    else if (gBomberPosition.y - gBomberScale.y /2 <= 0){
            gBomberPosition.y =  gBomberScale.y /2;
    }
    // single player
    if (!two_player_game){
        if (gChuckPosition.y + gChuckScale.y / 2 >= SCREEN_HEIGHT){
            gChuckMovement.y = -1;
        }
        else if (gChuckPosition.y - gChuckScale.y /2 <= 0){
            gChuckMovement.y = 1;
        }
    }
    //bound chuck
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

    if (ball1 == ACTIVE){
        gTennisBallPosition1= {
            gTennisBallPosition1.x + SPEED * gTennisBallMovement1.x * deltaTime,
            gTennisBallPosition1.y + SPEED * gTennisBallMovement1.y * deltaTime
        };
        
        if (gTennisBallPosition1.y < gTennisBallScale1.y / 2 || gTennisBallPosition1.y + gTennisBallScale1.y / 2 > SCREEN_HEIGHT){
            gTennisBallMovement1.y *= -1;
        }

        if (isColliding(&gChuckPosition, &gChuckScale, &gTennisBallPosition1, &gTennisBallScale1)){
            gTennisBallMovement1.x *= -1;
        }

        if (isColliding(&gBomberPosition, &gBomberScale, &gTennisBallPosition1, &gTennisBallScale1)){
            gTennisBallMovement1.x *= -1;
        }

        if (gTennisBallPosition1.x < 0){
            winner = 2;
            winner_found = true;
        }
        else if (gTennisBallPosition1.x > SCREEN_WIDTH){
            winner = 1;
            winner_found = true;
        }
    }

    if (ball2 == ACTIVE){
        gTennisBallPosition2= {
            gTennisBallPosition2.x + SPEED * gTennisBallMovement2.x * deltaTime,
            gTennisBallPosition2.y + SPEED * gTennisBallMovement2.y * deltaTime
        };
        
        if (gTennisBallPosition2.y < gTennisBallScale2.y / 2 || gTennisBallPosition2.y + gTennisBallScale2.y / 2 > SCREEN_HEIGHT){
            gTennisBallMovement2.y *= -1;
        }

        if (isColliding(&gChuckPosition, &gChuckScale, &gTennisBallPosition2, &gTennisBallScale2)){
           gTennisBallMovement2.x *= -1;
        }

        if (isColliding(&gBomberPosition, &gBomberScale, &gTennisBallPosition2, &gTennisBallScale2)){
            gTennisBallMovement2.x *= -1;
        }
        if (gTennisBallPosition2.x < 0){
            winner = 2;
            winner_found = true;
        }
        else if (gTennisBallPosition2.x > SCREEN_WIDTH){
            winner = 1;
            winner_found = true;
        }
    }

    if (ball3 == ACTIVE){
        gTennisBallPosition3= {
            gTennisBallPosition3.x + SPEED * gTennisBallMovement3.x * deltaTime,
            gTennisBallPosition3.y + SPEED * gTennisBallMovement3.y * deltaTime
        };
        
        if (gTennisBallPosition3.y < gTennisBallScale3.y / 2 || gTennisBallPosition3.y + gTennisBallScale3.y / 2 > SCREEN_HEIGHT){
            gTennisBallMovement3.y *= -1;
        }

        if (isColliding(&gChuckPosition, &gChuckScale, &gTennisBallPosition3, &gTennisBallScale3)){
            gTennisBallMovement3.x *= -1;
        }
        if (isColliding(&gBomberPosition, &gBomberScale, &gTennisBallPosition3, &gTennisBallScale3)){
            gTennisBallMovement3.x *= -1;
        }
        if (gTennisBallPosition3.x < 0){
            winner = 2;
            winner_found = true;
        }
        else if (gTennisBallPosition3.x > SCREEN_WIDTH){
            winner = 1;
            winner_found = true;
        }
    }
}
void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    DrawTexture(gBackgroundTexture, 0, 0, WHITE);

    if (winner_found){
        if (winner == 1){
            DrawTexture(gWinTexture1, 0, 0, WHITE);
        }
        else{
            DrawTexture(gWinTexture2, 0, 0, WHITE);
        }
    }
    else{
        // Render Bomber
        renderObject(&gBomberTexture, &gBomberPosition, &gBomberScale);

        // Render Chuck
        renderObject(&gChuckTexture, &gChuckPosition, &gChuckScale);

        //Render Tennis Balls
        if (ball1 == ACTIVE){
            renderObject(&gTennisBallTexture1, &gTennisBallPosition1, &gTennisBallScale1);
        }
        if (ball2 == ACTIVE){
            renderObject(&gTennisBallTexture2, &gTennisBallPosition2, &gTennisBallScale2);
        }
        if (ball3 == ACTIVE){
            renderObject(&gTennisBallTexture3, &gTennisBallPosition3, &gTennisBallScale3);
       }
    }
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
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gWinTexture1);
    UnloadTexture(gWinTexture2);
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