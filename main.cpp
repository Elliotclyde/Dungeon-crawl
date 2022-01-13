/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>


#include "main.h"
#include "goblin.h"
#include "ltexture.h"
#include "navigation.h"
#include "collision.h"
#include "backgroundtile.h"
#include "test.h"

//TODO:
// -Animate character
// -potentially add the music from: https://pixabay.com/music/main-title-medieval-horror-music-theme-2916/

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Draws elements to the screen
void draw();

//Draws elements to the screen
void update();

void drawMenuScreen();

void mousePress(SDL_MouseButtonEvent &b);

//The window renderer
SDL_Renderer *gRenderer = NULL;

//The window we'll be rendering to
SDL_Window *gWindow = NULL;

/* * * *  * * *  * * *  Game assets * *  * * *  * * *  * * *  * * *  * * * */

// The medieval font, and some textures where we'll hold messages to the user
TTF_Font *medFont = NULL;
LTexture titleTexture;
LTexture scoreTexture;
LTexture restartTexture;
LTexture winTexture;
LTexture playAgainTexture;
LTexture startGameTexture;
LTexture highScoreTexture;

// The background for the button
LTexture buttonBack;

// Map sheet holds the tiles for the map
LTexture mapSheet;
const int MAP_SHEET_WIDTH = 144;
const int MAP_SHEET_HEIGHT = 128;

// Char sheet holds the tiles for the characters
LTexture charSheet;
const int CHAR_SHEET_WIDTH = 448;
const int CHAR_SHEET_HEIGHT = 144;

// These are made up of 16px16p tiles
const int TILE_SIZE = 16;
// The measure of how the sprites get scaled up
const float SCALE_UP = static_cast<float>(SCREEN_TILE_SIZE) / static_cast<float>(TILE_SIZE);

// These are 2d arrays of rectangles that will clip a tile from the spritesheets
SDL_Rect mapClips[MAP_SHEET_WIDTH / TILE_SIZE][MAP_SHEET_HEIGHT / TILE_SIZE];
SDL_Rect charClips[CHAR_SHEET_WIDTH / TILE_SIZE][CHAR_SHEET_HEIGHT / TILE_SIZE];

/* * * *  * * *  * * *  Game data and states  * *  * * *  * * *  * * *  * * *  * * * */

/* * * *  * * *  * * *  These are all pieces of data the update  * *  * * *  * * *  * * *  * * *  * * * */
/* * * *  * * *  * * *  function mutates and the draw function draws  * * *  * * *  * * *  * * *  * * * */

// 2d array of floor and wall tiles - these contain the logic of the tiles (whether they're workable)
BackgroundTile tileMap[TILE_MAP_WIDTH][TILE_MAP_HEIGHT];

GameState gameState;
UserData userData;
KeysDown keysDown;

const float moveSpeed = 0.2;
const int bounceTime = 50;
const float bounceSpeed = 0.1;
const int timeToShowRestart = 1000;

Item items[MAX_ITEMS];
Goblin goblins[MAX_GOBLINS];
int score;
int scoreThisLife;

Uint32 lastTick;

int level;
const int MAX_LEVEL = 5;

/* * * *  * * *  * * *  Sets up SDL including font and image libraries  * * *  * * *  * * *  * * * */
bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        //Create window
        gWindow = SDL_CreateWindow("Dungeon Crawl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
                    success = false;
                }

                //Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool setUpGame()
{
    level = 1;
    score = 0;
    scoreThisLife = 0;

    SDL_Color textColor = {255, 255, 255};
    scoreTexture.loadFromRenderedText("Score: " + std::to_string(score), textColor, medFont, gRenderer);
}

