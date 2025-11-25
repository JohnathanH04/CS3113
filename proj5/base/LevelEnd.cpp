#include "LevelEnd.h"

LevelEnd::LevelEnd()                                      : Scene { {0.0f}, nullptr   } {}
LevelEnd::LevelEnd(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelEnd::~LevelEnd() { shutdown(); }

void LevelEnd::initialise()
{
   mGameState.curSceneID = 4;
   mGameState.nextSceneID = 0;
   mGameState.reset_scene = false;
   mGameState.next_scene = false;

   mGameState.bgm = LoadMusicStream("assets/Journey To Ascend.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   float sizeRatio  = 48.0f / 64.0f;
}

void LevelEnd::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

}

void LevelEnd::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   DrawText("YOU BEAT THE GAME.", 600.0f, 200.0f, 40, WHITE);

   DrawText("PRESS R TO RESTART OR Q TO QUIT.", 600.0f, 500.0f, 40, WHITE);
}

void LevelEnd::renderUI(){}

void LevelEnd::shutdown()
{
   UnloadMusicStream(mGameState.bgm);
}