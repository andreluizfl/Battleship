#ifndef BOARD_H
#define BOARD_H

#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "vessels.h"
#include "clock.h"
#include "clickable_component.h"
#include "llist.h"

#include <stdlib.h>
#include <time.h>

/*
    Board struct and functions that control vessel positioning and attack events

*/


enum SEA{
    WATER = 0,
};

typedef struct Board{
    CLICKABLE_COMPONENT cc;
    int rows;
    int cols;
    int** tiles;
    int** hitmap;
    int tile_size;
    LList* vessels;
    Clock* clk;
    int score;
    int sunk_ships;
    int hits;
    ALLEGRO_FONT *font_label;
    ALLEGRO_BITMAP *water_tile;
    ALLEGRO_BITMAP *missing_tile;
    ALLEGRO_BITMAP *bombing_tile;

    int* powerset_i;
    int* powerset_j;
    int available_positions;
    int picked_ij[2];
}Board;

void board_clear(Board* b);

//Board* board_create(int rows, int cols, int x0, int y0, int tile_size){
Board* board_create(int rows, int cols, int tile_size){
    int i,j;

    Board* b = (Board*) malloc(sizeof(Board));
    b->rows = rows;
    b->cols = cols;

    b->score = 0;
    b->sunk_ships=0;
    b->hits = 0;

    b->tiles = (int**) malloc(sizeof(int*)*rows);
    for(i=0;i<rows;i++){
        b->tiles[i] = (int*) malloc(sizeof(int)*cols);
        for(j=0;j<cols;j++){
            b->tiles[i][j] = 0;
        }
    }

    b->hitmap = (int**) malloc(sizeof(int*)*rows);
    for(i=0;i<rows;i++){
        b->hitmap[i] = (int*) malloc(sizeof(int)*cols);
        for(j=0;j<cols;j++){
            b->hitmap[i][j] = 0;
        }
    }

    b->font_label = al_load_font("data/fonts/arial.ttf", 16, 0);
    b->tile_size = tile_size;
    b->water_tile = al_load_bitmap("data/tiles/water.jpg");
    b->missing_tile =  al_load_bitmap("data/tiles/missing_bomb.png");
    if(b->missing_tile){
        al_convert_mask_to_alpha(b->missing_tile, al_map_rgb(255, 0, 255));
    }
    b->bombing_tile =  al_load_bitmap("data/tiles/explosion.png");
    if(b->bombing_tile){
        al_convert_mask_to_alpha(b->bombing_tile, al_map_rgb(255, 0, 255));
    }

    b->vessels = llist_new(NULL);
    b->clk = clock_create(0,0,al_load_font("data/fonts/arial.ttf", 20, 0),al_map_rgb(255,255,255));

    //Powerset of positions i and j. It helps computer to randomically select one position on board
    b->powerset_i = NULL;
    b->powerset_j = NULL;
    b->available_positions = 0;
    return b;
}

void board_destroy(Board* b){
    int i;
    for(i=0;i<b->rows;i++){
        free(b->tiles[i]);
    }
    free(b->tiles);

    for(i=0;i<b->rows;i++){
        free(b->hitmap[i]);
    }
    free(b->hitmap);

    if(b->water_tile){
        al_destroy_bitmap(b->water_tile);
    }
    if(b->missing_tile){
        al_destroy_bitmap(b->missing_tile);
    }
    if(b->bombing_tile){
        al_destroy_bitmap(b->bombing_tile);
    }

    llist_destroy_full(b->vessels , (EventHandler_Destroy)vessel_destroy);
    clock_destroy(b->clk);

    if(b->powerset_i){
        free(b->powerset_i);
    }
    if(b->powerset_j){
        free(b->powerset_j);
    }
    free(b);
}

void board_create_powerset(Board* b, bool shuffle){
    b->available_positions = b->rows*b->cols;
    b->powerset_i = malloc(sizeof(int)*b->available_positions);
    b->powerset_j = malloc(sizeof(int)*b->available_positions);
    int i,j,k = 0;
    for(i=0;i<b->rows;i++){
        for(j=0;j<b->cols;j++){
             b->powerset_i[k] = i;
             b->powerset_j[k] = j;
             k++;
        }
    }
    //Shuffle positions
    if (shuffle){
        int kr, temp;
        for(k=0;k<b->available_positions;k++){
            kr = rand()%b->available_positions;
            //Swap indexes
            temp = b->powerset_i[k];
            b->powerset_i[k] = b->powerset_i[kr];
            b->powerset_i[kr] = temp;

            temp = b->powerset_j[k];
            b->powerset_j[k] = b->powerset_j[kr];
            b->powerset_j[kr] = temp;
        }
    }
}