/* * * *  * * *  * * *  Reads from map file and sets up game data  * * *  * * *  * * *  * * * */
/* * * *  * * *  * * *  This function gets called when the game is restarted  *  * * *  * * * */
bool setUpLevel()
{

    std::fstream mapFile;
    std::string levelString = std::to_string(level);
    mapFile.open("maps/map" + levelString + ".txt", std::ios::in);
    if (!mapFile)
    {
        printf("Could not load map file!");
    }
    else
    {

        for (int i = 0; i < sizeof(items) / sizeof(items[0]); i++)
        {
            items[i] = {};
        }
        for (int i = 0; i < sizeof(goblins) / sizeof(goblins[0]); i++)
        {
            delete &goblins[i];
        }

        int itemIndex = 0;
        int goblinIndex = 0;

        userData = {};
        userData.x = 0;
        userData.y = 0;
        userData.state = CONTROLLABLE;
        userData.direction = USER_DIR_NONE;
        userData.timeSinceBounce = 0;
        userData.timeSinceDeath = 0;

        gameState = PLAYING;

        scoreThisLife = 0;

        int x = 0;
        int y = 0;
        std::string line;
        while (getline(mapFile, line))
        {
            for (int i = 0; i < line.size(); i++)
            {
                switch (line[i])
                {
                case 'f':
                    tileMap[x][y] = BackgroundTile(FLOOR1);
                    break;
                case 'd':
                    tileMap[x][y] = BackgroundTile(FLOOR2);
                    break;
                case 's':
                    tileMap[x][y] = BackgroundTile(FLOOR3);
                    break;
                case 'a':
                    tileMap[x][y] = BackgroundTile(FLOOR4);
                    break;
                case 'w':
                    tileMap[x][y] = BackgroundTile(WALL1);
                    break;
                case 'W':
                    tileMap[x][y] = BackgroundTile(WALL2);
                    break;
                case 'q':
                    tileMap[x][y] = BackgroundTile(WALL3);
                    break;
                case 'p':
                    tileMap[x][y] = BackgroundTile(FLOOR1);
                    userData.x = x * SCREEN_TILE_SIZE;
                    userData.y = y * SCREEN_TILE_SIZE;
                    break;
                case 'c':
                    tileMap[x][y] = BackgroundTile(FLOOR1);
                    items[itemIndex] = {};
                    items[itemIndex].type = CHEST;
                    items[itemIndex].x = x;
                    items[itemIndex].y = y;
                    items[itemIndex].exists = true;
                    itemIndex++;
                    break;
                case 'k':
                    tileMap[x][y] = BackgroundTile(FLOOR1);
                    items[itemIndex] = {};
                    items[itemIndex].type = KEY;
                    items[itemIndex].x = x;
                    items[itemIndex].y = y;
                    items[itemIndex].exists = true;
                    itemIndex++;
                    break;
                case 'g':
                    tileMap[x][y] = BackgroundTile(FLOOR1);
                    goblins[goblinIndex] = Goblin(x * SCREEN_TILE_SIZE, y * SCREEN_TILE_SIZE);
                    goblinIndex++;
                    break;
                default:
                    tileMap[x][y] = BackgroundTile(FLOOR1);
                    break;
                }
                x++;
            }
            x = 0;
            y++;
        }
    }

    keysDown = {};
    keysDown.up = false;
    keysDown.right = false;
    keysDown.down = false;
    keysDown.left = false;

    lastTick = SDL_GetTicks();
}
/* * * *  * * *  * * *  Loads images and fonts and converts them into textures * * *  * * *  * * *  * * * */
bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load sprite sheet texture
    if (!mapSheet.loadFromFile("mapsheet.png", SCALE_UP, gRenderer) || !charSheet.loadFromFile("charsheet.png", SCALE_UP, gRenderer) || !buttonBack.loadFromFile("buttonback.png", 1, gRenderer))
    {
        printf("Failed to load sprite sheet textures!\n");
        success = false;
    }
    else
    {
        medFont = TTF_OpenFont("medieval.ttf", 28);
        if (medFont == NULL)
        {
            printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
            success = false;
        }
        else
        {
            SDL_Color textColor = {255, 255, 255};
            SDL_Color black = {0, 0, 0};
            if (!titleTexture.loadFromRenderedText("Dungeon Crawl", textColor, medFont, gRenderer) || !scoreTexture.loadFromRenderedText("Score: 0", textColor, medFont, gRenderer) || !restartTexture.loadFromRenderedText("Restart?", textColor, medFont, gRenderer) || !winTexture.loadFromRenderedText("You win!", black, medFont, gRenderer) || !playAgainTexture.loadFromRenderedText("Play again?", textColor, medFont, gRenderer) || !highScoreTexture.loadFromRenderedText("High scores", textColor, medFont, gRenderer) || !startGameTexture.loadFromRenderedText("Play", textColor, medFont, gRenderer))
            {
                printf("Failed to render text texture!\n");
                success = false;
            }
            for (int i = 0; i < sizeof(mapClips) / sizeof(mapClips[0]); i++)
            {
                for (int j = 0; j < sizeof(mapClips[0]) / sizeof(mapClips[0][0]); j++)
                {
                    mapClips[i][j].x = i * SCREEN_TILE_SIZE;
                    mapClips[i][j].y = j * SCREEN_TILE_SIZE;
                    mapClips[i][j].w = SCREEN_TILE_SIZE;
                    mapClips[i][j].h = SCREEN_TILE_SIZE;
                }
            }
            for (int i = 0; i < sizeof(charClips) / sizeof(charClips[0]); i++)
            {
                for (int j = 0; j < sizeof(charClips[0]) / sizeof(charClips[0][0]); j++)
                {
                    charClips[i][j].x = i * SCREEN_TILE_SIZE;
                    charClips[i][j].y = j * SCREEN_TILE_SIZE;
                    charClips[i][j].w = SCREEN_TILE_SIZE;
                    charClips[i][j].h = SCREEN_TILE_SIZE;
                }
            }
        }
    }
    return success;
}
/* * * *  * * *  * * * Deallocates everything * * *  * * *  * * *  * * * */
void close()
{
    //Free loaded images
    mapSheet.free();
    charSheet.free();

    //Free loaded text
    titleTexture.free();
    scoreTexture.free();
    restartTexture.free();
    winTexture.free();
    playAgainTexture.free();
    highScoreTexture.free();
    startGameTexture.free();
    TTF_CloseFont(medFont);

    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
/* * * *  * * *  * * * Draws the world to the screen - gets called every 30th of a second  * *  * * *  * * * */
void draw()
{
    //Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(gRenderer);

    if (gameState == PLAYING || gameState == RESTART)
    {

        //First draw the map
        for (int i = 0; i < sizeof(tileMap) / sizeof(tileMap[0]); i++)
        {
            for (int j = 0; j < sizeof(tileMap[0]) / sizeof(tileMap[0][0]); j++)
            {
                mapSheet.render(i * SCREEN_TILE_SIZE, (j + 1) * SCREEN_TILE_SIZE, &mapClips[tileMap[i][j].getImageTile().x][tileMap[i][j].getImageTile().y]);
            }
        }

        //Render items
        for (int i = 0; i < sizeof(items) / sizeof(items[0]); i++)
        {
            if (items[i].exists)
            {
                if (items[i].type == CHEST)
                {
                    charSheet.render(items[i].x * SCREEN_TILE_SIZE, (items[i].y + 1) * SCREEN_TILE_SIZE, &charClips[6][1]);
                }
                if (items[i].type == KEY)
                {
                    mapSheet.render(items[i].x * SCREEN_TILE_SIZE, (items[i].y + 1) * SCREEN_TILE_SIZE, &mapClips[5][0]);
                }
            }
        }
        //Render goblins
        for (int i = 0; i < sizeof(goblins) / sizeof(goblins[0]); i++)
        {
            if (goblins[i].doesExist())
            {
                charSheet.render(goblins[i].getX(), goblins[i].getY() + (1 * SCREEN_TILE_SIZE), &charClips[0][1]);
            }
        }

        //Then draw the user
        if (userData.state == DEAD)
        {
            mapSheet.render(userData.x, userData.y + SCREEN_TILE_SIZE, &mapClips[4][1]);
        }
        else
        {
            charSheet.render(userData.x, userData.y + SCREEN_TILE_SIZE, &charClips[0][5]);
        }
        //Render text
        titleTexture.render(50, 0);
        scoreTexture.render(SCREEN_WIDTH - scoreTexture.getWidth() - 50, 0);

        //If in restart, draw a big black square and the restart button
        if (gameState == RESTART)
        {
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x7f);
            SDL_Rect fullRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(gRenderer, &fullRect);
            SDL_Rect buttonRect = {SCREEN_WIDTH / 2 - buttonBack.getWidth(), SCREEN_HEIGHT / 2 - buttonBack.getHeight(), buttonBack.getWidth() * 2, buttonBack.getHeight() * 2};
            buttonBack.renderAtSize(0, 0, &buttonRect);
            restartTexture.render(SCREEN_WIDTH / 2 - restartTexture.getWidth() / 2, SCREEN_HEIGHT / 2 - buttonBack.getHeight() + 20);
        }
    }
    if (gameState == WIN)
    {
        SDL_SetRenderDrawColor(gRenderer, 0x22, 0x00, 0x00, 0x7f);
        SDL_Rect fullRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(gRenderer, &fullRect);
        SDL_Rect buttonRect = {SCREEN_WIDTH / 2 - buttonBack.getWidth(), SCREEN_HEIGHT / 2 - buttonBack.getHeight(), buttonBack.getWidth() * 2, buttonBack.getHeight() * 2};
        buttonBack.renderAtSize(0, 0, &buttonRect);
        buttonRect = {SCREEN_WIDTH / 2 - buttonBack.getWidth(), SCREEN_HEIGHT / 2 + 50, buttonBack.getWidth() * 2, buttonBack.getHeight() * 2};
        buttonBack.renderAtSize(0, 0, &buttonRect);
        winTexture.render(SCREEN_WIDTH / 2 - restartTexture.getWidth() / 2, SCREEN_HEIGHT / 2 - buttonBack.getHeight() + 20);
        playAgainTexture.render(SCREEN_WIDTH / 2 - playAgainTexture.getWidth() / 2, SCREEN_HEIGHT / 2 + 70);
    }
    if (gameState == MENU)
    {
        drawMenuScreen();
    }
    //Update screen
    SDL_RenderPresent(gRenderer);
}

