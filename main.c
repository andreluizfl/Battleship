#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_color.h>
#include <stdio.h>

#include "battleship_game.h"

//GLOBALS==============================
#define WIDTH 800
#define HEIGHT 600

int main(int argc, char *argv[])
{
    //Primitive variable
	bool redraw = true; //Redraw flag
	const int FPS = 1;  //Control de time flow of game. Its important to be 1 because of Clock time increment of 1 second

    //Allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

    // Initialization Allegro API modules
	if (!al_init()) //initialize Allegro
		return -1;
	if (!al_init_primitives_addon())
        return -1;
    if (!al_install_keyboard())
        return -1;
    if(!al_install_mouse())
        return -1;
    if(!al_init_font_addon())
        return -1;
    if(!al_init_ttf_addon())
        return -1;
    if(!al_init_image_addon())
        return -1;
    //al_install_audio();       //Not used
    //al_init_acodec_addon();   //Not used

    // Create Allegro API objects
    display = al_create_display(WIDTH, HEIGHT); //create our display object
	if(!display)    //test display object
		return -1;
	event_queue = al_create_event_queue();// create the event queue
	if(!event_queue)    //test event_queue object
		return -1;
	timer = al_create_timer(1.0/FPS);// create the timer
	if(!timer)    //test timer object
		return -1;

	//Set cursor system
    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

    // Initialize BattleshipGame Logic and Draw Context
    BattleshipGame* game = battleshipgame_create(display);
    battleshipgame_draw(game);

    // Register event sources
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_start_timer(timer);// start the timer
	// Game loop
	while(battleshipgame_isrunning(game)){

		ALLEGRO_MOUSE_STATE mouse_state;
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev); // fetch the event (if one exists)

		// Handle the event
        switch (ev.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = battleshipgame_increment_time(game,1);
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                battleshipgame_quit(game);
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                //Do Nothing
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                //Perform click interaction
                al_get_mouse_state(&mouse_state);
                if (mouse_state.buttons & 1) {
                    //printf("Mouse position 1: (%d, %d)\n", state.x, state.y);
                    battleshipgame_process_interaction(game, mouse_state.x, mouse_state.y);
                    redraw = true;
                }
                break;

            case ALLEGRO_EVENT_KEY_CHAR:
                //Perform keyboard interaction - Enter nickname
                if (game->state == NICKNAME){
                    switch(ev.keyboard.keycode){
                        case ALLEGRO_KEY_ENTER :
                            /// Enter string, process
                            battleshipgame_nickname_end_edit(game);
                            break;
                        case ALLEGRO_KEY_BACKSPACE :
                            /// Remove character before caret
                            battleshipgame_nickname_del_char(game);
                            break;
                        case ALLEGRO_KEY_DELETE :
                            /// Remove character at caret
                            //Not implemented
                            break;
                        default :
                            /// Add character to our string
                            battleshipgame_nickname_add_char(game,ev.keyboard.unichar);
                            break;
                    }
                    redraw = true;
                }
                break;

            default:
                //fprintf(stderr, "Unsupported event received: %d\n", ev.type);
                redraw = false;
                break;
        }

		// Check if we need to redraw
		if (redraw && al_is_event_queue_empty(event_queue)){
			// Perform Redraw
			redraw = false;
			battleshipgame_draw(game);
            al_flip_display();
		}
	}

	// Clean up memory
	battleshipgame_release(game);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	return 0;
}


