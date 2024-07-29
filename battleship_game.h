#ifndef BATTLESHIP_GAME_H
#define BATTLESHIP_GAME_H

#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdio.h>

#include "button.h"
#include "player.h"
#include "board.h"
#include "vessels.h"

/*
    BattleshipGame has a struct and all funcions that control the game flow.

*/

typedef enum GameState {
  MENU,             //Main Menu screen
  NICKNAME,         //Nickname entry screen
  DIFICULTY,        //Dificulty selection screen
  PLAYING,          //Playing game screen
  SCORE,            //Score Board screen
  WIN,              //Endgame with Victory
  LOST,             //Endgame with Defeat
  QUIT              //Quit game state
} GameState;

typedef struct BattleshipGame{
    // Display Properties
    ALLEGRO_DISPLAY *display;
    int WIDTH;
    int HEIGHT;

    //Background
    ALLEGRO_FONT *font_title;
	ALLEGRO_BITMAP *background_menu;
	ALLEGRO_BITMAP *background_board;

    //Main Menu Buttons
    Button* start;
    Button* score;
    Button* exit;
    ALLEGRO_FONT *menu_font;
    ALLEGRO_COLOR menu_color;

    //Nickname contents
    char nickname[32];
    ALLEGRO_FONT *font24;
    ALLEGRO_FONT *font36;

    //Sub Menu Buttons
    Button* easy_game;
    Button* hard_game;


    //Score board contents
    LList* highest_score;

    //Players
    Player* player1;
    Player* player2;

    // Boards
    Board* board_p1;
    Board* board_p2;

    //Game control
    bool your_turn;
    ALLEGRO_THREAD * computer_player;

    //Game Status
    GameState state;
}BattleshipGame;

