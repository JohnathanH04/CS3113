#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
   mTimer = 45.0f;
   mGameState.curSceneID = 2;
   mGameState.nextSceneID = 3;
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
      {mOrigin.x - 2000.0f, mOrigin.y - 200.0f}, // position
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
      ----------- CANNONS -----------
   */

   mGameState.cannon = new Entity[NUM_CANNON];

   for (int i = 0; i < NUM_CANNON; i++){
      mGameState.cannon[i] = Entity(
         {2000.0f, 50.0f + (75.0f * i)},
         {75.0f, 75.0f},
         "assets/cannon.png",
         CANNON
      );
      mGameState.cannon[i].setTexture("assets/cannon.png");
      mGameState.cannon[i].activate();
   }

   /*
      ----------- ENEMIES -----------
   */

   std::map<Direction, std::vector<int>> enemyAnimationAtlas = {
        {LEFT,  { 16, 17, 18, 19 }},
        {RIGHT, { 48, 59, 50, 51 }},
      //   {DOWN, { 0, 1, 2, 3 }},
      //   {UP, { 32, 33, 34, 35 }},

   };

   mGameState.enemy = new Entity[NUM_ENEMIES];
   
   for (int i = 0; i < NUM_ENEMIES; i++){
      mGameState.enemy[i] = Entity(
         {0,0},
         {100.0f, 100.0f},
         "assets/monkey_enemy.png",
         ATLAS,
         { 8, 8 },
         enemyAnimationAtlas,
         NPC
      );
      mGameState.enemy[i].deactivate();
      mGameState.enemy[i].setColliderDimensions({
         mGameState.enemy[i].getScale().x / 4.0f,
         mGameState.enemy[i].getScale().y / 2.0f
      });
      mGameState.enemy[i].setAIType(FOLLOWER);
      mGameState.enemy[i].setAIState(IDLE);
      mGameState.enemy[i].setSpeed(Entity::DEFAULT_SPEED *  0.70f);
      mGameState.enemy[i].setTexture("assets/monkey_enemy.png");
      mGameState.enemyQueue.push(i);
      mEnemyRespawn[i] = 3.0f + (i * 2.0f);
   }

   /*
      ----------- AMMO -----------
   */
   mGameState.ammo = new Entity[NUM_AMMO];

   for (int i = 0; i < NUM_AMMO; i++){
      mGameState.ammo[i] = Entity(
         {0, 0},
         {40, 40},
         "assets/sprite_carotte0.png",
         AMMO
      );
      mGameState.ammo[i].deactivate();
      mGameState.ammo[i].setTexture("assets/sprite_carotte0.png");
      mGameState.ammoQueue.push(i);
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

void LevelB::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);
   mTimer -= deltaTime;

   if (mTimer <= 0.0f){
      mGameState.reset_scene = true;
   }

   mGameState.rabbit->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.bullet, // collidable entities
      NUM_BULLETS              // col. entity count
   );

   Vector2 currentPlayerPosition = { mGameState.rabbit->getPosition().x, mGameState.rabbit->getPosition().y };

   //BULLET LOGIC
   for (int i = 0; i < NUM_BULLETS; i++){
      int index = mGameState.bulletQueue.front();
      mGameState.bulletQueue.pop();
      mGameState.bulletQueue.push(index);

      Entity& b = mGameState.bullet[index];
      if (mGameState.rabbit->getPosition().x < 1850.0f){
         if (!b.isActive()){
            mBulletRespawn[index] -= deltaTime;
            if (mBulletRespawn[index] <= 0.0f){
               float _spawnX = 2000.0f;
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
               mGameState.cur_lives--;
            }
            if (b.getPosition().x < mGameState.camera.target.x - 500.0f){
               b.deactivate();
            }
         }
      }
      else{
         b.deactivate();
      }
   }

   //ENEMY LOGIC
   for (int i  = 0; i < NUM_ENEMIES; i++){
      int index = mGameState.enemyQueue.front();
      mGameState.enemyQueue.pop();
      mGameState.enemyQueue.push(index);

      Entity& e = mGameState.enemy[index];

      if (!e.isActive()){
         mEnemyRespawn[index] -= deltaTime;
         if (mEnemyRespawn[index] <= 0.0f){
            float _spawnX = (700 + (index * 200));
            float _spawnY = GetRandomValue(50, 500);
            e.activate();
         }
      }
      else{
         e.update(
            deltaTime,
            mGameState.rabbit,
            mGameState.map,
            nullptr,
            0
         );
      }
   }

   for (int i = 0; i < NUM_AMMO; i++){
      Entity& ammo = mGameState.ammo[i];
      if (ammo.isActive()){
         ammo.update(deltaTime, nullptr, nullptr, mGameState.enemy, NUM_ENEMIES);
         if (ammo.getPosition().x < mGameState.camera.target.x - 500.0f || 
            ammo.getPosition().x > mGameState.camera.target.x + 500.0f ||
            ammo.getPosition().y < mGameState.camera.target.y - 500.0f || 
            ammo.getPosition().y > mGameState.camera.target.y + 500.0f){
            ammo.deactivate();
            mGameState.ammoQueue.push(i);
         }
      }
   }  


   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelB::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.rabbit->render();
   mGameState.map->render();

   for (int i = 0; i < NUM_BULLETS; i++){
      if (mGameState.bullet[i].isActive()){
         mGameState.bullet[i].render();
      }
   }
   for (int i = 0; i < NUM_CANNON; i++){
      mGameState.cannon[i].render();
   }
   for (int i = 0; i < NUM_ENEMIES; i++){
      if (mGameState.enemy[i].isActive()){
         mGameState.enemy[i].render();
      }
   }
   for (int i = 0; i < NUM_AMMO; i++){
      if (mGameState.ammo[i].isActive()){
         mGameState.ammo[i].render();
      }
   }
}

void LevelB::renderUI()
{
    DrawText(TextFormat("Lives: %d", mGameState.cur_lives), 20, 20, 40, WHITE);

   int minutes = (int)mTimer / 60;
   int seconds = (int)mTimer % 60;

   DrawText(TextFormat("%02d:%02d", minutes, seconds), 20, 70, 40, YELLOW);
}

void LevelB::shutdown()
{
   delete mGameState.rabbit;
   delete mGameState.map;
   delete[] mGameState.bullet;
   delete[] mGameState.cannon;
   delete[] mGameState.enemy;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}