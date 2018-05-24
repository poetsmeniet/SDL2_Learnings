/* compile: gcc -lm -lSDL2 -lSDL2_image */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 500
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

//Todo: add some more error checking

/* Space ship struct */
typedef struct spaceShip{
    double velocity;
    int directionY;
    double x;
    double y;
    double velX;
    double velY;
    double angle; /* Ships angle */
    double targetAngle; /* Ships possible angle */
    int rotation;
    SDL_Rect sRect;
}ss;

/* Declare functions */
SDL_Window *init();
SDL_Renderer *initRenderer(SDL_Window *window);
SDL_Texture *loadImageAsTexture(SDL_Window *window, SDL_Renderer *gRenderer, char *file, int width, int height, int x, int y);
void closeAll(SDL_Window *window, SDL_Renderer *gRenderer);
int handleEvents(SDL_Window *window, SDL_Renderer *gRenderer, ss *ss1);
void move(ss *ss1);
void initSpaceShip(ss *ss1);

int main(int argc, char* args[])
{
    /* Define space ship */
    ss ss1;
    initSpaceShip(&ss1);
    
    /* SDL window + renderer */
	SDL_Window *window = init();
    SDL_Renderer *gRenderer = initRenderer(window);
    
    /* Load game objects */
    SDL_Texture *bg = loadImageAsTexture(window, gRenderer, "space-wallpaper-preview-11.jpg", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    SDL_Texture *ship1 = loadImageAsTexture(window, gRenderer, "spaceship.png", 30, 30, 0, 0);
        
    /* Main event loop */
    while(handleEvents(window, gRenderer, &ss1)){
        move(&ss1);
        SDL_RenderClear(gRenderer);

        /* Render bg and ship.. todo: move to function */
        SDL_RenderCopyEx(gRenderer, bg, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(gRenderer, ship1, NULL, &ss1.sRect, ss1.targetAngle, NULL, SDL_FLIP_NONE);

	    SDL_RenderPresent(gRenderer);
    }

    SDL_DestroyTexture(ship1);
    SDL_DestroyTexture(bg);
    closeAll(window, gRenderer);
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
	SDL_Texture* newTexture = NULL;
    newTexture = SDL_CreateTextureFromSurface(gRenderer, surfImg);
	if(newTexture == NULL)
		printf("Unable to create texture from %s! SDL Error: %s\n", file, SDL_GetError());

    SDL_FreeSurface(surfImg);

    return newTexture;
}

void closeAll(SDL_Window *window, SDL_Renderer *gRenderer)
{
    SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(window);
    gRenderer = NULL;
    window = NULL;
    IMG_Quit();
	SDL_Quit();
}

int handleEvents(SDL_Window *window, SDL_Renderer *gRenderer, ss *ss1)
{
    SDL_Event e;

    while(SDL_PollEvent(&e) != 0){
        /* Handle quit event */
        if(e.type == SDL_QUIT)
            return 0;

        /* Handle keypresses */
        if(e.type == SDL_KEYDOWN && e.key.repeat == 0){

            /* Find adjustments for going through poles */
            double diff = (ss1->angle - ss1->targetAngle);
            if(diff > 260){
                diff = 360 - diff;
                ss1->angle = ss1->targetAngle - diff;
            }
            if(diff < -260){
                diff = 360 + diff;
                ss1->angle = ss1->targetAngle + diff;
            }

            switch(e.key.keysym.sym){
                case SDLK_UP:
                    if(ss1->velocity > 0){
                        if(diff > 130|| diff < -130){
                            /* Only reduce speed, minimal autopilot */
                            ss1->velocity -= 0.5;
                        }else{
                            /* Increase speed */
                            ss1->velocity += 0.5;
                            /* Keeping some momentum in old direction (funcky) */
                            ss1->angle = ss1->angle - ((ss1->angle - ss1->targetAngle) / ss1->velocity);
                        }
                    }else{
                        /* When starting engines, no previous angle available */
                        ss1->angle = ss1->targetAngle;
                        ss1->velocity += 0.5;
                    }
                    break;
                case SDLK_LEFT:
                    ss1->rotation = -2;
                    break;
                case SDLK_RIGHT:
                    ss1->rotation = 2;
                    break;
                //case SDLK_SPACE:
                //    ss1->velocity += 2.0;
                //    break;
                case SDLK_q:
                    return 0;
                    break;
                default:
                    break;
            }
        }
        if(e.type == SDL_KEYUP && e.key.repeat == 0){
            switch(e.key.keysym.sym){
                case SDLK_LEFT:
                    ss1->rotation = 0;
                    break;
                case SDLK_RIGHT:
                    ss1->rotation = 0;
                    break;
                default:
                    break;
            }
        }
    }

    return 1;
}

/* Sim smooth movement */
void move(ss *ss1)
{
    /* Reverse movement not implemented */
    if(ss1->velocity < 0)
        ss1->velocity = 0;
    
    /* Determine movement by using angle/cos/sin */
    ss1->velX = ss1->velocity * cos(degToRad(ss1->angle - 90));
    ss1->x += ss1->velX;
    ss1->sRect.x = ss1->x;
    
    ss1->velY = ss1->velocity * sin(degToRad(ss1->angle - 90));
    ss1->y += ss1->velY;
    ss1->sRect.y = ss1->y;
    
    /* Teleport through walls*/
    if((ss1->sRect.x < 0)){
        ss1->x = SCREEN_WIDTH;
    }else if(ss1->sRect.x > SCREEN_WIDTH){
        ss1->x = 0.0;
    }

    ss1->sRect.y += ss1->velY;

    /* Teleport through floor/ceiling */
    if( ss1->sRect.y + ss1->sRect.h < 0 ){
        ss1->y = SCREEN_HEIGHT;
    }
    
    if(ss1->sRect.y > SCREEN_HEIGHT){
        ss1->y = 0;
    }

    /* Rotation */
    ss1->targetAngle += ss1->rotation;
    if(ss1->targetAngle <= 0)
        ss1->targetAngle = 360.0;
    else if(ss1->targetAngle >= 360)
        ss1->targetAngle = 0;
}

void initSpaceShip(ss *ss1)
{
    ss1->velocity = 0.0;
    ss1->directionY = 1;
    ss1->velX = 0;
    ss1->velY = 0;
    ss1->x = 100;
    ss1->y = 100;
    ss1->angle = 0.0;
    ss1->rotation = 0;
    ss1->sRect.x = 100; 
    ss1->sRect.y = 100; 
    ss1->sRect.w = 40; 
    ss1->sRect.h = 40; 
}
