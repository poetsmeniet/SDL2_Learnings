/* compile: gcc -lSDL2 -lSDL2_image */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 500

SDL_Window *init();
SDL_Renderer *initRenderer(SDL_Window *window);
SDL_Texture *loadImageAsTexture(SDL_Window *window, SDL_Renderer *gRenderer, char *file, int width, int height, int x, int y);
void closeAll(SDL_Window *window);
int handleEvents(SDL_Window *window, SDL_Renderer *gRenderer, SDL_Rect *bRect);
void bounce(SDL_Rect *bRect);
void move(SDL_Rect *bRect);

/* Will pass these globals as a property of the ball */

int velocity = 1;
int directionY = 0;
int velX= 0;
int velY= 0;

int main(int argc, char* args[])
{
	SDL_Window *window = init();
    SDL_Renderer *gRenderer = initRenderer(window);
    
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_Rect bRect; 
    bRect.x = 0; 
    bRect.y = 0; 
    bRect.w = SCREEN_WIDTH; 
    bRect.h = SCREEN_HEIGHT; 

    /* Initial position of ball */
    int x = 0;
    int y = 0;

    /* Ball dimensions */
    bRect.w = 40; 
    bRect.h = 40; 

    /* Load background image */
    SDL_Texture *bg = loadImageAsTexture(window, gRenderer, "space-wallpaper-preview-11.jpg", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    SDL_Texture *bball = loadImageAsTexture(window, gRenderer, "spaceship.png", 30, 30, x, y);
        
    directionY = 1;
    while(handleEvents(window, gRenderer, &bRect)){
        move(&bRect);
        SDL_RenderClear(gRenderer);

        /* Render bg */
        SDL_RenderCopyEx(gRenderer, bg, NULL, NULL, 0.0, NULL, flip);
    
        int test = SDL_RenderCopyEx(gRenderer, bball, NULL, &bRect, 0.0, NULL, flip);
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
	
	SDL_Window *window = SDL_CreateWindow("Learnings spaces'", \
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

    /* Convert to texture */
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_Rect bRect; 
    bRect.x = x; 
    bRect.y = y; 
    bRect.w = surfImg->w; 
    bRect.h = surfImg->h; 

	SDL_Texture* newTexture = NULL;
    newTexture = SDL_CreateTextureFromSurface(gRenderer, surfImg);
	if(newTexture == NULL)
		printf("Unable to create texture from %s! SDL Error: %s\n", file, SDL_GetError());

    SDL_FreeSurface(surfImg);

    return newTexture;
}

void closeAll(SDL_Window *window)
{
	SDL_DestroyWindow(window);
    IMG_Quit();
	SDL_Quit();
}

int handleEvents(SDL_Window *window, SDL_Renderer *gRenderer, SDL_Rect *bRect)
{
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0){
        /* Handle quit event */
        if(e.type == SDL_QUIT)
            return 0;

        /* Handle keypresses */
        if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
            switch(e.key.keysym.sym){
                case SDLK_UP:
                    velY -= velocity;
                    break;
                case SDLK_DOWN:
                    velY += velocity;
                    break;
                case SDLK_LEFT:
                    velX -= velocity;
                    break;
                case SDLK_RIGHT:
                    velX += velocity;
                    break;
                case SDLK_q:
                    return 0;
                    break;
                default:
                    break;
            }
        }

        //if(e.type == SDL_KEYUP && e.key.repeat == 0){
        //    switch(e.key.keysym.sym){
        //        case SDLK_UP:
        //            velY += velocity;
        //            break;
        //        case SDLK_DOWN:
        //            velY -= velocity;
        //            break;
        //        case SDLK_LEFT:
        //            velX += velocity;
        //            break;
        //        case SDLK_RIGHT:
        //            velX -= velocity;
        //            break;
        //        default:
        //            break;
        //    }
        //}
    }

    return 1;
}

/* Sim smooth movement */
void move(SDL_Rect *bRect)
{
    bRect->x += velX;

    /* Teleport */
    if((bRect->x < 0 )){
        bRect->x = SCREEN_WIDTH;
    }

    if(bRect->x > SCREEN_WIDTH){
        bRect->x = 0;
    }

    bRect->y += velY;

    /* Detect floor/ceiling */
    if( bRect->y + bRect->h < 0 ){
        bRect->y = SCREEN_HEIGHT;
    }
    
    if(bRect->y > SCREEN_HEIGHT){
        bRect->y = 0;
    }
}
