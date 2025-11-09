#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
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
      "assets/char_spritesheet.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 4 },                                 // atlas dimensions
      rabbitAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.rabbit->setJumpingPower(600.0f);
   mGameState.rabbit->setColliderDimensions({
      mGameState.rabbit->getScale().x / 3.5f,
      mGameState.rabbit->getScale().y / 3.0f
   });
   mGameState.rabbit->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    /*
        ----------- monkey -----------
    */
    std::map<Direction, std::vector<int>> monkeyAnimationAtlas = {
        {LEFT,  { 17, 18, 19, 20 }},
        {RIGHT, { 49, 50, 51, 52 }},
    };

    // @see dyru.itch.io/pixel-monkey-template
    mGameState.monkey = new Entity(
        {mOrigin.x - 400.0f, mOrigin.y - 100.0f}, // position
        {100.0f, 100.0f},                       // scale
        "assets/loose_sprites.png",                // texture file address
        ATLAS,                                  // single image or atlas?
        { 8, 8 },                       // atlas dimensions
        monkeyAnimationAtlas,                    // actual atlas
        NPC                                     // entity type
    );

    mGameState.monkey->setAIType(FOLLOWER);
    mGameState.monkey->setAIState(IDLE);
    mGameState.monkey->setSpeed(Entity::DEFAULT_SPEED * 0.50f);

    mGameState.monkey->setColliderDimensions({
        mGameState.monkey->getScale().x / 2.0f,
        mGameState.monkey->getScale().y
    });

    mGameState.monkey->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.monkey->setDirection(LEFT);
    mGameState.monkey->render(); // calling render once at the beginning to switch monkey's direction

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.rabbit->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.rabbit->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.monkey, // collidable entities
      1               // col. entity count
   );

   mGameState.monkey->update(
      deltaTime,      // delta time / fixed timestep
      mGameState.rabbit,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   Vector2 currentPlayerPosition = { mGameState.rabbit->getPosition().x, mOrigin.y };

   if (mGameState.rabbit->getPosition().y > 800.0f){
      PlaySound(mGameState.hitSound);
      mGameState.reset_scene = true;
      mGameState.cur_lives--;
   }

   if (mGameState.rabbit->isCollidingEnemy()){
      PlaySound(mGameState.hitSound);
      mGameState.reset_scene = true;
      mGameState.cur_lives--;
   }

   if (mGameState.rabbit->getPosition().x > 1380.0f){
      PlaySound(mGameState.flagSound);
      mGameState.next_scene = true;
   }

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.rabbit->render();
   mGameState.monkey->render();
   mGameState.map->render();
}

void LevelA::renderUI()
{
    DrawText(TextFormat("Lives: %d", mGameState.cur_lives), 20, 20, 40, WHITE);
}

void LevelA::shutdown()
{
   delete mGameState.rabbit;
   delete mGameState.map;
   delete mGameState.monkey;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}