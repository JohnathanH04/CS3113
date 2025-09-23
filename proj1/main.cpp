#include "base/start.h"
#include <math.h>

// Global Constants
constexpr int SCREEN_WIDTH        = 800 * 1.5f,
              SCREEN_HEIGHT       = 450 * 1.5f,
              FPS                 = 60,
              SIDES               = 3,
              SIZE                = 100;

constexpr float RADIUS          = 100.0f, // radius of the orbit
                ORBIT_SPEED     = 0.05f,  // the speed at which the triangle will travel its orbit
                MAX_AMPLITUDE   = 10.0f,  // by how much the triangle will be expanding/contracting
                PULSE_SPEED     = 100.0f, // how fast the triangle is going to be "pulsing"
                PULSE_INCREMENT = 10.0f;  // the current value we're scaling by

constexpr char    BG_COLOUR[] = "#000000";
constexpr Vector2 BASE_SIZE   = { (float) SIZE, (float) SIZE };


constexpr Vector2 ORIGIN = { 
    SCREEN_WIDTH  / 2, 
    SCREEN_HEIGHT / 2
};

// Global Variables
AppStatus gAppStatus = RUNNING;

float gScaleFactor   = 50.0f,
      gAngle         = 0.0f,
      gPulseTime     = 0.0f;
Vector2 planetPosition    = ORIGIN;
float planetSpeed = 2.0f;
float planetRotation = 0.0f;
//star
Vector2 starPosition = {0,0};
Vector2 gScale = BASE_SIZE;
float gOrbitLocation = 0.0f;
Texture2D starTexture;
Texture2D planetTexture;


// free pixel art
constexpr char PIXEL_STAR_FP[] = "assets/star.png";
constexpr char PIXEL_EARTH_FP[] = "assets/earth.png";

// Function Declarations
Color ColorFromHex(const char *hex);
void initialise();
void processInput();
void update();
void render();
void shutdown();

#include <stdio.h>
#include <stdlib.h>

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project1");

    starTexture = LoadTexture(PIXEL_STAR_FP);
    planetTexture = LoadTexture(PIXEL_EARTH_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    planetRotation += 1.0f;
    planetPosition.x += planetSpeed;
    if (planetPosition.x < 100 || planetPosition.x > SCREEN_WIDTH - 100){
        planetSpeed *= -1;
    }
    //Planet oscillation
    gPulseTime += 0.05f;
    planetPosition.y = ORIGIN.y + sin(gPulseTime) * 50.0f; 
    
    /**
     * ORBIT EFFECT
     */
    if (planetSpeed < 0){
        gAngle -= ORBIT_SPEED;
    }
    else{
        gAngle += ORBIT_SPEED;
    }
    starPosition.x  = planetPosition.x + RADIUS * cos(gAngle);
    starPosition.y  = planetPosition.y + RADIUS * sin(gAngle);
}

void render()
{
 BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

        Rectangle pTextureArea = {
        /*
        TOP LEFT CORNER
        */
        0,0,
        // bottom-right corner (of texture)
        static_cast<float>(planetTexture.width),
        static_cast<float>(planetTexture.height)
    };

    Rectangle pDestinationArea = {
        planetPosition.x,
        planetPosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    Vector2 pObjectOrigin = {
        static_cast<float>(gScale.x) / 2.0f,
        static_cast<float>(gScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        planetTexture, 
        pTextureArea, 
        pDestinationArea, 
        pObjectOrigin, 
        planetRotation, 
        WHITE
    );

    Rectangle sTextureArea = {
        /*
        TOP LEFT CORNER
        */
        0,0,
        // bottom-right corner (of texture)
        static_cast<float>(starTexture.width),
        static_cast<float>(starTexture.height)
    };

    Rectangle sDestinationArea = {
        starPosition.x,
        starPosition.y,
        static_cast<float>(gScale.x),
        static_cast<float>(gScale.y)
    };

    Vector2 sObjectOrigin = {
        static_cast<float>(gScale.x) / 2.0f,
        static_cast<float>(gScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        starTexture, 
        sTextureArea, 
        sDestinationArea, 
        sObjectOrigin, 
        gAngle, 
        WHITE
    );
    EndDrawing();
}

void shutdown() 
{ 
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