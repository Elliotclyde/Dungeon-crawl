#pragma once

#include "main.h"
#include "backgroundtile.h"

class Goblin
{
public:
    Goblin();

    //Initializes variables
    Goblin(int x, int y);
    //Deallocates memory
    ~Goblin();
    int getX();
    int getY();
    void update(BackgroundTile (*currentTileMap)[TILE_MAP_WIDTH][TILE_MAP_HEIGHT], UserData *currentUserData,float delta);
    bool doesExist();

private:
    bool exists;
    int x;
    int y;
    double direction; // Radian value of current direction
    bool isBouncing; // Whether the goblin is currently bouncing off a wall
    int timeSinceLastBounce;
    int timeSinceLastRedirect;
};