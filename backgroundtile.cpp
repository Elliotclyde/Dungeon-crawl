#include "backgroundtile.h"

BackgroundTile::BackgroundTile(){
}
BackgroundTile::BackgroundTile(BackGroundTileType inputType){
    type = inputType;
    switch (inputType)
    {
    case FLOOR1:
        image = {};
        image.x = 2;
        image.y = 3;
        walkable = true;
        break;

    case FLOOR2:
        image = {};
        image.x = 2;
        image.y = 1;
        walkable = true;
        break;
    case FLOOR3:
        image = {};
        image.x = 3;
        image.y = 1;
        walkable = true;
       break;
       case FLOOR4:
        image = {};
        image.x = 5;
        image.y = 2;
        walkable = true;
       break;
     case WALL1:
        image = {};
        image.x = 6;
        image.y = 4;
        walkable = false;
        break;
        case WALL2:
        image = {};
        image.x = 6;
        image.y = 5;
        walkable = false;
        break;
        case WALL3:
        image = {};
        image.x = 8;
        image.y = 5;
        walkable = false;
        break;
    default:
        printf("Incorrect tiletype");
        break;
    }
}

BackgroundTile::~BackgroundTile()
{
    //Deallocate
    image = {};

}
bool BackgroundTile::isWalkable(){
    return walkable;
}

imageTile BackgroundTile::getImageTile(){
    return image;
}