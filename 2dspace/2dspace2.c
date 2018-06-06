/* compile: gcc -lm -lSDL2 -lSDL2_image */
/* An exploration on creating "2d planetary mechanics" 
 * gcc -Wall -lm -lSDL2 -lSDL2_image */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 700 
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

//Todo: add some more error checking

/* Space ship struct */
typedef struct spaceShip{
    double thrust;
    double speed;
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
void grav(ss *ss1);

int main(int argc, char* args[])
{
    /* Define space ship */
    ss ss1;
    initSpaceShip(&ss1);
    
    /* SDL window + renderer */
	SDL_Window *window = init();
    SDL_Renderer *gRenderer = initRenderer(window);

    /* Temporary moon rect */
    SDL_Rect moonRect;
    moonRect.x = 500;
    moonRect.y = 220;
    moonRect.w = 83;
    moonRect.h = 83;
    
    /* Load game objects */
    SDL_Texture *bg = loadImageAsTexture(window, gRenderer, "space-wallpaper-preview-1.jpg", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    SDL_Texture *ship1 = loadImageAsTexture(window, gRenderer, "spaceship.png", 30, 30, 0, 0);
    SDL_Texture *moon = loadImageAsTexture(window, gRenderer, "moon.png", 79, 79, 640, 350);
        
    /* Main event loop */
    while(handleEvents(window, gRenderer, &ss1)){
        move(&ss1);
        grav(&ss1);
        SDL_RenderClear(gRenderer);

        /* Render bg and ship.. todo: move to function */
        SDL_RenderCopyEx(gRenderer, bg, NULL, NULL, 0.0, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(gRenderer, moon, NULL, &moonRect, 0.0, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(gRenderer, ship1, NULL, &ss1.sRect, ss1.targetAngle, NULL, SDL_FLIP_NONE);

	    SDL_RenderPresent(gRenderer);
        //printf("Speed: %f, thrust: %f, angle: %f, ta: %f\n", ss1.speed, ss1.thrust, ss1.angle, ss1.targetAngle);
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
            switch(e.key.keysym.sym){
                case SDLK_UP:
                    if(ss1->speed > 0){
                        ss1->thrust += 0.5;
                        
                        /* This might need some mathimatical refinement */
                        float diff;
                        diff = remainder(ss1->targetAngle - ss1->angle, 360);
                        diff /= 2;
                        
                        double newAngle = 0.0;
                        newAngle = (ss1->angle + diff);

                        if(diff < 0)
                            diff *= -1;

                        if(diff < 75){
                            ss1->speed += ss1->thrust;
                            ss1->angle = newAngle;
                        }else{
                            ss1->speed -= ss1->thrust;
                        }
                    }else{
                        /* When starting engines, no previous angle available */
                        ss1->angle = ss1->targetAngle;
                        ss1->thrust += 0.5;
                        ss1->speed += ss1->thrust;
                    }
                    printf("Speed: %f, thrust: %f, angle: %f, ta: %f\n", ss1->speed, ss1->thrust, ss1->angle, ss1->targetAngle);
                    break;
                case SDLK_LEFT:
                    ss1->rotation -= 2;
                    break;
                case SDLK_RIGHT:
                    ss1->rotation += 2;
                    break;
                case SDLK_DOWN:
                    ss1->speed -= 0.5;
                    break;
                case SDLK_SPACE:
                    ss1->thrust += 2.0;
                    break;
                case SDLK_q:
                    return 0;
                    break;
                default:
                    break;
            }
        }
        if(e.type == SDL_KEYUP && e.key.repeat == 0){
            switch(e.key.keysym.sym){
                case SDLK_UP:
                    ss1->thrust = 0;
                    break;
                case SDLK_DOWN:
                    ss1->thrust = 0;
                    break;
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

/* Movement of space ship */
void move(ss *ss1)
{
    /* Reverse movement not implemented */
    if(ss1->speed < 0)
        ss1->speed = 0;
    
    /* Determine x/y vel by using angle/cos/sin */
    ss1->velX = ss1->speed * cos(degToRad(ss1->angle - 90));
    ss1->x += ss1->velX;
    ss1->sRect.x = ss1->x;
    
    ss1->velY = ss1->speed * sin(degToRad(ss1->angle - 90));
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
    if(ss1->targetAngle <= 0)
        ss1->targetAngle = 360.0;
    else if(ss1->targetAngle >= 360)
        ss1->targetAngle = 0;

    ss1->targetAngle += ss1->rotation;
}

void initSpaceShip(ss *ss1)
{
    ss1->thrust = 0.0;
    ss1->speed = 0.0;
    ss1->velX = 0;
    ss1->velY = 0;
    ss1->x = 300;
    ss1->y = 100;
    ss1->angle = 105.0;
    ss1->targetAngle = ss1->angle;
    ss1->rotation = 0;
    ss1->sRect.x = 100; 
    ss1->sRect.y = 100; 
    ss1->sRect.w = 40; 
    ss1->sRect.h = 40; 
}

double bearing(double a1, double a2, double b1, double b2) {
    double TWOPI = 6.2831853071795865;
    double RAD2DEG = 57.2957795130823209;
    
    double theta = atan2(b1 - a1, a2 - b2);
    if (theta < 0.0)
        theta += TWOPI;
    
    return RAD2DEG * theta;
}

double distance(int x1, int y1, int x2, int y2)
{
    int a = x1 - x2;
    int b = y1 - y2;
    
    double c = sqrt( a*a + b*b );
    return c;
}

/* Hard coded the gravity object, just for testing */
void grav(ss *ss1)
{
    float dst = 0.0;
    int xD = 535;
    int yD = 250;
    dst = distance(ss1->x, ss1->y, xD, yD);
    if(dst < 200 && dst > 10){

        /* Apply gravity to x/y */
        float gravity = 1.0;
        float brng = bearing(ss1->x, ss1->y, xD, yD);
        float dstMp = dst * 0.04;
        gravity /= dstMp;

        ss1->velX = gravity * cos(degToRad(brng - 90));
        ss1->x += ss1->velX;
        ss1->sRect.x = ss1->x;
        
        ss1->velY = gravity * sin(degToRad(brng - 90));
        ss1->y += ss1->velY;
        ss1->sRect.y = ss1->y;

        /* The closer the object, the higher the speed towards it 
         * -todo:  speed increases at vector comp*/
        ss1->speed -= 0.1 / (dst / 10);
    }
}
