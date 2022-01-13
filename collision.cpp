
#include <cmath>
#include "collision.h"

const int marginOfError = SCREEN_TILE_SIZE/10;

bool checkMapCollision(BackgroundTile (*currentTileMap)[TILE_MAP_WIDTH][TILE_MAP_HEIGHT], coord *inputCoords)
{
    if(inputCoords->x % SCREEN_TILE_SIZE == 0 && inputCoords->y % SCREEN_TILE_SIZE == 0){
        return false;
    }

    coord coords = getTopLeftBoxForUser(inputCoords);
    // check if out of bounds.
    if (coords.x < 0 ||
        coords.y < 0 ||
        coords.x >= (sizeof(currentTileMap) / sizeof(currentTileMap[0])) - 1 ||
        coords.y >= (sizeof(currentTileMap[0]) / sizeof(currentTileMap[0][0])) - 1)
    {
        return true;
    }
    if (!currentTileMap[0][coords.x][coords.y].isWalkable())
    {
        return true;
    }
    if (!currentTileMap[0][coords.x + 1][coords.y].isWalkable())
    {
        return true;
    }
    if (!currentTileMap[0][coords.x][coords.y + 1].isWalkable())
    {
        return true;
    }
    if (!currentTileMap[0][coords.x + 1][coords.y + 1].isWalkable())
    {
        return true;
    }
    return false;
}

//Returns the index of the chest you've hit
int checkItemCollision(Item (*currentItems)[MAX_ITEMS], UserData *currentUserData)
{
    int result = -1;
    coord userCoords = {};
    userCoords.x = currentUserData->x;
    userCoords.y = currentUserData->y;
    coord coords = getTopLeftBoxForUser(&userCoords);

    for (int i = 0; i < sizeof(currentItems[0]) / sizeof(currentItems[0][0]); i++)
    {
        if (currentItems[0][i].exists)
        {
            if (currentItems[0][i].x == coords.x && currentItems[0][i].y == coords.y)
            {
                result = i;
                break;
            }
            if (currentItems[0][i].x == coords.x + 1 && currentItems[0][i].y == coords.y)
            {
                result = i;
                break;
            }
            if (currentItems[0][i].x == coords.x && currentItems[0][i].y == coords.y + 1)
            {
                result = i;
                break;
            }
            if (currentItems[0][i].x == coords.x + 1 && currentItems[0][i].y == coords.y + 1)
            {
                result = i;
                break;
            }
        }
    }
    return result;
}

coord getTopLeftBoxForUser(coord *inputCoords)
{
    int boxX = floor(inputCoords->x / static_cast<float>(SCREEN_TILE_SIZE));
    int boxY = floor(inputCoords->y / static_cast<float>(SCREEN_TILE_SIZE));
    coord coords = {};
    coords.x = boxX;
    coords.y = boxY;
    return coords;
}

int checkGoblinCollision(Goblin (*currentGoblins)[MAX_GOBLINS], UserData *currentUserData)
{
    int result = -1;

    for (int i = 0; i < sizeof(currentGoblins[0]) / sizeof(currentGoblins[0][0]); i++)
    {
        if (currentGoblins[0][i].doesExist())
        {
            int xDist = std::abs(currentGoblins[0][i].getX() - currentUserData->x);
            int yDist = std::abs(currentGoblins[0][i].getY() - currentUserData->y);
            // Multiply the distance by 0.7 to give some leniency around avoiding gobins.
            float distanceCheck = (static_cast<float>(SCREEN_TILE_SIZE) * 0.7);
            if (xDist <= distanceCheck && yDist <= distanceCheck)
            {
                result = i;
                break;
            }
        }
    }
    return result;
}