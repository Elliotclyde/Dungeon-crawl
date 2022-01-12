#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

//Wrapper class for textures
class LTexture
{
public:
    //Initializes variables
    LTexture();

    //Deallocates memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile(std::string path, float scaleUp,SDL_Renderer * renderer);

    //Creates image from font string
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor,TTF_Font *font, SDL_Renderer * renderer);

    //Deallocates texture
    void free();

    //Renders texture at given point
    void render(int x, int y, SDL_Rect *clip = NULL);
    
    //Renders texture at specified size
    void renderAtSize(int x, int y,SDL_Rect *destination);

    //Gets image dimensions
    int getWidth();
    int getHeight();

    void setBlendMode(SDL_BlendMode blending);

private:
    //The actual hardware texture
    SDL_Texture *mTexture;
    SDL_Renderer * renderer;

    //Image dimensions
    int mWidth;
    int mHeight;
};