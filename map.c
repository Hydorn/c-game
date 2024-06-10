#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 1900
#define SCREEN_HEIGHT 950

#define MAP_SIZE 19
#define CELL_SIZE 50

#define PI 3.14159265

typedef uint8_t ui8;

static ui8 MAP[19 * 19] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void draw_map(){
	for(int y=0; y<MAP_SIZE; y++){
		for(int x=0; x<MAP_SIZE; x++){
			ui8 cell = MAP[y * MAP_SIZE + x];
			SDL_Rect rect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};

			SDL_SetRenderDrawColor(renderer,255*cell,255*cell,255*cell,255);
			SDL_RenderFillRect(renderer,&rect);
		}
	}
}

struct line {
	int x0, y0, x1, y1, length, angle;
};

void initialize_line(struct line *x, int x0, int y0, int length){
	x->x0 = x0;
	x->y0 = y0;
	x->x1 = x0+length;
	x->y1 = y0;
	x->length = length;
	x->angle = 0;
}

void rotate_line(struct line *x, int angle) {
    x->angle = (x->angle + angle) % 360; // Update the angle and keep it within [0, 360)

    // Convert the angle to radians
    double radians = x->angle * M_PI / 180.0;

    // Calculate the new endpoint (x1, y1) based on the rotation
    x->x1 = x->x0 + (int)(x->length * cos(radians));
    x->y1 = x->y0 + (int)(x->length * sin(radians));
}

int main(){
	SDL_Event event;
	struct line NICE;
	

	SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("2D Map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	
	

	SDL_Rect player = {SCREEN_WIDTH/4,SCREEN_HEIGHT/2,10,10};
	SDL_Rect * player_p = &player;
	initialize_line(&NICE, player.x, player.y,100);

		

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
						case SDLK_RIGHT:
							player.x += 10;
							NICE.x0 += 10;
							NICE.x1 += 10;
							printf("to the right mano\n");
							break;
						case SDLK_LEFT:
							player.x -= 10;
							NICE.x0 -= 10;
							NICE.x1 -= 10;

							break;
						case SDLK_UP:
						case SDLK_w:
							player.y -= 10;
							NICE.y0 -= 10;
							NICE.y1 -= 10;
							printf("up mano\n");
							break;
						case SDLK_DOWN:
						case SDLK_s:
							player.y += 10;
							NICE.y0 += 10;
							NICE.y1 += 10;
							printf("down mano\n");
							break;
						case SDLK_ESCAPE:
							printf("going out\n");
							exit(0);
							break;
						case SDLK_a:
							rotate_line(&NICE,10);
							break;
						case SDLK_d:
							rotate_line(&NICE,-10);
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
		draw_map();

		//Draw player
		SDL_SetRenderDrawColor(renderer,255,125,0,255);
		SDL_RenderDrawLine(renderer,player.x, player.y, NICE.x1, NICE.y1);
		// SDL_RenderClear(renderer);

		// SDL_SetRenderDrawColor(renderer,0,0,0,255);
		// SDL_RenderDrawLine(renderer,NICE.x0,NICE.y0,NICE.x1,NICE.y1);

		SDL_RenderFillRect(renderer,player_p);


		SDL_RenderPresent(renderer);
		// SDL_Delay(10);
	}
	return 0;
}
