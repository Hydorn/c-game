#include <stddef.h>
#include <SDL2/SDL.h>


#define S_W 600
#define S_H 600

SDL_Texture* LoadTexture(const char filename[], SDL_Renderer* renderTarget) {
    SDL_Texture* texture = NULL;
    SDL_Surface* surface = NULL;

    surface = SDL_LoadBMP(filename);
    if (!surface) {
        printf("SDL_LoadBMP Error: %s\n", SDL_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(renderTarget, surface);
    if (!texture) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(surface);  // Free the surface after creating the texture

    return texture;
}


int main(){

    const int FPS = 60;
    int frameTime = 0;


    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *window_text = NULL;
    SDL_Rect playerRect;
    SDL_Rect playerPos = {0,0,32,32};
    int frameWidth, frameHeight;
    int textureWidth, textureHeight;

    window = SDL_CreateWindow("Sprite test",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,S_W,S_H,0);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    window_text = LoadTexture("sprites.bmp",renderer);
    int running = 1;
    SDL_Event inputsEv;

    SDL_QueryTexture(window_text,NULL,NULL,&textureWidth,&textureHeight);
    frameWidth = textureWidth/4;
    frameHeight = textureHeight/4;

    playerRect.x = playerRect.y = 0;
    playerRect.w = frameWidth;
    playerRect.h = frameHeight;



    while(running){
        SDL_PollEvent(&inputsEv);
        switch (inputsEv.type)
        {
        case SDL_QUIT:
            running=0;
            break;
        default:
            break;
        }

        printf("x: %d - tw: %d - fw: %d \n",playerRect.x, textureWidth,frameWidth);
        frameTime++;
        if(FPS/frameTime == 4){
            frameTime=0;
            playerRect.x += frameWidth;
            if(fabs(textureWidth-playerRect.x)<frameWidth){
                playerRect.x=0;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,window_text,&playerRect,&playerPos);
        SDL_RenderPresent(renderer);
    }
    return 0;
}