//Pick any coordinate from board. If powerset already randomized it doest need random=true
int* board_pick_any_coordinate(Board* b, bool random){
    int rpos;
    if (b->available_positions>0){
        if (random){
            rpos = rand()%b->available_positions;
            b->picked_ij[0] = b->powerset_i[rpos];
            b->picked_ij[1] = b->powerset_j[rpos];

            b->powerset_i[rpos] = b->powerset_i[b->available_positions-1];
            b->powerset_j[rpos] = b->powerset_j[b->available_positions-1];
        }else{
            rpos = b->available_positions-1;
            b->picked_ij[0] = b->powerset_i[rpos];
            b->picked_ij[1] = b->powerset_j[rpos];
        }
        b->available_positions--;
        return (int*)&b->picked_ij;
    }else{
        return NULL;
    }
}

int board_get_width(Board* b){
    return  b->cols*b->tile_size;
}
int  board_get_height(Board* b){
    return  b->rows*b->tile_size;
}

//Mouse mapping XY coordinates to IJ positions
int board_get_i(Board* b, int y){
    return (y-b->cc.y0)/b->tile_size;
}
int board_get_j(Board* b, int x){
    return (x-b->cc.x0)/b->tile_size;
}

//Check if has remaining ships on the board
bool board_has_ships(Board* b){
    return b->sunk_ships < b->vessels->count;
}

//Reveal or hide a vessel in position [i,j] or for all board using i=-1 and j=-1
void board_toggle_reveal(Board* b, int i, int j, bool reveal){
    int ii,jj;
    if (i==-1 && j==-1){
        for(ii=0;ii<b->rows;ii++){
            for(jj=0;jj<b->cols;jj++){
                board_toggle_reveal(b, ii, jj, reveal);
            }
        }
    }else{
        if ((reveal && b->tiles[i][j]<0) || (!reveal && b->tiles[i][j]>0)){
           b->tiles[i][j] = -b->tiles[i][j];
        }
    }
}

//Random positioning a vessel on board
bool board_place_vessel(Board* b, Vessel* v){
    int x_range = b->cols -v->w;
    int y_range = b->rows -v->h;
    int dx, dy, k;
    bool fit;
    int attempts = 50;

    do{
        fit = true;
        dx = rand()%(x_range);
        dy = rand()%(y_range);

        vessel_translate(v, dx, dy);
        for (k=0;k<v->type;k++){
            fit &= (b->tiles[v->i_pos[k]][v->j_pos[k]]==WATER);
        }
        attempts--;
    }while(!fit && (attempts>0));

    if (fit){
        llist_add_last(b->vessels,(CPointer)v);
        for (k=0;k<v->type;k++){
            b->tiles[v->i_pos[k]][v->j_pos[k]]= -(int)v->type;
        }
        b->hits+= v->type;
        b->score += 10*v->type;
    }
    return fit;
}

//Clear the board  (I think is not been used)
void board_clear(Board* b){
    int i,j;
    b->hits=0;
    b->score =0;
    b->sunk_ships=0;
    for(i=0;i<b->rows;i++){
        for(j=0;j<b->cols;j++){
            b->tiles[i][j] = WATER;
            b->hitmap[i][j] = 0;
        }
    }
    llist_clear_full(b->vessels, (EventHandler_Destroy)vessel_destroy);
}

