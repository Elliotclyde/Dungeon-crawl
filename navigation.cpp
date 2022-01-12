#include "navigation.h"
#include <math.h>

void moveUser(directions dir, float dist, UserData *currentUserData)
{
    switch (dir)
    {
    case USER_DIR_NONE:
        break;
    case USER_DIR_UP:
        up(dist, currentUserData);
        break;
    case USER_DIR_UP_RIGHT:
        up(dist * 0.8, currentUserData);
        right(dist * 0.8, currentUserData);
        break;
    case USER_DIR_RIGHT:
        right(dist, currentUserData);
        break;
    case USER_DIR_DOWN_RIGHT:
        right(dist * 0.8, currentUserData);
        down(dist * 0.8, currentUserData);
        break;
    case USER_DIR_DOWN:
        down(dist, currentUserData);
        break;
    case USER_DIR_DOWN_LEFT:
        left(dist * 0.8, currentUserData);
        down(dist * 0.8, currentUserData);
        break;
    case USER_DIR_LEFT:
        left(dist, currentUserData);
        break;
    case USER_DIR_UP_LEFT:
        left(dist * 0.8, currentUserData);
        up(dist * 0.8, currentUserData);
        break;
    }
}

void left(float dist, UserData *currentUserData)
{
    currentUserData->x = currentUserData->x - round(dist);
}
void right(float dist, UserData *currentUserData)
{
    currentUserData->x = currentUserData->x + round(dist);
}
void up(float dist, UserData *currentUserData)
{
    currentUserData->y = currentUserData->y - round(dist);
}
void down(float dist, UserData *currentUserData)
{
    currentUserData->y = currentUserData->y + round(dist);
}

directions getDirectionFromKeys(KeysDown *currentKeysDown)
{
    //First check for none
    if (!currentKeysDown->up && !currentKeysDown->right && !currentKeysDown->down && !currentKeysDown->left)
    {
        return USER_DIR_NONE;
    }
    //Then diagonals
    if (currentKeysDown->up && currentKeysDown->right)
    {
        return USER_DIR_UP_RIGHT;
    }
    if (currentKeysDown->down && currentKeysDown->right)
    {
        return USER_DIR_DOWN_RIGHT;
    }
    if (currentKeysDown->down && currentKeysDown->left)
    {
        return USER_DIR_DOWN_LEFT;
    }
    if (currentKeysDown->up && currentKeysDown->left)
    {
        return USER_DIR_UP_LEFT;
    }
    //Then up down left right

    if (currentKeysDown->up)
    {
        return USER_DIR_UP;
    }
    if (currentKeysDown->right)
    {
        return USER_DIR_RIGHT;
    }
    if (currentKeysDown->down)
    {
        return USER_DIR_DOWN;
    }
    if (currentKeysDown->left)
    {
        return USER_DIR_LEFT;
    }
    return USER_DIR_NONE;
}

directions getOppositeDirection(directions dir)
{
    switch (dir)
    {
    case USER_DIR_NONE:
        return USER_DIR_NONE;
    case USER_DIR_LEFT:
        return USER_DIR_RIGHT;
    case USER_DIR_RIGHT:
        return USER_DIR_LEFT;
    case USER_DIR_UP:
        return USER_DIR_DOWN;
    case USER_DIR_DOWN:
        return USER_DIR_UP;
    case USER_DIR_DOWN_RIGHT:
        return USER_DIR_UP_LEFT;
    case USER_DIR_UP_LEFT:
        return USER_DIR_DOWN_RIGHT;
    case USER_DIR_DOWN_LEFT:
        return USER_DIR_UP_RIGHT;
    case USER_DIR_UP_RIGHT:
        return USER_DIR_DOWN_LEFT;
    default:
        return USER_DIR_NONE;
    }
}
