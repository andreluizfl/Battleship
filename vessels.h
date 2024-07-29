#ifndef VESSELS_H
#define VESSELS_H
#include <stdlib.h>

/*
    Vessel defines a struct information about a series of types of vessels
*/

typedef enum VesselType{
    SUBMARINE = 1,
    CRUISER = 2,
    SEAPLANE = 3,
    DESTROYER = 4,
    CARRIER = 5
}VesselType;

typedef struct Vessel{
    VesselType type;
    int* i_kernel_pos;
    int* j_kernel_pos;
    int* i_pos;
    int* j_pos;
    int w;
    int h;
    int hits;
}Vessel;


Vessel* vessel_create(VesselType type){
    Vessel* v = (Vessel*)malloc(sizeof(Vessel));
    v->type = type;
    v->i_pos = (int*)malloc(sizeof(int)*(int)type);
    v->j_pos = (int*)malloc(sizeof(int)*(int)type);
    v->i_kernel_pos = (int*)malloc(sizeof(int)*(int)type);
    v->j_kernel_pos = (int*)malloc(sizeof(int)*(int)type);
    v->hits = 0;

    //Create the ship kernel with random orientation
    int k, orientation = rand()%2;
    switch(type){
        case SUBMARINE:
            v->i_kernel_pos[0] = 0;
            v->j_kernel_pos[0] = 0;
            break;
        case SEAPLANE:
            if(orientation){
                 v->i_kernel_pos[0] = 0;
                 v->j_kernel_pos[0] = 0;
                 v->i_kernel_pos[1] = 1;
                 v->j_kernel_pos[1] = 1;
                 v->i_kernel_pos[2] = 2;
                 v->j_kernel_pos[2] = 0;
            }else{
                 v->i_kernel_pos[0] =  1;
                 v->j_kernel_pos[0] = 0;
                 v->i_kernel_pos[1] = 0;
                 v->j_kernel_pos[1] = 1;
                 v->i_kernel_pos[2] = 1;
                 v->j_kernel_pos[2] = 2;
            }
            break;
        default:
            if(orientation){
                for (k=0;k<type;k++){
                    v->i_kernel_pos[k] = 0;
                    v->j_kernel_pos[k] = k;
                }
            }else{
                for (k=0;k<type;k++){
                    v->j_kernel_pos[k] = 0;
                    v->i_kernel_pos[k] = k;
                }
            }
            break;
    }

    v->w = 0;
    v->h = 0;
    for (k=0;k<type;k++){
        if (v->w < v->j_kernel_pos[k]){
            v->w = v->j_kernel_pos[k];
        }
        if (v->h < v->i_kernel_pos[k]){
            v->h = v->i_kernel_pos[k];
        }
    }
    return v;
}

void vessel_destroy(Vessel* v){
    free(v->i_pos);
    free(v->j_pos);
    free(v->i_kernel_pos);
    free(v->j_kernel_pos);
    free(v);
}

//Once the ship kernel is created the Vessel can be placed in any region of board game simply using 'vessel_translate' function
void vessel_translate(Vessel* v, int dx, int dy){
    int k;
    for (k=0;k<v->type;k++){
        v->i_pos[k] = v->i_kernel_pos[k]+dy;
        v->j_pos[k] = v->j_kernel_pos[k]+dx;
    }
}

ALLEGRO_COLOR vessel_color(VesselType type){
    switch(type){
        case SUBMARINE:
            return al_map_rgb(0,166,80);
            break;
        case CRUISER:
            return al_map_rgb(245,130,31);
            break;
        case SEAPLANE:
            return al_map_rgb(0,173,239);
            break;
        case DESTROYER:
            return al_map_rgb(237,27,35);
            break;
        case CARRIER:
            return al_map_rgb(254,242,0);
            break;
        default:
            return al_map_rgb(0,0,0);
            break;
    }
    return al_map_rgb(0,0,0);
}

//Check if vessel has been hit by a bomb at i_j coordinates and return its position index. If missed return -1
int vessel_try_hit(Vessel* v, int i, int j){
    int k;
    for (k=0;k<v->type;k++){
        if(v->i_pos[k] == i && v->j_pos[k] == j){
            v->hits++;
            return k;
        }
    }
    return -1;
}

//Check if vessel is destroyed. It means that all parts of vessel was hit by a bomb
bool vessel_isdesroyed(Vessel* v){
    return v->hits == v->type;
}

#endif // WARSHIP_H
