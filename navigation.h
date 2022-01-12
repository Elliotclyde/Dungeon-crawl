#pragma once

#include "main.h"

void moveUser(directions dir, float dist, UserData *currentUserData);
void left(float dist, UserData *currentUserData);
void right(float dist,UserData *currentUserData);
void up(float dist,UserData *currentUserData);
void down(float dist,UserData *currentUserData);

directions getDirectionFromKeys(KeysDown *currentKeysDown);
directions getOppositeDirection(directions);