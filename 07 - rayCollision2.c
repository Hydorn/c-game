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
    1,1,1,1,0,0,0,0,0,1,
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
    int x = fabs(x1-x0);
    int y = fabs(y1-y0);

    if (*angle < 0) {
        *angle += 2*PI;
    }
}

void draw_pos_ray(int x0, int y0, double angle){
    int size = 100;
    float x,y;
    x = cos(-1*angle) *size + x0;
    y = sin(-1*angle) *size + y0;
    SDL_RenderDrawLineF(renderer,x0,y0,x,y);
}

// x0 and y0 are player position
void draw_rays(int x0, int y0, double angle) {
    int rays = 200;
    float rayAngle = angle + PI/6;
    float rayAngleIncrement = (PI/3) / rays;

    for (int i = 0; i <= rays; i++) { 
        // initial pos in grid
        int map_x = x0 / CELL_SIZE;
        int map_y = y0 / CELL_SIZE;

        // initial dif to the next grid
        int dx, dy;

        // unit hypotenuse variation
        float hyp_dx = fabs(CELL_SIZE/cos(rayAngle));
        float hyp_dy = fabs(CELL_SIZE/sin(rayAngle));

        // in which position are we moving
        int step_x, step_y;
        float h_dx, h_dy;

        // initialization
        if (cos(rayAngle) > 0) {
            step_x = 1;
            dx = (map_x + 1) * (step_x * CELL_SIZE) - x0;
            h_dx = fabs(dx/cos(rayAngle));
        } else {
            step_x = -1;
            dx = x0 - map_x * CELL_SIZE;
            h_dx = fabs(dx/cos(PI-rayAngle));

        }

        if (sin(rayAngle) > 0) {
            step_y = -1;
            dy = y0 - map_y * CELL_SIZE ;
            h_dy = fabs(dy/sin(rayAngle));
        } else {
            step_y = 1;
            dy = (map_y + 1) * CELL_SIZE  - y0;
            h_dy = fabs(dy/sin(PI-rayAngle));
        }

        int hit = 0;
        int i = 0;

        // Var to check wich side was updated last.
        int side = 0;
        
         while (!hit && i < 100) {          
            if (h_dx < h_dy) {
                map_x += step_x;
                h_dx += hyp_dx;
                side = 0;
            } else {
                map_y += step_y;
                h_dy += hyp_dy; 
                side = 1;
            }

            i++;

            if (MAP[map_y * MAP_SIZE + map_x] == 1) {
                hit = 1;
            } 
        }

        float collition_x, collition_y;
        float ray_length;

        if(side){
            if(sin(rayAngle)<0){
                collition_y = map_y * CELL_SIZE;
            } else {
                collition_y = (map_y + 1) * CELL_SIZE;
            }

            ray_length = fabs(collition_y-y0) / sin(rayAngle);
            collition_x = x0 + (cos(rayAngle)<0? -1:1) *  sqrt(ray_length*ray_length - fabs(collition_y-y0)*fabs(collition_y-y0));
            
        } else {
            if(cos(rayAngle)>0){
                collition_x = map_x * CELL_SIZE;
            } else {
                collition_x = (map_x + 1) * CELL_SIZE;
            }

            ray_length = fabs(collition_x-x0) / cos(rayAngle);
            collition_y = y0 + (sin(rayAngle)<0? 1:-1) * sqrt(ray_length*ray_length - fabs(collition_x-x0)*fabs(collition_x-x0));
        }

        // Render ray with collition
        SDL_RenderDrawLine(renderer, x0,y0,collition_x,collition_y);
        rayAngle = rayAngle - rayAngleIncrement;
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
                    calc_cam_angle(&game_player.player_cam.angle, game_player.pos.x,game_player.pos.y, event.motion.x, event.motion.y);
                    break;
				default:
					break;
			}
		}

        // Draw map
		SDL_SetRenderDrawColor(renderer,255,125,0,255);
		SDL_RenderClear(renderer);
		draw_map();


		//Draw player -5 so middle poit is the center
	    SDL_Rect player = {game_player.pos.x-5,game_player.pos.y-5,10,10};
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