int round(float input)
{
    float pointFiveAdded = input + 0.5 - (input < 0);
    return (int)pointFiveAdded;
}

void drawMenuScreen()
{
    SDL_SetRenderDrawColor(gRenderer, 0x31, 0x19, 0x24, 0xff);
    SDL_Rect fullRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(gRenderer, &fullRect);
    SDL_Rect buttonRect = {SCREEN_WIDTH / 2 - buttonBack.getWidth(), SCREEN_HEIGHT / 2 - buttonBack.getHeight(), buttonBack.getWidth() * 2, buttonBack.getHeight() * 2};
    buttonBack.renderAtSize(0, 0, &buttonRect);
    buttonRect = {SCREEN_WIDTH / 2 - buttonBack.getWidth(), SCREEN_HEIGHT / 2 + 50, buttonBack.getWidth() * 2, buttonBack.getHeight() * 2};
    buttonBack.renderAtSize(0, 0, &buttonRect);
    startGameTexture.render(SCREEN_WIDTH / 2 - startGameTexture.getWidth() / 2, SCREEN_HEIGHT / 2 - buttonBack.getHeight() + 20);
    highScoreTexture.render(SCREEN_WIDTH / 2 - highScoreTexture.getWidth() / 2, SCREEN_HEIGHT / 2 + 70);
}

