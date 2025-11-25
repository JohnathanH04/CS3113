#include "Entity.h"
#include <queue>

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *rabbit;
    Entity *bullet;
    Entity *cannon;
    Entity *enemy;
    Entity *ammo;
    Entity* shield;
    Entity* boss;
    Map *map;

    Music bgm;
    Sound shootSound;
    Sound hitSound;
    Sound flagSound;

    Camera2D camera;

    // hardcode progression
    int nextSceneID;
    int curSceneID;
    // track cur lives
    int cur_lives;
    // boolean checkers
    bool reset_scene = false;
    bool next_scene = false;
    //used on level C to set winner to true if player wins
    bool levelComplete = false;

    // for boss scenario
    int boss_lives;
    bool boss_death = false;

    //bullet logic
    std::queue<int> bulletQueue;

    //enemy logic
    std::queue<int> enemyQueue;

    //ammo logic
    std::queue<int> ammoQueue;
};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    // RENDER UI TO SHOW NUMBER OF LIVES
    virtual void renderUI() = 0;
    virtual void shutdown() = 0;
    
    GameState&   getState()                { return mGameState; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }
    void        setLives(int value)        { mGameState.cur_lives = value; }
    void        setBossStatus (bool value)     { mGameState.boss_death = value; }
};

#endif