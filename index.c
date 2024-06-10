#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

int main(){

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_Event event;
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH,SCREEN_HEIGHT,0,&window,&renderer);
	SDL_RenderSetScale(renderer,1,1);


	SDL_Rect rectang = {SCREEN_WIDTH/2,SCREEN_HEIGHT/2,10,10};
	SDL_Rect * point_rect = &rectang;



	
	while(1){
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					printf("exiting the window mano\n");
					exit(0);
					break;
				case SDL_KEYDOWN:
					printf("a key was pressed mano\n");
					switch(event.key.keysym.sym){
						case SDLK_d:
						case SDLK_RIGHT:
							rectang.x += 10;
							printf("to the right mano\n");
							break;
						case SDLK_LEFT:
						case SDLK_a:
							rectang.x -= 10;
							break;
						case SDLK_UP:
						case SDLK_w:
							rectang.y -= 10;
							printf("up mano\n");
							break;
						case SDLK_DOWN:
						case SDLK_s:
							rectang.y += 10;
							printf("down mano\n");
							break;
						case SDLK_ESCAPE:
							printf("going out\n");
							exit(0);
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer,255,125,0,255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer,0,0,0,255);
		SDL_RenderFillRect(renderer,point_rect);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}
	return 0;
}
