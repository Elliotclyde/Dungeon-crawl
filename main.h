#pragma once

const bool TEST_MODE_ON = true;

//Screen dimension constants

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int SCREEN_TILES_WIDTH = 16; // How many tiles are the width of the screen
const int SCREEN_TILE_SIZE = SCREEN_WIDTH / SCREEN_TILES_WIDTH;
// minus 1 height to allow for top bar
const int TILE_MAP_WIDTH = SCREEN_WIDTH / SCREEN_TILE_SIZE;
const int TILE_MAP_HEIGHT = SCREEN_HEIGHT / SCREEN_TILE_SIZE - 1;


enum userState
{
    CONTROLLABLE,
    BOUNCING,
    DEAD
};

enum directions
{
    USER_DIR_NONE,
    USER_DIR_UP,
    USER_DIR_UP_RIGHT,
    USER_DIR_RIGHT,
    USER_DIR_DOWN_RIGHT,
    USER_DIR_DOWN,
    USER_DIR_DOWN_LEFT,
    USER_DIR_LEFT,
    USER_DIR_UP_LEFT,
};

enum GameState
{
    MENU,
    PLAYING,
    RESTART,
    WIN
};

// Holds the position of the user on the screen
struct UserData
{
    int x;
    int y;
    userState state;
    directions direction;
    int timeSinceBounce;
    int timeSinceDeath;
};

struct KeysDown
{
    bool up;
    bool right;
    bool left;
    bool down;
};

struct coord
{
    int x;
    int y;
};

enum ItemType{
    CHEST,
    KEY
};

struct Item
{
    ItemType type;
    int x;
    int y;
    bool exists;
};

const int MAX_ITEMS = 25;
const int MAX_GOBLINS = 10;