//Perform a bomb attack in a given position [i,j]
bool board_attack(Board* b, int i, int j){
    int k, hit_k;
    Vessel* v;

    if (i<0 || i>=b->rows || j<0 || j>=b->cols || b->hitmap[i][j] != 0){
        //Invalid movement or coordinate: already missed or bombed
        return false;
    }else{
        if(b->tiles[i][j] == WATER){
            //Missed the bomb
            b->hitmap[i][j] = -1;
            return true;
        }else{
            for(k=0; k<b->vessels->count;k++){
                v = (Vessel*)llist_get(b->vessels,k);
                hit_k = vessel_try_hit(v, i, j);
                if (hit_k>=0){
                    board_toggle_reveal(b, i, j, true);
                    b->hitmap[i][j] = 1;
                    b->score -= 10;
                    if(vessel_isdesroyed(v)){
                        b->sunk_ships++;
                        //al_draw_textf(b->font_label, al_map_rgb(255,0,0),b->cc.x0+b->cc.w-50, b->cc.y0+50, ALLEGRO_ALIGN_CENTER, "Bomba Afundou!");
                    }
                    return true;
                }
            }
            return false;
        }
    }
}


//Draw board game
void board_draw(Board* b, int x, int y){
    int i,j;
    float x1,y1,x2,y2;

    b->cc.x0 = x;
    b->cc.y0 = y;
    b->cc.w  = board_get_width(b);
    b->cc.h = board_get_height(b);
    b->clk->x0 = x;
    b->clk->y0 = y+ board_get_height(b)+10;
    clock_draw(b->clk);

    ALLEGRO_COLOR score_color = al_map_rgb(255,255,255);
    ALLEGRO_COLOR label_color = al_map_rgb(255,255,255);

    al_draw_textf(b->font_label, score_color ,b->clk->x0+150, b->clk->y0, 0, "Score: %d",b->score);

    int x_d,y_d;
    int tile_size = b->tile_size;
    for(i=0;i<b->rows;i++){
        for(j=0;j<b->cols;j++){
            //Draw label coordinates
            if(i==0){
                x_d = x+i*tile_size-22;
                y_d = y+j*tile_size;
                al_draw_textf(b->font_label, label_color,x_d, y_d, 0, "%c",'A'+j);
            }
            if(j==0){
                x_d = x+i*tile_size ;
                y_d = y+j*tile_size - 22;
                al_draw_textf(b->font_label, label_color ,x_d, y_d, 0, "%d",i+1);
            }

            x1 = (float)x+j*tile_size;
            y1 = (float)y+i*tile_size;
            x2 = (float)x+(j+1)*tile_size;
            y2 = (float)y+(i+1)*tile_size;


            //Unrevealed Vessels
            if(b->tiles[i][j]<=0){
                //Draw Water Tile
                if (b->water_tile){
                    al_draw_bitmap_region(b->water_tile, x1, y1, tile_size, tile_size , x1, y1,0);
                }else{
                    al_draw_filled_rectangle(x1,y1,x2,y2, al_map_rgb(175, 225, 239));
                }
            }else{
                al_draw_filled_rectangle(x1,y1,x2,y2, vessel_color(b->tiles[i][j]));
                //Revealed Vessels
                if (b->tiles[i][j]>WATER){
                    al_draw_filled_rectangle(x1,y1,x2,y2, vessel_color(b->tiles[i][j]));
                }
            }

            if(b->hitmap[i][j]<0){
                //Bomb was missed
                if (b->missing_tile){
                    al_draw_scaled_bitmap(b->missing_tile,0,0,al_get_bitmap_width(b->missing_tile),al_get_bitmap_height(b->missing_tile),x1+2,y1+2,tile_size-2,tile_size-2,0);
                }else{
                    al_draw_line(x1,y1,x2,y2, al_map_rgb(255, 0, 0),3.0);
                    al_draw_line(x1,y1+tile_size,x2,y1, al_map_rgb(255, 0, 0),3.0);
                }
            }else{
                if(b->hitmap[i][j]>0){
                    //Bomb was target
                    if (b->bombing_tile){
                        al_draw_scaled_bitmap(b->bombing_tile,0,0,al_get_bitmap_width(b->bombing_tile),al_get_bitmap_height(b->bombing_tile),x1+2,y1+2,tile_size-2,tile_size-2,0);
                    }else{
                        al_draw_filled_rectangle(x1,y1,x2,y2, al_map_rgb(0, 0, 0));
                    }
                }
            }
            //Draw grid border
            al_draw_rectangle(x1,y1,x2,y2, al_map_rgb(0, 0, 0), 1.0);
        }
    }
    //Draw board border
    clickbale_component_contour_draw(&b->cc, al_map_rgb(0,0,0), 3.0);
}

#endif // BOARD_H
