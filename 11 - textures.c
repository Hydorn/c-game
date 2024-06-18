#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "assets/wall.h"


#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

#define MAP_SIZE 10
#define CELL_SIZE 60
#define TEXTURE_SIZE 16
#define PI 3.14159265

typedef uint8_t ui8;

static ui8 MAP[MAP_SIZE * MAP_SIZE] = {
    1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,2,0,0,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,1,0,2,0,0,0,2,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,2,0,0,1,
    1,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,
};

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* wallTexture = NULL;

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

void calc_cam_angle(double *angle, int x0, int y0, int x1, int y1) {
    // Calculate the angle between the two points
    *angle = atan2(-1*(y1 - y0), x1 - x0); 
    int x = fabs(x1-x0);
    int y = fabs(y1-y0);

    if (*angle < 0) {
        *angle += 2*PI;
    }
}

// x0 and y0 are player position
void ray_cast(int x0, int y0, double angle) {
    int rays = 600;
    float rayAngle = angle + PI/6;
    float rayAngleIncrement = (PI/3) / rays;

    for (int i = 0; i < rays; i++) { 
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
        int j = 0;

        // Var to check wich side was updated last.
        int side = 0;
        
         while (!hit && j < 100) {          
            if (h_dx < h_dy) {
                map_x += step_x;
                h_dx += hyp_dx;
                side = 0;
            } else {
                map_y += step_y;
                h_dy += hyp_dy; 
                side = 1;
            }

            j++;

            if (MAP[map_y * MAP_SIZE + map_x] > 0) {
                hit = MAP[map_y * MAP_SIZE + map_x];
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

        //Upgraded ray_length to fix cat_eye effect
        float ray_length2 = ray_length * cos(angle-rayAngle);

        // Render ray with collition
        SDL_SetRenderDrawColor(renderer,0,side ? 255 : 155,0,0); 
        SDL_RenderDrawLine(renderer, x0,y0,collition_x,collition_y);

        //Render 3D
        float line_height = ray_length==0? SCREEN_HEIGHT: fabs(SCREEN_HEIGHT/(ray_length2/100)); 
        int tile_width = ceil((SCREEN_WIDTH/2)/(float)rays);

        // Extract position in texture array
        float ty = 0;
        float tx;
        if (side){
            tx =(int)(collition_x/(CELL_SIZE/(float)TEXTURE_SIZE))%TEXTURE_SIZE;
            if(rayAngle>PI){ tx = (TEXTURE_SIZE-1)-tx;}
        } else {
            tx =(int)(collition_y/(CELL_SIZE/(float)TEXTURE_SIZE))%TEXTURE_SIZE;
            if(rayAngle>PI/2 && rayAngle<3*PI/2){ tx = (TEXTURE_SIZE-1)-tx;}
        }
        float ty_step = TEXTURE_SIZE/line_height;

        // Drawing wall with textures
        for (int y=0; y < line_height; y++){
            float shade = side ? 1.0 : 0.5;

            int pixel = ((int)ty * TEXTURE_SIZE + (int)tx)*3;
            int red = (hit==1?T_2:T_1)[pixel] * shade;
            int green = (hit==1?T_2:T_1)[pixel+1] * shade;
            int blue = (hit==1?T_2:T_1)[pixel+2] * shade;

            SDL_SetRenderDrawColor(renderer,red,green,blue,255);

            if(tile_width!=1){

                // Considereation to draw points when theres more pixels than rays.
                SDL_Point points[tile_width];

                for (int k = 0; k < tile_width; k++){
                    points[k].x = SCREEN_HEIGHT+(i*tile_width) + k;
                    points[k].y = (SCREEN_HEIGHT-line_height)/2 + y;
                }

                SDL_RenderDrawPoints(renderer,&points[0],tile_width);

            } else {
                SDL_RenderDrawPoint(renderer,SCREEN_HEIGHT+(i*tile_width),(SCREEN_HEIGHT-line_height)/2 + y);
            }
            ty += ty_step;
        }

   
        //Increment rayAngle for next iteration
        rayAngle = rayAngle - rayAngleIncrement;
    }
}

int main(){
    //Initialize windows and renderer
	SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("2D Map", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    struct player game_player;
    game_player.pos.x =  SCREEN_WIDTH/8;
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
                        case SDLK_d:
                            game_player.player_cam.camera_pos.x += 10;
                            game_player.pos.x += 10;
							break;
						case SDLK_LEFT:
                        case SDLK_a:
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
		SDL_RenderClear(renderer);
		draw_map();

        // Draw floor and ceiling
        SDL_Rect ceiling = {SCREEN_WIDTH/2,0, SCREEN_WIDTH/2,SCREEN_HEIGHT};
        SDL_Rect floor = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2,SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
		SDL_SetRenderDrawColor(renderer,110,190,255,255);
        SDL_RenderFillRect(renderer,&ceiling);
		SDL_SetRenderDrawColor(renderer,175,255,115,255);
        SDL_RenderFillRect(renderer,&floor);
        
        
        ray_cast(game_player.pos.x,game_player.pos.y, game_player.player_cam.angle);
        SDL_RenderPresent(renderer);
	}
	return 0;
}