/* * * Updates the world to the screen - gets called every 30th of a second * *  * * *  * * * */

void update()
{
    if (gameState == PLAYING)
    {
        Uint32 now = SDL_GetTicks();
        float delta = (static_cast<float>(now) - static_cast<float>(lastTick));

        for (int i = 0; i < sizeof(goblins) / sizeof(goblins[0]); i++)
        {
            if (goblins[i].doesExist())
            {
                goblins[i].update(&tileMap, &userData, delta);
            }
        }

        if (userData.state == DEAD)
        {
            userData.timeSinceDeath += delta;
            if (userData.timeSinceDeath <= timeToShowRestart)
            {
                gameState = RESTART;
            }
        }
        if (userData.state == BOUNCING)
        {
            if (userData.timeSinceBounce >= bounceTime)
            {
                userData.state = CONTROLLABLE;
                userData.timeSinceBounce = 0;
            }
            else
            {
                //do some stuff
                moveUser(userData.direction, bounceSpeed * delta, &userData);
                userData.timeSinceBounce += delta;
            }
        }
        else if (userData.state == CONTROLLABLE)
        {
            moveUser(userData.direction = getDirectionFromKeys(&keysDown), moveSpeed * delta, &userData);
            coord userCoords = {};
            userCoords.x = userData.x;
            userCoords.y = userData.y;
            if (checkMapCollision(&tileMap, &userCoords))
            {

                bool xIsclosestToEdge = abs(SCREEN_TILE_SIZE / 2 - (userData.x % SCREEN_TILE_SIZE)) > abs(SCREEN_TILE_SIZE / 2 - (userData.y % SCREEN_TILE_SIZE));
                if (xIsclosestToEdge)
                {
                    userData.x = round(static_cast<double>(userData.x) + SCREEN_TILE_SIZE / 2) / static_cast<double>(SCREEN_TILE_SIZE) * SCREEN_TILE_SIZE- SCREEN_TILE_SIZE/2;
                }
                else
                {
                    userData.y = round(static_cast<double>(userData.y) + SCREEN_TILE_SIZE / 2) / static_cast<double>(SCREEN_TILE_SIZE) * SCREEN_TILE_SIZE - SCREEN_TILE_SIZE/2;
                }
                userData.state = BOUNCING;
                userData.direction = getOppositeDirection(userData.direction);
            }
            int itemIndex = checkItemCollision(&items, &userData);
            if (itemIndex != -1)
            {
                if (items[itemIndex].type == CHEST)
                {
                    score = score + 1;
                    scoreThisLife = scoreThisLife + 1;
                    items[itemIndex].exists = false;
                    SDL_Color textColor = {255, 255, 255};
                    scoreTexture.loadFromRenderedText("Score: " + std::to_string(score), textColor, medFont, gRenderer);
                }
                else if (items[itemIndex].type == KEY)
                {
                    if (level == MAX_LEVEL)
                    {
                        gameState = WIN;
                    }
                    else
                    {
                        level = level + 1;
                        setUpLevel();
                    }
                }
            }
            int goblinIndex = checkGoblinCollision(&goblins, &userData);
            if (goblinIndex != -1)
            {

                score = score - scoreThisLife;
                scoreThisLife = 0;
                SDL_Color textColor = {255, 255, 255};
                scoreTexture.loadFromRenderedText("Score: " + std::to_string(score), textColor, medFont, gRenderer);
                userData.state = DEAD;
            }
        }
        lastTick = now;
    }
}
/* * * Handles mousebutton down * *  * * *  * * * */

