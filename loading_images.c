/* compile: gcc -lSDL2 -lSDL2_image */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window *init();
void mediaStuff(SDL_Window *window);
void closeAll(SDL_Window *window);

int main( int argc, char* args[] )
{
	SDL_Window* window = init();
    mediaStuff(window);
    closeAll(window);

	return 0;
}

/* Init window */
SDL_Window * init()
{
    IMG_Init(IMG_INIT_JPG);

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return NULL;
	}
	
	SDL_Window *window = SDL_CreateWindow( "Template window", \
            SDL_WINDOWPOS_UNDEFINED, \
            SDL_WINDOWPOS_UNDEFINED, \
            SCREEN_WIDTH, \
            SCREEN_HEIGHT, \
            SDL_WINDOW_SHOWN );

	if( window == NULL ){
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return NULL;
	}

    return window;
}

void mediaStuff(SDL_Window *window)
{
	SDL_Surface *screenSurface = SDL_GetWindowSurface( window );

	SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

    SDL_Surface* gCat = NULL;
    
    gCat = SDL_LoadBMP("cat.bmp");
    if( gCat == NULL ) { 
        printf( "Unable to load image %s! SDL Error: %s\n", "cat.bmp", SDL_GetError()); 
    } 

	SDL_BlitSurface( gCat, NULL, screenSurface, NULL );
	SDL_UpdateWindowSurface(window);

	SDL_Delay( 2000 );
    
    /* IMG_Load needs additional lib */
    gCat = IMG_Load("cat2.jpg");
    if( gCat == NULL ) { 
        printf( "Unable to load image %s! SDL Error: %s\n", "cat2.jpg", SDL_GetError()); 
    } 

	SDL_BlitSurface( gCat, NULL, screenSurface, NULL );
	SDL_UpdateWindowSurface(window);
	
	SDL_Delay( 2000 );
}

void closeAll(SDL_Window *window)
{
	SDL_DestroyWindow( window );
    IMG_Quit();
	SDL_Quit();
}