//Automatic Routine to emulate computer playing
void *computer_play(ALLEGRO_THREAD *thr, void *arg){
    BattleshipGame* game = (BattleshipGame*)arg;
    if(game->state == PLAYING && !game->your_turn){
        int* pick_ij = board_pick_any_coordinate(game->board_p1,true);
        al_rest(rand()%5);
        if(board_attack(game->board_p1,pick_ij[0],pick_ij[1])){
            if (!board_has_ships(game->board_p1)){
                clock_stop(game->board_p1->clk);
                clock_stop(game->board_p2->clk);

                player_update_stats(game->player1, game->board_p1->clk,  game->board_p1->score, false);
                player_update_stats(game->player2, game->board_p2->clk,  game->board_p2->score, true);

                player_save(game->player1);
                player_save(game->player2);

                game->state = LOST;
                //End game - CPU WON!
            }else{
                //Pass the turn to Player
                game->your_turn = true;
                clock_stop(game->board_p2->clk);
                clock_start(game->board_p1->clk);
            }
        }
    }
    al_set_system_mouse_cursor(game->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    return NULL;
}

BattleshipGame* battleshipgame_create(ALLEGRO_DISPLAY *display){
    BattleshipGame* game = (BattleshipGame*) malloc(sizeof(BattleshipGame));

    game->display = display;
    game->WIDTH = al_get_display_width(display);
    game->HEIGHT = al_get_display_height(display);

    game->font_title = al_load_font("data/fonts/freeshipping.ttf", 60, 0);
	game->background_menu = al_load_bitmap("data/tiles/background.png");
	game->background_board = al_load_bitmap("data/tiles/background.png");
    game->menu_font = al_load_font("data/fonts/freeshipping.ttf", 45, 0);

    game->font24 = al_load_font("data/fonts/arial.ttf", 24, 0);
    game->font36 = al_load_font("data/fonts/arial.ttf", 36, 0);

    game->menu_color = al_map_rgb(220, 220, 220);
    game->menu_color = al_map_rgb(20, 20, 250);
    //game->menu_color = al_map_rgb(128, 255,128);

    game->start = button_new(game->WIDTH/2-80, (game->HEIGHT - 100)/2,       150, 50, "Start", game->menu_font, game->menu_color);
    game->score = button_new(game->WIDTH/2-150, (game->HEIGHT - 100)/2 + 100, 290, 50, "Score Board", game->menu_font, game->menu_color);
    game->exit =  button_new(game->WIDTH/2-80, (game->HEIGHT - 100)/2 + 200, 150, 50, "Exit", game->menu_font, game->menu_color);

    game->easy_game = button_new(game->WIDTH/2-80, (game->HEIGHT - 100)/2,       150, 50, "Easy", game->menu_font, game->menu_color);
    game->hard_game = button_new(game->WIDTH/2-80, (game->HEIGHT - 100)/2 + 100, 150, 50, "Hard", game->menu_font, game->menu_color);

    game->nickname[0]='\0';
    game->computer_player  = NULL;
    game->player1 = NULL;
    game->player2 = NULL;
    game->board_p1 = NULL;
    game->board_p2 = NULL;

    game->highest_score = llist_new((EventHandler_Compare)player_compare);

    srand(time(NULL));
    game->state = MENU;
    return game;
}

void battleshipgame_release(BattleshipGame* game){
    game->display = NULL;

    al_destroy_font(game->font_title);
    al_destroy_bitmap(game->background_board);
    al_destroy_bitmap(game->background_menu);
    al_destroy_font(game->menu_font);

    al_destroy_font(game->font24);
    al_destroy_font(game->font36);

    button_destroy(game->start);
    button_destroy(game->score);
    button_destroy(game->exit);

    button_destroy(game->easy_game);
    button_destroy(game->hard_game);

    if(game->player1){
        player_destroy(game->player1);
    }
    if(game->player2){
        player_destroy(game->player2);
    }
    if (game->board_p1){
        board_destroy(game->board_p1);
    }
    if (game->board_p2){
        board_destroy(game->board_p2);
    }

    llist_destroy_full(game->highest_score,(EventHandler_Destroy)player_destroy);

    if(game->computer_player)
        al_destroy_thread(game->computer_player);

    free(game);
}

//Check if game still running
bool battleshipgame_isrunning(BattleshipGame* game){
    return game->state!=QUIT;
}

//Quit state
void battleshipgame_quit(BattleshipGame* game){
    game->state=QUIT;
}

//Start nickname editing routine
void battleshipgame_nickname_start_edit(BattleshipGame* game){
    game->nickname[0]='\0';
}
//Add nickname char
void battleshipgame_nickname_add_char(BattleshipGame* game, char c){
    int pos = strlen(game->nickname);
    game->nickname[pos] = c;
    game->nickname[pos+1] = '\0';
}
//Delete last nickname char
void battleshipgame_nickname_del_char(BattleshipGame* game){
    game->nickname[strlen(game->nickname)-1] = '\0';
}
//End nickname editing routine
void battleshipgame_nickname_end_edit(BattleshipGame* game){
    game->state = DIFICULTY;
}

//Process the input interaction of mouse click
void battleshipgame_process_interaction(BattleshipGame* game, int x, int y){
    int i;

    switch(game->state){
        case MENU:
            if(check_click(game->exit, x,y)){
                battleshipgame_quit(game);
                return;
            }

            if(check_click(game->start, x,y)){
                game->state = NICKNAME;
                battleshipgame_nickname_start_edit(game);
                return;
            }

            if(check_click(game->score, x,y)){
                llist_clear_full(game->highest_score,(EventHandler_Destroy)player_destroy);
                char nickname[32];
                DIR *d;
                struct dirent *dir;
                d = opendir("./score");
                if(d){
                    while ((dir = readdir(d)) != NULL) {
                        if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0){
                            continue;
                        }
                        strcpy(nickname,dir->d_name);
                        for(i=0;i<strlen(nickname);i++){
                            if(nickname[i]=='.'){
                                nickname[i] ='\0';
                                break;
                            }
                        }
                        llist_add_ordered(game->highest_score,(CPointer)player_new(nickname));
                    }
                    closedir(d);
                }
                game->state = SCORE;
                return;
            }
            break;

        case NICKNAME:
            game->state = MENU;
            break;

        case DIFICULTY:
            if (check_click(game->easy_game,x,y)){
                if(game->player1){
                    player_destroy(game->player1);
                }
                game->player1 = player_new(game->nickname);

                if(game->player2){
                    player_destroy(game->player2);
                }
                game->player2 = player_new("Computer");

                if (game->board_p1){
                    board_destroy(game->board_p1);
                }
                game->board_p1 = board_create(10,10,22);

                board_place_vessel(game->board_p1, vessel_create(SUBMARINE));
                board_place_vessel(game->board_p1, vessel_create(SUBMARINE));
                board_place_vessel(game->board_p1, vessel_create(SUBMARINE));
                board_place_vessel(game->board_p1, vessel_create(SUBMARINE));

                board_place_vessel(game->board_p1, vessel_create(SEAPLANE));
                board_place_vessel(game->board_p1, vessel_create(SEAPLANE));
                board_place_vessel(game->board_p1, vessel_create(SEAPLANE));

                board_place_vessel(game->board_p1, vessel_create(CRUISER));
                board_place_vessel(game->board_p1, vessel_create(CRUISER));
                board_place_vessel(game->board_p1, vessel_create(CRUISER));

                board_toggle_reveal(game->board_p1, -1,-1, true);

                if (game->board_p2){
                    board_destroy(game->board_p2);
                }
                game->board_p2 = board_create(10,10,22);
                board_place_vessel(game->board_p2, vessel_create(SUBMARINE));
                board_place_vessel(game->board_p2, vessel_create(SUBMARINE));
                board_place_vessel(game->board_p2, vessel_create(SUBMARINE));
                board_place_vessel(game->board_p2, vessel_create(SUBMARINE));

                board_place_vessel(game->board_p2, vessel_create(SEAPLANE));
                board_place_vessel(game->board_p2, vessel_create(SEAPLANE));
                board_place_vessel(game->board_p2, vessel_create(SEAPLANE));

                board_place_vessel(game->board_p2, vessel_create(CRUISER));
                board_place_vessel(game->board_p2, vessel_create(CRUISER));
                board_place_vessel(game->board_p2, vessel_create(CRUISER));

                //board_toggle_reveal(game->board_p2, -1,-1, true);

                board_create_powerset(game->board_p1, true);

                game->your_turn = true;
                clock_start(game->board_p1->clk);
                game->state = PLAYING;

            }else{
                if (check_click(game->hard_game,x,y)){
                    if(game->player1){
                        player_destroy(game->player1);
                    }
                    game->player1 = player_new(game->nickname);

                    if(game->player2){
                        player_destroy(game->player2);
                    }
                    game->player2 = player_new("Computer");


                    if (game->board_p1){
                        board_destroy(game->board_p1);
                    }
                    game->board_p1 = board_create(15,15,22);

                    board_place_vessel(game->board_p1, vessel_create(CARRIER));

                    board_place_vessel(game->board_p1, vessel_create(DESTROYER));
                    board_place_vessel(game->board_p1, vessel_create(DESTROYER));

                    board_place_vessel(game->board_p1, vessel_create(CRUISER));
                    board_place_vessel(game->board_p1, vessel_create(CRUISER));
                    board_place_vessel(game->board_p1, vessel_create(CRUISER));

                    board_place_vessel(game->board_p1, vessel_create(SEAPLANE));
                    board_place_vessel(game->board_p1, vessel_create(SEAPLANE));
                    board_place_vessel(game->board_p1, vessel_create(SEAPLANE));

                    board_place_vessel(game->board_p1, vessel_create(SUBMARINE));
                    board_place_vessel(game->board_p1, vessel_create(SUBMARINE));
                    board_place_vessel(game->board_p1, vessel_create(SUBMARINE));
                    board_place_vessel(game->board_p1, vessel_create(SUBMARINE));

                    board_toggle_reveal(game->board_p1, -1,-1, true);

                    if (game->board_p2){
                        board_destroy(game->board_p2);
                    }
                    game->board_p2 = board_create(15,15,22);

                    //Place Vessels
                    board_place_vessel(game->board_p2, vessel_create(CARRIER));

                    board_place_vessel(game->board_p2, vessel_create(DESTROYER));
                    board_place_vessel(game->board_p2, vessel_create(DESTROYER));

                    board_place_vessel(game->board_p2, vessel_create(CRUISER));
                    board_place_vessel(game->board_p2, vessel_create(CRUISER));
                    board_place_vessel(game->board_p2, vessel_create(CRUISER));

                    board_place_vessel(game->board_p2, vessel_create(SEAPLANE));
                    board_place_vessel(game->board_p2, vessel_create(SEAPLANE));
                    board_place_vessel(game->board_p2, vessel_create(SEAPLANE));

                    board_place_vessel(game->board_p2, vessel_create(SUBMARINE));
                    board_place_vessel(game->board_p2, vessel_create(SUBMARINE));
                    board_place_vessel(game->board_p2, vessel_create(SUBMARINE));
                    board_place_vessel(game->board_p2, vessel_create(SUBMARINE));

                    //board_toggle_reveal(game->board_p2, -1,-1, true);

                    board_create_powerset(game->board_p1, true);

                    game->your_turn = true;
                    clock_start(game->board_p1->clk);
                    game->state = PLAYING;
                }else{
                    game->state = MENU;
                }
            }
            break;

        case SCORE:
            game->state = MENU;
            break;

        case PLAYING:
            if(game->your_turn){
                if(check_click(game->board_p2,x,y)){
                    int i,j;
                    i = board_get_i(game->board_p2, y);
                    j = board_get_j(game->board_p2,  x);

                    if(board_attack(game->board_p2,i,j)){
                        if (!board_has_ships(game->board_p2)){
                            clock_stop(game->board_p1->clk);
                            clock_stop(game->board_p2->clk);

                            player_update_stats(game->player1, game->board_p1->clk,  game->board_p1->score, true);
                            player_update_stats(game->player2, game->board_p2->clk,  game->board_p2->score, false);

                            player_save(game->player1);
                            player_save(game->player2);

                            game->state = WIN;
                            //End game - Player WON!
                        }else{
                            //Pass the turn to CPU
                            al_set_system_mouse_cursor(game->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
                            game->your_turn = false;
                            clock_stop(game->board_p1->clk);
                            clock_start(game->board_p2->clk);
                            if(game->computer_player){
                                al_destroy_thread(game->computer_player);
                            }
                            game->computer_player = al_create_thread(computer_play, game);
                            //al_rest(1);
                            al_start_thread(game->computer_player);
                        }
                    }
                }
            }
            break;

        case WIN:
            game->state = MENU;
            break;

        case LOST:
            game->state = MENU;
            break;

        case QUIT:
            //Do nothing. Wait to Quit
            break;
        default:
            break;
    }
}

//Draw backgorund
void battleshipgame_draw_brackground(BattleshipGame* game){
    if(game->state != PLAYING)
        if (game->background_menu) {
            // Draw background
            al_draw_bitmap(game->background_menu, 0, 0, 0);
        }else{
            al_clear_to_color(al_map_rgb(124, 74, 4));
        }
    else{
        if (game->background_board) {
            // Draw background
            al_draw_bitmap(game->background_board, 0, 0, 0);
        }else{
            al_clear_to_color(al_map_rgb(124, 74, 4));
        }
    }

    //int title_xlen = al_get_text_width(game->font_title,"Batalha_Naval");
    int title_xlen = 405;
    al_draw_textf(game->font_title, al_map_rgb(0, 222, 255), game->WIDTH/2- title_xlen/2, 25, 0, "Naval Battle");
}

//Increment time for current player
bool battleshipgame_increment_time(BattleshipGame* game, int i){
    if (game->state == PLAYING){
        clock_increment(game->board_p1->clk,0,0,1);
        clock_increment(game->board_p2->clk,0,0,1);
        return true;
    }
    return false;
}

//Draw main menu screen
void battleshipgame_main_menu(BattleshipGame* game){
    battleshipgame_draw_brackground(game);
    button_draw(game->start);
    button_draw(game->score);
    button_draw(game->exit);
}

//Draw nickname insertion panel screen
void battleshipgame_enter_nickname(BattleshipGame* game){
    battleshipgame_draw_brackground(game);
    int title_xlen = al_get_text_width(game->menu_font,game->nickname);

    title_xlen = al_get_text_width(game->menu_font,"Enter_Your_Nickname");
    al_draw_textf(game->menu_font, game->menu_color,game->WIDTH/2-title_xlen/2, (game->HEIGHT - 100)/2, 0, "Enter Your Nickname");

    title_xlen = al_get_text_width(game->font36,game->nickname);
    al_draw_textf(game->font36, game->menu_color,game->WIDTH/2-title_xlen/2, (game->HEIGHT - 100)/2 + 100, 0, game->nickname);

    title_xlen = al_get_text_width(game->menu_font,"Press_Enter");
    al_draw_textf(game->menu_font, game->menu_color,game->WIDTH/2-title_xlen/2, (game->HEIGHT - 100)/2+200, 0, "Press Enter");
}

//Draw dificulty selecion screen
void battleshipgame_dificulty_menu(BattleshipGame* game){
    battleshipgame_draw_brackground(game);
    button_draw(game->easy_game);
    button_draw(game->hard_game);
}

//Draw score board screen
void battleshipgame_score(BattleshipGame* game){
    battleshipgame_draw_brackground(game);
    int title_xlen = al_get_text_width(game->font36,"________________");
    int i;

    for (i=0; i<game->highest_score->count && i<10;i++){
        Player* p = (Player* ) llist_get(game->highest_score,i);
        al_draw_textf(game->font36, game->menu_color, game->WIDTH/2-title_xlen/2, game->HEIGHT/4 + 35*i, 0, "%d) %s - %d",i+1,p->nickname,p->total_score);
    }
}

//Draw endgame screen
void battleshipgame_endgame(BattleshipGame* game, char* message){
    battleshipgame_draw_brackground(game);
    int title_xlen = al_get_text_width(game->menu_font,message);
    al_draw_textf(game->menu_font, game->menu_color,game->WIDTH/2-title_xlen/2, (game->HEIGHT - 100)/2, 0, message);
}

//Draw playing screen
void battleshipgame_playing(BattleshipGame* game){
    battleshipgame_draw_brackground(game);
    int x0,y0,w,h;
    int xlen;
    float del_x;
    ALLEGRO_COLOR player_color = al_map_rgb(0,255,0);
    ALLEGRO_COLOR cpu_color = al_map_rgb(255,0,0);

    //Draw Player 1 nickname (with alignment)
    x0 = game->WIDTH/4 - board_get_width(game->board_p1)/2;
    y0 = game->HEIGHT/2 - board_get_height(game->board_p1)/2 -10;
    w = board_get_width(game->board_p1);
    h = 25;
    xlen = al_get_text_width(game->font24,game->player1->nickname);
    del_x = w/2 - xlen/2;
    al_draw_justified_textf(game->font24, player_color, x0+del_x, x0+del_x+w, y0-10, h , ALLEGRO_ALIGN_CENTER , game->player1->nickname);
    //Draw Player 1 board
    board_draw(game->board_p1, x0, y0+50);

    //Draw Player 2 (Computer) nickname (with alignment)
    x0 = 3*game->WIDTH/4 - board_get_width(game->board_p2)/2;
    y0 = game->HEIGHT/2 - board_get_height(game->board_p2)/2 - 10;
    w = board_get_width(game->board_p2);
    h = 25;
    xlen = al_get_text_width(game->font24,game->player2->nickname);
    del_x = w/2 - xlen/2;
    al_draw_justified_textf(game->font24, cpu_color, x0+del_x, x0+del_x+w, y0-10, h , ALLEGRO_ALIGN_CENTER , game->player2->nickname);
    //Draw Player 2 board
    board_draw(game->board_p2,x0,y0+50);
}

void battleshipgame_draw(BattleshipGame* game){
    switch(game->state){
        case MENU:
            battleshipgame_main_menu(game);
            break;
        case NICKNAME:
            battleshipgame_enter_nickname(game);
            break;
        case DIFICULTY:
            battleshipgame_dificulty_menu(game);
            break;
        case SCORE:
            battleshipgame_score(game);
            break;
        case PLAYING:
            battleshipgame_playing(game);
            break;
        case WIN:
            battleshipgame_endgame(game,"Victory!");
            break;
        case LOST:
            battleshipgame_endgame(game,"Defeat!");
            break;
        case QUIT:
            //Do nothing. Wait to Quit
            break;
        default:

            break;
    }
    al_flip_display();
}

#endif // BATTLESHIP_GAME_H
