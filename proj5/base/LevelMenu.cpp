#include "LevelMenu.h"

LevelMenu::LevelMenu()                                      : Scene { {0.0f}, nullptr   } {}
LevelMenu::LevelMenu(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelMenu::~LevelMenu() { shutdown(); }

void LevelMenu::initialise()
{
   mGameState.curSceneID = 0;
   mGameState.nextSceneID = 1;
   mGameState.reset_scene = false;
   mGameState.next_scene = false;

   mGameState.bgm = LoadMusicStream("assets/Journey To Ascend.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   float sizeRatio  = 48.0f / 64.0f;
}

void LevelMenu::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

}

void LevelMenu::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   DrawText("A Rabbit's Bizarre Adventure", 500.0f, 280.0f, 50, WHITE);

   DrawText("WASD - Move, Spacebar - Shoot", 500.0f , 400.0f, 40, WHITE);

   DrawText("Press ENTER to Start", 500.0f , 550.0f, 40, WHITE);

}

void LevelMenu::renderUI()
{}

void LevelMenu::shutdown()
{
   UnloadMusicStream(mGameState.bgm);
}