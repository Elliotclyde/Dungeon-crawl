#include "test.h"

void runTests()
{
    printf("Test mode ON!\n");
    printf("Running tests . . .\n");
    // assert(true, "True should pass");
    // assert(false, "False should fail");
    backgroundTileTests();
}

void backgroundTileTests(){
    BackgroundTile floor = BackgroundTile(FLOOR1);
    assert(floor.isWalkable(),"Floor should be walkable");
    BackgroundTile wall = BackgroundTile(WALL1);
    assert(!wall.isWalkable(),"Wall should not be walkable");
}

bool assert(bool condition, char message[])
{
    if (condition)
    {
        printf("test passed: %s\n", message);
    }
    else
    {
        printf("test FAILED: %s\n", message);
    }
}