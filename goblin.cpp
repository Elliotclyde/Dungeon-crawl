#include "goblin.h"
#include <math.h>
#include <stdlib.h>
#include "collision.h"

const float GOBLIN_MOVE_SPEED = 0.1;
const float GOBLIN_BOUNCE_MOVE_SPEED = 0.2;
#define M_PI 3.14159265358979323846
const int GOBLIN_BOUNCE_TIME = 30;
const int GOBLIN_REDIRECT_TIME = 2000;

Goblin::Goblin()
{
    exists = false;
}

//Initializes variables
Goblin::Goblin(int inputX, int inputY)
{
    x = inputX;
    y = inputY;
    direction = fmod((double)rand(), (2 * M_PI));
    isBouncing = false;
    exists = true;
    timeSinceLastBounce = 0;
    timeSinceLastRedirect = 0;
}
//Deallocates memory
Goblin::~Goblin()
{
    x = 0;
    y = 0;
    direction = 0;
    isBouncing = false;
    exists = false;
    timeSinceLastBounce = 0;
    timeSinceLastRedirect = 0;
}
int Goblin::getX()
{
    return x;
}
int Goblin::getY()
{
    return y;
}
bool Goblin::doesExist()
{
    return exists;
}
void Goblin::update(BackgroundTile (*currentTileMap)[TILE_MAP_WIDTH][TILE_MAP_HEIGHT], UserData *currentUserData, float delta)
{

    coord gobCoords = {};
    gobCoords.x = x;
    gobCoords.y = y;
    float speed = GOBLIN_MOVE_SPEED;
    if (!isBouncing && checkMapCollision(currentTileMap, &gobCoords))
    {
        bool xIsclosestToEdge = abs(SCREEN_TILE_SIZE / 2 - (x % SCREEN_TILE_SIZE)) > abs(SCREEN_TILE_SIZE / 2 - (y % SCREEN_TILE_SIZE));
        if (xIsclosestToEdge)
        {
            x = round((x + SCREEN_TILE_SIZE / 2) / SCREEN_TILE_SIZE) * SCREEN_TILE_SIZE;
        }
        else
        {
            y = round((y + SCREEN_TILE_SIZE / 2) / SCREEN_TILE_SIZE) * SCREEN_TILE_SIZE;
        }

        if (fmod((double)rand(), 10) < 7) // 3 in 10 chance of going toward centre of screen
        {
            direction = direction + M_PI; // Reverses direction
        }
        else
        {
            direction = atan2(SCREEN_WIDTH / 2 - x, SCREEN_HEIGHT / 2 - y); // centre of screen
        }

        isBouncing = true;
    }
    if (isBouncing)
    {
        speed = GOBLIN_BOUNCE_MOVE_SPEED;
        timeSinceLastBounce += delta;
        if (timeSinceLastBounce >= GOBLIN_BOUNCE_TIME)
        {
            isBouncing = false;
            timeSinceLastBounce = 0;
        }
    }
    timeSinceLastRedirect += delta;
    if (timeSinceLastRedirect >= GOBLIN_REDIRECT_TIME)
    {
        if (fmod((double)rand(), 10) < 0.7)
        {
            direction = atan2(currentUserData->x - x, currentUserData->y - y);
        }
        else
        {
            direction = fmod((double)rand(), (2 * M_PI));
            isBouncing = true;
        }
        timeSinceLastRedirect = 0;
    }
    x = x + sin(direction) * speed * delta;
    y = y + cos(direction) * speed * delta;
}
