#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
   mTimer = 45.0f;
   mGameState.curSceneID = 3;
   mGameState.nextSceneID = 4;
   mGameState.reset_scene = false;
   mGameState.next_scene = false;

   mGameState.bgm = LoadMusicStream("assets/Journey To Ascend.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.shootSound = LoadSound("assets/Shoot29.wav");
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
      {mOrigin.x - 1800.0f, mOrigin.y - 200.0f}, // position
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
        {RIGHT, { 48, 49, 50, 51 }},
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
      mGameState.enemy[i].setSpeed(Entity::DEFAULT_SPEED *  0.65f);
      mGameState.enemy[i].setTexture("assets/monkey_enemy.png");
      mGameState.enemyQueue.push(i);
      mEnemyRespawn[i] = 3.0f + (i * 4.0f);
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
      ----------- SHIELD -----------
   */

   mGameState.shield = new Entity (
      {mOrigin.x - 1500.0f, mOrigin.y - 200.0f}, // position
      {baseShield, baseShield},             // scale
      "assets/shield.png",            // texture file address 
      NPC                                   // entity type
   );
   mGameState.shield->setColliderDimensions({
         mGameState.shield->getScale().x,
         mGameState.shield->getScale().y
      });
   mGameState.shield->setAIType(SHIELD);
   mGameState.shield->setSpeed(Entity::DEFAULT_SPEED *  0.40f);

   /*
      ----------- BOSS -----------
   */

   mGameState.boss = new Entity (
      {mOrigin.x - 1100.0f, mOrigin.y - 100.0f}, // position
      {125.0f, 125.0f},             // scale
      "assets/spacecraft.png",            // texture file address 
      NPC                                   // entity type
   );
   mGameState.boss->setColliderDimensions({
         mGameState.boss->getScale().x,
         mGameState.boss->getScale().y
      });
   mGameState.boss->setAIType(BOSS);
   mGameState.boss->setSpeed(Entity::DEFAULT_SPEED *  0.50f);
   mGameState.boss_lives = 10;

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.rabbit->getPosition(); // camera follows player
   mGameState.camera.offset = {mOrigin.x + 100.0f, mOrigin.y} ;  // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.6f;                                // default zoom
}

void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   if (mGameState.boss_death){
      mTimer -= deltaTime;
   }

   mGameState.rabbit->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.bullet, // collidable entities
      NUM_BULLETS              // col. entity count
   );

   //SHIELD LOGIC
   mGameState.shield->update(deltaTime, nullptr, mGameState.map, mGameState.bullet, NUM_BULLETS);

   // GROWING/SHRINKING SHIELD
   shieldPulse += 1.0f;
   float shieldOffset = baseShield + (60.0f * sin(shieldPulse/pulseSpeed));
   mGameState.shield->setScale({shieldOffset, shieldOffset});
   mGameState.shield->setColliderDimensions({
      mGameState.shield->getScale().x,
      mGameState.shield->getScale().y
   });

   //BOSS LOGIC
   mGameState.boss->update(deltaTime, nullptr, mGameState.map, mGameState.ammo, NUM_AMMO);
   if (mGameState.boss_lives <= 0){
      mGameState.boss_death = true;
      mGameState.boss->deactivate();
      bulletSpeed = -0.75f;
   }

   if (mGameState.boss->isCollidingPlayer()){
      printf("BOSS LOSING HP");
      PlaySound(mGameState.hitSound);
      mGameState.boss_lives--;
   }

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
               float _spawnY = GetRandomValue(25, 575);
               b.setPosition({ _spawnX, _spawnY});
               b.setMovement({ bulletSpeed, 0.0f });
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
            float _spawnX = (-200 + (100 * index));
            float _spawnY = GetRandomValue(50, 500);
            e.setPosition({_spawnX, _spawnY});
            mEnemyRespawn[index] = 20000.0f;
            e.activate();
         }
      }
      else{
         e.update(
            deltaTime,
            mGameState.rabbit,
            mGameState.map,
            mGameState.rabbit,
            1
         );

         if(e.isCollidingPlayer()){
            mGameState.cur_lives--;
            mGameState.reset_scene = true;
         }
      }
   }

   //AMMO LOGIC
   for (int i = 0; i < NUM_AMMO; i++){
      Entity& ammo = mGameState.ammo[i];
      if (ammo.isActive()){
         ammo.update(deltaTime, nullptr, nullptr, mGameState.enemy, NUM_ENEMIES);
         if (ammo.getPosition().x < mGameState.camera.target.x - 700.0f || 
            ammo.getPosition().x > mGameState.camera.target.x + 700.0f ||
            ammo.getPosition().y < mGameState.camera.target.y - 500.0f || 
            ammo.getPosition().y > mGameState.camera.target.y + 500.0f){
            ammo.deactivate();
         }

         if (ammo.isCollidingPlayer()){
            ammo.deactivate();
         }
      }

      if (!ammo.isActive()){
         mGameState.ammoQueue.push(i);
      }
   }  

   if (mGameState.rabbit->getPosition().x > 2300.0f){
      mGameState.next_scene = true;
   }

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.rabbit->render();
   mGameState.map->render();
   mGameState.shield->render();

   if (mGameState.boss->isActive()){
      mGameState.boss->render();
   }

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

void LevelC::renderUI()
{
    DrawText(TextFormat("Lives: %d", mGameState.cur_lives), 20, 20, 40, WHITE);

   int minutes = (int)mTimer / 60;
   int seconds = (int)mTimer % 60;

   DrawText(TextFormat("%02d:%02d", minutes, seconds), 20, 70, 40, YELLOW);
}

void LevelC::shutdown()
{
   delete mGameState.rabbit;
   delete mGameState.map;
   delete mGameState.shield;
   delete mGameState.boss;

   delete[] mGameState.bullet;
   delete[] mGameState.cannon;
   delete[] mGameState.enemy;
   delete[] mGameState.ammo;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.shootSound);
   UnloadSound(mGameState.hitSound);
}