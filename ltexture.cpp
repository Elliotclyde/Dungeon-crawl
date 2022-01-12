#include "ltexture.h"

LTexture::LTexture()
{
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    //Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path,float scaleUp,SDL_Renderer * inputRenderer)
{
    renderer=inputRenderer;
    
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture *newTexture = NULL;

    //Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    SDL_Surface *resizedSurface = SDL_CreateRGBSurface(0, loadedSurface->w * scaleUp, loadedSurface->h * scaleUp, 32, 0, 0, 0, 0);

    int scaledSurface = SDL_BlitScaled(loadedSurface, NULL, resizedSurface, NULL);

    if (scaledSurface > 0)
    {
        printf("Surface could not be resized! SDL Error: %s\n", SDL_GetError());
    }

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Color key image
        //Create texture from surface pixels
        Uint32 colorkey = SDL_MapRGB(resizedSurface->format, 0, 0, 0);
        SDL_SetColorKey(resizedSurface, SDL_TRUE, colorkey);

        newTexture = SDL_CreateTextureFromSurface(renderer, resizedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor,TTF_Font *font ,SDL_Renderer * inputRenderer)
{
    renderer=inputRenderer;
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( font, textureText.c_str(), textColor );

    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( renderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL;
}

void LTexture::free()
{
    //Free texture if it exists
    if (mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}


void LTexture::render(int x, int y, SDL_Rect *clip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

  SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
    
}
void LTexture::renderAtSize(int x, int y,SDL_Rect *destination)
{
  SDL_Rect clip ={0,0,mWidth,mHeight};
  SDL_RenderCopy(renderer, mTexture, &clip, destination);
}


int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    //Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}
