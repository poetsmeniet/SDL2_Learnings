/* compile: gcc -lSDL2 -lSDL2_image */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window *init();
void loadImage(SDL_Window *window, char *file);
void closeAll(SDL_Window *window);
int handleEvents(SDL_Window *window);

int main( int argc, char* args[] )
{
	SDL_Window* window = init();

    while(handleEvents(window)){
        SDL_Delay(5);
    }

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

/* Loads an image onto surface */
void loadImage(SDL_Window *window, char *file)
{
	SDL_Surface *screenSurface = SDL_GetWindowSurface( window );

	SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

    /* IMG_Load needs additional lib */
    SDL_Surface *surfImg = IMG_Load(file);
    if( surfImg == NULL ) { 
        printf( "Unable to load image %s! SDL Error: %s\n", "cat2.jpg", SDL_GetError()); 
    } 

    /* Optimize surface */
    surfImg = SDL_ConvertSurface(surfImg, screenSurface->format, 0); 
    if(surfImg == NULL){ 
        printf( "Unable to optimize image %s! SDL Error: %s\n", file, SDL_GetError()); 
    }

    /* Scaled vs non scaled image blitz */
    if(strncmp(file, "q.jpg", 5) == 0){
        SDL_Rect stretchRect; 
        stretchRect.x = 0; 
        stretchRect.y = 0; 
        stretchRect.w = SCREEN_WIDTH; 
        stretchRect.h = SCREEN_HEIGHT; 
        SDL_BlitScaled(surfImg, NULL, screenSurface, &stretchRect);
    }else{
	    SDL_BlitSurface( surfImg, NULL, screenSurface, NULL );
    }
	SDL_UpdateWindowSurface(window);
    SDL_FreeSurface(surfImg);
}

void closeAll(SDL_Window *window)
{
	SDL_DestroyWindow( window );
    IMG_Quit();
	SDL_Quit();
}

int handleEvents(SDL_Window *window)
{
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0){
        /* Handle quit event */
        if(e.type == SDL_QUIT)
            return 0;

        /* Handle keypresses */
        if(e.type == SDL_KEYDOWN){
            switch(e.key.keysym.sym){
                case SDLK_UP:
                    loadImage(window, "cat.bmp");
                    break;
                case SDLK_DOWN:
                    loadImage(window, "cat2.jpg");
                    break;
                case SDLK_q:
                    loadImage(window, "q.jpg");
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}
