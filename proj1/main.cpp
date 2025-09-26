/**
* Author: Johnathan Huang
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "base/start.h"
#include <math.h>

// Global Constants
constexpr int SCREEN_WIDTH        = 1000 * 1.5f,
              SCREEN_HEIGHT       = 600 * 1.5f,
              FPS                 = 60,
              SIDES               = 3,
              SIZE                = 100;

constexpr float RADIUS          = 100.0f, // radius of the orbit
                ORBIT_SPEED     = 0.05f,  // the speed of orbit
                MAX_AMPLITUDE   = 10.0f,  
                PULSE_SPEED     = 100.0f;

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
Vector2 gScale = BASE_SIZE;

//COLOR
enum BG_COLORS { BG_BLACK, BG_GREEN, BG_LIGHTBLUE };
BG_COLORS screen_background = BG_BLACK;
int colorTimer = 0;
const char * BG_COLOUR = "#000000";

//PLANET
Vector2 planetPosition    = ORIGIN;
float planetSpeed = 2.0f;
float planetRotation = 0.0f;

//STAR
Vector2 starPosition = {0,0};
float gOrbitLocation = 0.0f;

//KIRBY
enum KirbyDirection { RIGHT, DOWN, LEFT, UP };
KirbyDirection kirbyDir = RIGHT;
Vector2 kirbyPosition = {100, 100};
Vector2 kirbyScale = {100.0f, 100.0f};
float kirbySpeed = 7.0f;
float kirbyRotation = 0.0f;
float kirbyAmp = 45;
float kirbyPulse = 0.0f;

//Textures
Texture2D starTexture;
Texture2D planetTexture;
Texture2D kirbyTexture;


// free pixel art
constexpr char PIXEL_STAR_FP[] = "assets/star.png";
constexpr char PIXEL_EARTH_FP[] = "assets/earth.png";
constexpr char PIXEL_KIRBY_FP[] = "assets/kirby.png";

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
    kirbyTexture = LoadTexture(PIXEL_KIRBY_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    /*-----------------------PLANET MOVEMENT----------------------------*/
    planetRotation += 1.0f;
    planetPosition.x += planetSpeed;
    if (planetPosition.x < 400 || planetPosition.x > SCREEN_WIDTH - 400){
        planetSpeed *= -1;
    }
    //Planet oscillation
    gPulseTime += 0.05f;
    planetPosition.y = ORIGIN.y + sin(gPulseTime) * 50.0f; 
    /*-----------------------STAR MOVEMENT----------------------------*/
    if (planetSpeed < 0){
        gAngle -= ORBIT_SPEED;
    }
    else{
        gAngle += ORBIT_SPEED;
    }
    starPosition.x  = planetPosition.x + RADIUS * cos(gAngle);
    starPosition.y  = planetPosition.y + RADIUS * sin(gAngle);

    /*-----------------------KIRBY MOVEMENT----------------------------*/
    //rocking back and forth
    kirbyRotation = sin(gPulseTime) * kirbyAmp;

    switch(kirbyDir){
        case RIGHT:
            kirbyPosition.x += kirbySpeed;
            if (kirbyPosition.x > SCREEN_WIDTH - 100){
                kirbyPosition.x = SCREEN_WIDTH - 100;
                kirbyDir = DOWN;
            }
            break;
        case DOWN:
            kirbyPosition.y += kirbySpeed;
            if (kirbyPosition.y > SCREEN_HEIGHT - 100){
                kirbyPosition.y = SCREEN_HEIGHT - 100;
                kirbyDir = LEFT;
            }
            break;
        case LEFT:
            kirbyPosition.x -= kirbySpeed;
            if (kirbyPosition.x < 100){
                kirbyPosition.x = 100;
                kirbyDir = UP;
            }
            break;
        case UP:
            kirbyPosition.y -= kirbySpeed;
            if (kirbyPosition.y < 100){
                kirbyPosition.y = 100;
                kirbyDir = RIGHT;
            }
            break;
    }
    // scale
    kirbyPulse += 1.0f;
    float kirbyOffset = SIZE + kirbyAmp * sin(kirbyPulse/PULSE_SPEED);
    kirbyScale.x = BASE_SIZE.x + kirbyOffset;
    kirbyScale.y = BASE_SIZE.y + kirbyOffset;

    /*-----------------------BACKGROUND----------------------------*/
    colorTimer++;
    switch(screen_background){
        case BG_BLACK:
            BG_COLOUR = "#000000";
            if (colorTimer > 100){
                screen_background = BG_GREEN;
            }
            break;
        case BG_GREEN:
            BG_COLOUR = "#90EE90";
            if (colorTimer > 200){
                screen_background = BG_LIGHTBLUE;
            }
            break;
        case BG_LIGHTBLUE:
            BG_COLOUR = "#ADD8E6";
            if (colorTimer > 300){
                screen_background = BG_BLACK;
                colorTimer = 0;
            }
            break;
    }
}

void render()
{
 BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    /*-----------------------PLANET DRAWING----------------------------*/
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
    /*-----------------------STAR DRAWING----------------------------*/
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

    /*-----------------------KIRBY DRAWING----------------------------*/
    Rectangle kTextureArea = {
        /*
        TOP LEFT CORNER
        */
        0,0,
        // bottom-right corner (of texture)
        static_cast<float>(kirbyTexture.width),
        static_cast<float>(kirbyTexture.height)
    };

    Rectangle kDestinationArea = {
        kirbyPosition.x,
        kirbyPosition.y,
        static_cast<float>(kirbyScale.x),
        static_cast<float>(kirbyScale.y)
    };

    Vector2 kObjectOrigin = {
        static_cast<float>(kirbyScale.x) / 2.0f,
        static_cast<float>(kirbyScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        kirbyTexture, 
        kTextureArea, 
        kDestinationArea, 
        kObjectOrigin, 
        kirbyRotation, 
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