#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

#define MAP_SIZE 10
#define CELL_SIZE 100

#define PI 3.14159265

typedef uint8_t ui8;

static ui8 MAP[MAP_SIZE * MAP_SIZE] = {
    1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,1,0,0,0,1,
    1,0,0,0,0,1,0,0,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,0,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,
};

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void draw_map(){
	for(int y=0; y<MAP_SIZE; y++){
		for(int x=0; x<MAP_SIZE; x++){
			ui8 cell = MAP[y * MAP_SIZE + x];
			SDL_Rect rect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE-1, CELL_SIZE-1};

			SDL_SetRenderDrawColor(renderer,255*cell,255*cell,255*cell,255);
			SDL_RenderFillRect(renderer,&rect);
		}
	}
}

struct position {
    int x,y;
};

struct line_position {
    int x0,y0,x1,y1;
};

int main(){
    //Initialize windows and renderer
	SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("2D Map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	
    //Create player
    struct position player_pos = { SCREEN_WIDTH/4, SCREEN_HEIGHT/4};
    struct line_position ray = {player_pos.x+5,player_pos.y+5,player_pos.x+95,player_pos.y+5};
	SDL_Rect player = {player_pos.x,player_pos.y,10,10};


		
    //Crete event and game loop
	SDL_Event event;
	while(1){
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					exit(0);
					break;

				case SDL_KEYDOWN:
                    
					switch(event.key.keysym.sym){
						case SDLK_RIGHT:
						case SDLK_a:
                            player_pos.x += 10;
							player.x += 10;
                            ray.x0 += 10;
                            ray.x1 += 10;
							break;
						case SDLK_LEFT:
						case SDLK_d:
							player.x -= 10;
                            player_pos.x = 10;
                            ray.x0 -= 10;
                            ray.x1 -= 10;
							break;
						case SDLK_UP:
						case SDLK_w:
							player.y -= 10;
							player_pos.y -= 10;
                            ray.y0 -= 10;
                            ray.y1 -= 10;
							break;
						case SDLK_DOWN:
						case SDLK_s:
							player.y += 10;
							player_pos.y += 10;
                            ray.y0 += 10;
                            ray.y1 += 10; 
							break;
						case SDLK_ESCAPE:
							exit(0);
							break;
						default:
							break;
					}
					break;
                case SDL_MOUSEMOTION:
                    ray.x1 = event.motion.x;
                    ray.y1 = event.motion.y;
                    printf("x: %d - y: %d \n",event.motion.x,event.motion.y);
                    break;
				default:
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer,255,125,0,255);
		SDL_RenderClear(renderer);
		draw_map();

		//Draw player
		SDL_SetRenderDrawColor(renderer,255,125,0,255);
        SDL_RenderDrawLine(renderer, ray.x0, ray.y0,ray.x1,ray.y1);
		SDL_RenderFillRect(renderer,&player);


		SDL_RenderPresent(renderer);
        SDL_Delay(10);
	}
	return 0;
}
