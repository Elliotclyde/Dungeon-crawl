#pragma once

#include <stdio.h>

// Imagetiles hold the coordinates of the sprite they represent
struct imageTile
{
    int x;
    int y;
};

enum BackGroundTileType{
    FLOOR1,
    FLOOR2,
    FLOOR3,
    FLOOR4,
    WALL1,
    WALL2,
    WALL3
};

class BackgroundTile{
    public:
   BackgroundTile();
       //Initializes variables
    BackgroundTile(BackGroundTileType inputType);
    //Deallocates memory
    ~BackgroundTile();
    //Can be walked through
    bool isWalkable();
    //Get the image
    imageTile getImageTile();

    private:

    bool walkable;
    BackGroundTileType type; 
    imageTile image;
};