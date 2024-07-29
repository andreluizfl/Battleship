#ifndef BUTTON_H
#define BUTTON_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "clickable_component.h"

/*
    Button struct that contain a centralized text label and can process mouse click events

*/

typedef struct Button{
    CLICKABLE_COMPONENT cc;
    char text[32];
    ALLEGRO_FONT *font;
    ALLEGRO_COLOR color;
    float border_thickness;
}Button;

Button* button_new(int x0, int y0, int w, int h, char* text, ALLEGRO_FONT *font, ALLEGRO_COLOR color){
    Button* btn = (Button*) malloc(sizeof(Button));
    btn->cc.x0 = x0;
    btn->cc.y0 = y0;
    btn->cc.w = w;
    btn->cc.h = h;
    btn->font = font;
    btn->color = color;
    btn->text[0] = '\0';
    if(text)
        strcpy(btn->text,text);
    return btn;
}

void button_destroy(Button* btn){
    free(btn);
}

void button_draw(Button* btn){
    //clickbale_component_filled_draw(&btn->cc, al_map_rgb(220,220,220));
    clickbale_component_contour_draw(&btn->cc, btn->color,btn->border_thickness);

    int xlen = al_get_text_width(btn->font,btn->text);
    float del_x = btn->cc.w/2 - xlen/2;
    al_draw_justified_textf(btn->font, btn->color, btn->cc.x0+del_x, btn->cc.x0+del_x+btn->cc.w, btn->cc.y0, btn->cc.h , ALLEGRO_ALIGN_CENTER , btn->text);
}

#endif // BUTTON_H





