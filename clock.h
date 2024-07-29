#ifndef CLOCK_H
#define CLOCK_H

#include <stdlib.h>
#include <stdio.h>

/*
    Clock defines a struct that hold time information and time display

    Obs: The clock time is increment each 1 second and it is controlled
    by Allegro timer event from main game loop
*/

typedef struct Clock{
    int hour;
    int minute;
    int second;
    char str[10];
    int x0;
    int y0;
    ALLEGRO_FONT * font;
    ALLEGRO_COLOR color;
    bool is_running;
}Clock;

void clock_reset(Clock* clk){
    clk->hour = 0;
    clk->minute = 0;
    clk->second = 0;
    clk->is_running = false;
}

Clock* clock_create(int x0, int y0, ALLEGRO_FONT * font, ALLEGRO_COLOR color){
    Clock* clk = (Clock*) malloc(sizeof(Clock));
    clock_reset(clk);
    clk->x0 = x0;
    clk->y0 = y0;
    clk->font = font;
    clk->color = color;

    return clk;
}

void clock_destroy(Clock* clk){
    al_destroy_font(clk->font);
    free(clk);
}

void clock_start(Clock* clk){
    clk->is_running = true;
}

void clock_stop(Clock* clk){
    clk->is_running = false;
}

int clock_total_secs(Clock* clk){
    return clk->second + 60*clk->minute + 3600*clk->hour;
}

void clock_increment(Clock* clk, int hour, int minute, int second){
    if(clk->is_running){
        clk->second += second;
        if(clk->second >= 60){
            clk->minute +=  clk->second/60;
            clk->second =+ clk->second%60;
        }
        if(clk->minute >= 60){
            clk->hour +=  clk->minute/60;
            clk->minute =+ clk->minute%60;
        }
    }
}

char* clock_str(Clock* clk){
    sprintf(clk->str,"%02d:%02d:%02d",clk->hour,clk->minute,clk->second);
    return clk->str;
}

void clock_draw(Clock* clk){
    //al_draw_justified_textf(clk->font, clk->x0 , 100, 30, 0,  ALLEGRO_ALIGN_CENTER , clock_str(clk));
    //al_draw_textf(clk->font, clk->color , clk->x0 , clk->y0, ALLEGRO_ALIGN_CENTER , clock_str(clk));
    al_draw_textf(clk->font, clk->color , clk->x0 , clk->y0, 0 , clock_str(clk));
}



#endif // CLOCK_H
