#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include "clock.h"

/*
    Player struct hold the profile information that can be saved or loaded from file.
    If Player doest exist then create a new profile
*/


typedef struct Player{
    char nickname[32];
    int won_games;
    int total_score;
    int lost_games;
    Clock time_played;
    char last_played[64];
}Player;

bool player_load(Player* p);

Player* player_new(char* nickname){
    Player* p = (Player*) malloc(sizeof(Player));
   strcpy(p->nickname,nickname);
    if(!player_load(p)){
        p->won_games=0;
        p->total_score=0;
        p->lost_games =0 ;
        clock_reset(&p->time_played);
        p->last_played[0] = '\0';
    }
    return p;
}

void player_destroy(Player* p){
    free(p);

}

int player_compare(Player* p1, Player* p2){
    return p2->total_score - p1->total_score;
}

bool player_load(Player* p){
    char discard[32];
    char filename[128] = "score/";
    mkdir(filename);
    sprintf(filename,"score/%s.txt",p->nickname);
    FILE* fd = fopen(filename,"r");
    if (fd){
        int h,m,s;
        fscanf(fd,"%s\t%[^\n]s\n",discard,p->nickname);
        fscanf(fd,"%s\t%d\n",discard,&p->won_games);
        fscanf(fd,"%s\t%d\n",discard,&p->total_score);
        fscanf(fd,"%s\t%d\n",discard,&p->lost_games);
        fscanf(fd,"%s\t%d:%d:%d\n",discard,&h,&m,&s);
        fscanf(fd,"%s\t%[^\n]s\n",discard,p->last_played);
        fclose(fd);
        p->time_played.hour = h;
        p->time_played.minute = m;
        p->time_played.second = s;
        return true;
    }else{
        return false;
    }
}

bool player_save(Player* p){
    //Player* p = NULL;
    char filename[128] = "score/";
    mkdir(filename);
    sprintf(filename,"score/%s.txt",p->nickname);
    FILE* fd = fopen(filename,"w");
    if (fd){
        fprintf(fd,"%s\t%s\n","Nickname:",p->nickname);
        fprintf(fd,"%s\t%d\n","Jogos_Ganhos:",p->won_games);
        fprintf(fd,"%s\t%d\n","Pontos_Total:",p->total_score);
        fprintf(fd,"%s\t%d\n","Jogos_Perdidos:",p->lost_games);
        fprintf(fd,"%s\t%s\n","Tempo_Médio_Partidas:",clock_str(&p->time_played));
        fprintf(fd,"%s\t%s\n","Ultima_Data_Jogada:",p->last_played);
        fclose(fd);
        return true;
    }
    return false;
}

//Updates all profile informations after each game played
void player_update_stats(Player* p, Clock* clk, int score, bool won_or_lost){
    int played_time = clock_total_secs(clk);
    int mean_played_time = clock_total_secs(&p->time_played);
    int total_time = mean_played_time*(p->won_games+p->lost_games);

    p->total_score+= score;
    if (won_or_lost){
        p->won_games++;
        p->total_score += 100;
    }else{
        p->lost_games++;
    }

    mean_played_time = (total_time+played_time)/(p->won_games+p->lost_games);

    clock_reset(&p->time_played);
    clock_start(&p->time_played);
    clock_increment(&p->time_played,0,0,mean_played_time);
    clock_stop(&p->time_played);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(p->last_played, 64, "%d %m %Y %H:%M", t);
}

#endif // PLAYER_H
