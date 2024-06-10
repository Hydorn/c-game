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
struct camera {
    struct position camera_pos;
    double angle;
};
struct player {
    struct position pos;
    struct camera player_cam;
};

void calc_cam_angle(double *angle, int x0, int y0, int x1, int y1) {
    // Calculate the angle between the two points
    *angle = atan2(-1*(y1 - y0), x1 - x0); 

    if (*angle < 0) {
        *angle += 2*PI;
    }
}

void draw_pos_ray(int x0, int y0, double angle){
    int size = 100;
    float x,y;
    x = cos(-1*angle) *size + x0;
    y = sin(-1*angle) *size + y0;
    SDL_RenderDrawLineF(renderer,x0+5,y0+5,x,y);
}

void draw_rays(int x0, int y0, double angle) {
    int size = 10000;
    int rays = 2;
    float rayAngleIncrement = 2.0 / rays;

    for (int i = 0; i < rays; i++) { 
        float rayAngle = (angle-0.5) + (i * rayAngleIncrement);
        float x = cos(-1*rayAngle) * size + x0;
        float y = sin(-1*rayAngle) * size + y0;
        SDL_RenderDrawLine(renderer, x0 + 5, y0 + 5, x, y); // Draw the ray
    }
}
int main(){
    //Initialize windows and renderer
	SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("2D Map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	
    //Create + init player
    struct player game_player;
    game_player.pos.x =  SCREEN_WIDTH/4;
    game_player.pos.y = SCREEN_HEIGHT/4;
		
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
                            game_player.pos.x += 10;
                            game_player.player_cam.camera_pos.x += 10;
							break;
						case SDLK_LEFT:
                        case SDLK_d:
                            game_player.pos.x -= 10;
                            game_player.player_cam.camera_pos.x -= 10;
							break;
						case SDLK_UP:
						case SDLK_w:
							game_player.pos.y -= 10;
                            game_player.player_cam.camera_pos.y -= 10;
							break;
						case SDLK_DOWN:
						case SDLK_s:
							game_player.pos.y += 10;
                            game_player.player_cam.camera_pos.y += 10; 
							break;
						case SDLK_ESCAPE:
							exit(0);
							break;
						default:
							break;
					}
					break;
                case SDL_MOUSEMOTION:
                    game_player.player_cam.camera_pos.x = event.motion.x;
                    game_player.player_cam.camera_pos.y = event.motion.y;
                    printf("camera x: %d - camera y: %d \n",event.motion.x,event.motion.y);
                    printf("player x: %d - player y: %d \n",game_player.pos.x,game_player.pos.y);
                    calc_cam_angle(&game_player.player_cam.angle, game_player.pos.x,game_player.pos.y, event.motion.x, event.motion.y);

                    printf("the angle is : %f", game_player.player_cam.angle);
                    break;
				default:
					break;
			}
		}

        // Draw map
		SDL_SetRenderDrawColor(renderer,255,125,0,255);
		SDL_RenderClear(renderer);
		draw_map();


		//Draw player
	    SDL_Rect player = {game_player.pos.x,game_player.pos.y,10,10};
		SDL_SetRenderDrawColor(renderer,255,125,0,255);
		SDL_RenderFillRect(renderer,&player);

        //Draw Ray
		SDL_SetRenderDrawColor(renderer,255,0,0,255);
        draw_pos_ray(game_player.pos.x,game_player.pos.y, game_player.player_cam.angle);

		SDL_SetRenderDrawColor(renderer,0,255,0,255);
        draw_rays(game_player.pos.x,game_player.pos.y, game_player.player_cam.angle);

		SDL_RenderPresent(renderer);
        SDL_Delay(10);
	}
	return 0;
}
