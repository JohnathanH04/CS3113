#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
   mGameState.curSceneID = 1;
   mGameState.nextSceneID = 2;
   mGameState.reset_scene = false;
   mGameState.next_scene = false;

   mGameState.bgm = LoadMusicStream("assets/Journey To Ascend.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/Jump1.wav");
   mGameState.hitSound = LoadSound("assets/Hit11.wav");
   mGameState.flagSound = LoadSound("assets/Pickup9.wav");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/spritesheet-tiles-default.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      18, 18,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
  
   std::map<Direction, std::vector<int>> rabbitAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3 }},
      {LEFT,  {  8,  9, 10, 11 }},
      {UP,    {  4,  5,  6,  7 }},
      {RIGHT, { 12, 13, 14, 15 }},
   };

   float sizeRatio  = 48.0f / 64.0f;


   mGameState.rabbit = new Entity(
      {mOrigin.x - 920.0f, mOrigin.y - 200.0f}, // position
      {250.0f * sizeRatio, 250.0f},             // scale
      "assets/char_spritesheet.png",            // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      rabbitAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.rabbit->setJumpingPower(600.0f);
   mGameState.rabbit->setColliderDimensions({
      mGameState.rabbit->getScale().x / 10.5f,
      mGameState.rabbit->getScale().y / 10.0f
   });
   
   mGameState.rabbit->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});


   /*
      ----------- BULLETS -----------
   */

   mGameState.bullet = new Entity[NUM_BULLETS];
   
   for (int i = 0; i < NUM_BULLETS; i++){
      mGameState.bullet[i] = Entity(
         {0,0},
         {30.0f, 30.0f},
         "assets/1.png",
         BULLET
      );
      mGameState.bullet[i].setTexture("assets/1.png");


      mGameState.bullet[i].deactivate();
      mGameState.bullet[i].setColliderDimensions({
         mGameState.bullet[i].getScale().x / 5.0f, 
         mGameState.bullet[i].getScale().y / 5.0f
      });
      mGameState.bulletQueue.push(i);
      mBulletRespawn[i] = GetRandomValue(60,1080) / 60.0f;
   }

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.rabbit->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.rabbit->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.bullet, // collidable entities
      NUM_BULLETS              // col. entity count
   );

   Vector2 currentPlayerPosition = { mGameState.rabbit->getPosition().x, mGameState.rabbit->getPosition().y };

   for (int i = 0; i < NUM_BULLETS; i++){
      int index = mGameState.bulletQueue.front();
      mGameState.bulletQueue.pop();
      mGameState.bulletQueue.push(index);

      Entity& b = mGameState.bullet[index];

      if (!b.isActive()){
         mBulletRespawn[index] -= deltaTime;
         if (mBulletRespawn[index] <= 0.0f){
            float _spawnX = 1380.0f;
            float _spawnY = GetRandomValue(50, 500);
            b.setPosition({ _spawnX, _spawnY});
            b.setMovement({ -0.75f, 0.0f });
            b.activate();
            mBulletRespawn[index] = GetRandomValue(60, 720) / 60.0f;
         }
      }
      else{
         b.update(deltaTime, mGameState.rabbit, mGameState.map, mGameState.rabbit, 1);
         if (b.isCollidingPlayer()){
            b.deactivate();
         }
         if (b.getPosition().x < -200.0f){
            b.deactivate();
         }
      }
   }

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.rabbit->render();
   mGameState.map->render();

   for (int i = 0; i < NUM_BULLETS; i++){
      if (mGameState.bullet[i].isActive()){
         mGameState.bullet[i].render();
      }
   }
}

void LevelC::renderUI()
{
    DrawText(TextFormat("Lives: %d", mGameState.cur_lives), 20, 20, 40, WHITE);
}

void LevelC::shutdown()
{
   delete mGameState.rabbit;
   delete mGameState.map;
   delete[] mGameState.bullet;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}