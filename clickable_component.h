#ifndef CLICKABLE_COMPONENT_H
#define CLICKABLE_COMPONENT_H

/*
    CLICKABLE_COMPONENT is a struct containing positioning information of any drawing region
    so it can be clicked  by a mouse event

*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>

typedef struct CLICKABLE_COMPONENT{
    int x0;
    int y0;
    int w;
    int h;
}CLICKABLE_COMPONENT;

//Check if mouse coordinates click inside graphical component
bool check_click(void* clickable, int mouse_x, int mouse_y){
    CLICKABLE_COMPONENT* cc = (CLICKABLE_COMPONENT*)clickable;
    return (mouse_x > cc->x0 && mouse_x < (cc->x0+cc->w) && mouse_y > cc->y0 && mouse_y < (cc->y0+cc->h));
}

//Fill the clickable component region
void clickbale_component_filled_draw(CLICKABLE_COMPONENT* cc, ALLEGRO_COLOR color){
    al_draw_filled_rectangle(cc->x0, cc->y0, cc->x0+cc->w, cc->y0+cc->h, color);
}

//Draw contour of clickable component region
void clickbale_component_contour_draw(CLICKABLE_COMPONENT* cc, ALLEGRO_COLOR color, float thickness){
    al_draw_rectangle(cc->x0, cc->y0, cc->x0+cc->w, cc->y0+cc->h, color, thickness);
}

#endif // CLICKABLE_COMPONENT_H
