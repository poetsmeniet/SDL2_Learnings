/* compile: gcc -lSDL2 -lSDL2_image */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 450

SDL_Window *init();
SDL_Renderer *initRenderer(SDL_Window *window);
SDL_Texture *loadImageAsTexture(SDL_Window *window, SDL_Renderer *gRenderer, char *file, int width, int height, int x, int y);
void closeAll(SDL_Window *window);
int handleEvents(SDL_Window *window, SDL_Renderer *gRenderer, SDL_Rect *ballRect);
void bounce(SDL_Rect *ballRect);

int velocity = 10;
int directionY = 0;

int main(int argc, char* args[])
{
	SDL_Window *window = init();
    SDL_Renderer *gRenderer = initRenderer(window);
    
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_Rect ballRect; 
    ballRect.x = 0; 
    ballRect.y = 0; 
    ballRect.w = SCREEN_WIDTH; 
    ballRect.h = SCREEN_HEIGHT; 

    /* Initial position of ball */
    int x = 0;
    int y = 0;

    /* Ball dimensions */
    ballRect.w = 30; 
    ballRect.h = 30; 

    /* Load background image */
    SDL_Texture *bg = loadImageAsTexture(window, gRenderer, "tdh_basketball_court_by_marcellsalek_26-d9aijwe.png", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    SDL_Texture *bball = loadImageAsTexture(window, gRenderer, "bbal_30.png", 30, 30, x, y);
        
    directionY = 1;
    while(handleEvents(window, gRenderer, &ballRect)){
        bounce(&ballRect);
        SDL_RenderClear(gRenderer);

        /* Render bg */
        SDL_RenderCopyEx(gRenderer, bg, NULL, NULL, 0.0, NULL, flip);
    
        int test = SDL_RenderCopyEx(gRenderer, bball, NULL, &ballRect, 0.0, NULL, flip);
	    SDL_RenderPresent(gRenderer);
    }

    SDL_DestroyTexture(bball);
    SDL_DestroyTexture(bg);
    closeAll(window);
	return 0;
}

/* Init window */
SDL_Window * init()
{
    IMG_Init(IMG_INIT_JPG);

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
	}
	
	SDL_Window *window = SDL_CreateWindow("Template window", \
            SDL_WINDOWPOS_UNDEFINED, \
            SDL_WINDOWPOS_UNDEFINED, \
            SCREEN_WIDTH, \
            SCREEN_HEIGHT, \
            SDL_WINDOW_SHOWN);

	if(window == NULL){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
	}

    return window;
}

SDL_Renderer *initRenderer(SDL_Window *window)
{
	/* Create vsynced renderer */
	SDL_Renderer *gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(gRenderer == NULL){
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    }else{
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		int imgFlags = IMG_INIT_PNG;
		if(!(IMG_Init(imgFlags) & imgFlags))
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
	}
     return gRenderer;
}

/* Loads an image onto surface */
SDL_Texture *loadImageAsTexture(SDL_Window *window, SDL_Renderer *gRenderer, char *file, int width, int height, int x, int y)
{
	SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    /* IMG_Load needs additional lib */
    SDL_Surface *surfImg = IMG_Load(file);
    if(surfImg == NULL) { 
        printf("Unable to load image %s! SDL Error: %s\n", "cat2.jpg", SDL_GetError()); 
    } 

    /* Optimize surface */
   // surfImg = SDL_ConvertSurface(surfImg, screenSurface->format, 0); 
   // if(surfImg == NULL){ 
   //     printf("Unable to optimize image %s! SDL Error: %s\n", file, SDL_GetError()); 
   // }
   
    /* Convert to texture */
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_Rect ballRect; 
    ballRect.x = x; 
    ballRect.y = y; 
    ballRect.w = surfImg->w; 
    ballRect.h = surfImg->h; 

	SDL_Texture* newTexture = NULL;
    newTexture = SDL_CreateTextureFromSurface(gRenderer, surfImg);
	if(newTexture == NULL)
		printf("Unable to create texture from %s! SDL Error: %s\n", file, SDL_GetError());

	//SDL_UpdateWindowSurface(window);
    SDL_FreeSurface(surfImg);

    return newTexture;
}

void closeAll(SDL_Window *window)
{
	SDL_DestroyWindow(window);
    IMG_Quit();
	SDL_Quit();
}

int handleEvents(SDL_Window *window, SDL_Renderer *gRenderer, SDL_Rect *ballRect)
{
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0){
        /* Handle quit event */
        if(e.type == SDL_QUIT)
            return 0;

        /* Handle keypresses */
        if(e.type == SDL_KEYDOWN && e.key.repeat == 1){
            switch(e.key.keysym.sym){
                case SDLK_UP:
                    ballRect->y-=velocity;
                    break;
                case SDLK_DOWN:
                    ballRect->y+=velocity;
                    break;
                case SDLK_LEFT:
                    ballRect->x-=velocity;
                    break;
                case SDLK_RIGHT:
                    ballRect->x+=velocity;
                    break;
                case SDLK_q:
                    return 0;
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}

void bounce(SDL_Rect *ballRect)
{
    //static int directionY = 10;
    static int ac = 0;
    
    ballRect->y += (directionY + ac);
    if(ac < 15)
        ac++;

    int floorOffset = 100;

    /* If the dot reaches top or bottom */
    if((ballRect->y < 0) || (ballRect->y + ballRect->h > (SCREEN_HEIGHT - floorOffset)))
    {
        /* Change direction */
        ac = ac * -1;
        printf("ac = %d\n", ac);
        
        /* Reverse acceleration */
        directionY = directionY * -1;

    }
}
