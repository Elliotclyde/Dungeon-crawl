#pragma once

#include "main.h"
#include "backgroundtile.h"
#include  "goblin.h" 

coord getTopLeftBoxForUser(coord *inputCoords);
bool checkMapCollision(BackgroundTile (*currentTileMap)[TILE_MAP_WIDTH][TILE_MAP_HEIGHT], coord *inputCoords);
int checkItemCollision(Item (*currentItem)[MAX_ITEMS], UserData *currentUserData);
int checkGoblinCollision(Goblin (*currentGoblins)[MAX_GOBLINS], UserData *currentUserData);