void mousePress(SDL_MouseButtonEvent &e)
{
    printf("event");
    if (e.button == SDL_BUTTON_LEFT)
    {
        if (gameState == RESTART)
        {
            int restartButtonX = SCREEN_WIDTH / 2 - buttonBack.getWidth();
            int restartButtonY = SCREEN_HEIGHT / 2 - buttonBack.getHeight();
            int restartButtonW = buttonBack.getWidth() * 2;
            int restartButtonH = buttonBack.getHeight() * 2;
            if (e.x >= restartButtonX &&
                e.x <= restartButtonX + restartButtonW &&
                e.y >= restartButtonY &&
                e.y <= restartButtonY + restartButtonH)
            {
                printf("Restarting");
                setUpLevel(); // restart the level
            }
        }
        if (gameState == WIN)
        {
            int restartButtonX = SCREEN_WIDTH / 2 - buttonBack.getWidth();
            int restartButtonY = SCREEN_HEIGHT / 2 + 50;
            int restartButtonW = buttonBack.getWidth() * 2;
            int restartButtonH = buttonBack.getHeight() * 2;
            if (e.x >= restartButtonX &&
                e.x <= restartButtonX + restartButtonW &&
                e.y >= restartButtonY &&
                e.y <= restartButtonY + restartButtonH)
            {
                printf("Playing again");
                setUpGame(); // restart the game
                setUpLevel();
            }
        }
        if (gameState == MENU)
        {
            int restartButtonX = SCREEN_WIDTH / 2 - buttonBack.getWidth();
            int restartButtonY = SCREEN_HEIGHT / 2 - buttonBack.getHeight();
            int restartButtonW = buttonBack.getWidth() * 2;
            int restartButtonH = buttonBack.getHeight() * 2;
            if (e.x >= restartButtonX &&
                e.x <= restartButtonX + restartButtonW &&
                e.y >= restartButtonY &&
                e.y <= restartButtonY + restartButtonH)
            {
                printf("Starting game");
                setUpGame();
                setUpLevel();
            }
        }
    }
}

/* * * * * * * * Entry point of program -  * *  * * *  * * * */
/* * * * * * * * Initialises SDL,    * *  * * *  * * * */
/* * * * * * * * then loads media, * *  * * *  * * * */
/* * * * * * * * then initialises game data * *  * * *  * * * */
/* * * * * * * * Then starts game loop * *  * * *  * * * */
/* * * * * * * * Calling update/draw every 30th of a second * *  * * *  * * * */

int main(int argc, char *args[])
{

    //Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        //Load media
        if (!loadMedia())
        {
            printf("Failed to load media!\n");
        }

        else
        {
            // setUpGame();
            // setUpLevel();
            gameState = MENU;
            if (TEST_MODE_ON)
            {
                runTests();
            }
            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            //While application is running
            while (!quit)
            {
                //Handle events on queue
                while (SDL_PollEvent(&e) != 0)
                {
                    //User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    else if (e.type == SDL_KEYDOWN)
                    {
                        //Select surfaces based on key press
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_UP:
                            keysDown.up = true;
                            break;

                        case SDLK_DOWN:
                            keysDown.down = true;
                            break;

                        case SDLK_LEFT:
                            keysDown.left = true;
                            break;

                        case SDLK_RIGHT:
                            keysDown.right = true;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (e.type == SDL_KEYUP)
                    {
                        //Select surfaces based on key press
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_UP:
                            keysDown.up = false;
                            break;

                        case SDLK_DOWN:
                            keysDown.down = false;
                            break;

                        case SDLK_LEFT:
                            keysDown.left = false;
                            break;

                        case SDLK_RIGHT:
                            keysDown.right = false;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (e.type == SDL_MOUSEBUTTONDOWN)
                    {
                        mousePress(e.button);
                    }
                }
                update();
                draw();
                SDL_Delay(1000 / 30);
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}