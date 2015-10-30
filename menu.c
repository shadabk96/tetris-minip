#include <ncurses.h>
#include "alldefine.h"

void update_inst();

WINDOW *menuw, *miw;

void print_menu(WINDOW *menuw) {
	int i;
	char item[20];
char list[NITEMS][20] = {	"New Game",
				"Instructions",
				"High Scores",
				"About Developers",
				"Exit" };
//	menuw = newwin( 15, 25, 1, 1 ); // create a new window
	box( menuw, ACS_VLINE, ACS_HLINE ); // sets default borders for the window
     
	// now print all the menu items and highlight the first one
	for( i = 0; i < NITEMS; i++ ) {
		sprintf(item, "%s",  list[i]);
		mvwprintw( menuw, i+1, 2, "%s", item );
	}

	mvwprintw( menuw, 11, 2, "'%c' -> Up", CONTROL_UP);
	mvwprintw( menuw, 12, 2, "'%c' -> Down", CONTROL_DOWN);
	mvwprintw( menuw, 13, 2, "Press '%c' to select ",CONTROL_NEXT );
	wrefresh( menuw ); // update the terminal screen 
}
int menu() {     
    char list[NITEMS][20] = { 	"New Game",
				"Instructions",
				"High Scores",
				"About Developers",
				"Exit" };
    char item[20];
    int ch, dh, i = 0, width = 7;
 
    initscr(); // initialize Ncurses
    menuw = newwin( 15, 25, 1, 1 ); // create a new window
    box( menuw, ACS_VLINE, ACS_HLINE ); // sets default borders for the window
     
// now print all the menu items and highlight the first one
    for( i = 0; i < NITEMS; i++ ) {
        if( i == 0 ) 
            wattron( menuw, A_STANDOUT ); // highlights the first item.
        else
            wattroff( menuw, A_STANDOUT );
        sprintf(item, "%s",  list[i]);
        mvwprintw( menuw, i+1, 2, "%s", item );
    }

	mvwprintw( menuw, 11, 2, "'%c' -> Up", CONTROL_UP);
	mvwprintw( menuw, 12, 2, "'%c' -> Down", CONTROL_DOWN);
	mvwprintw( menuw, 13, 2, "Press '%c' to select ", CONTROL_NEXT);
	wrefresh( menuw ); // update the terminal screen
 
    i = 0;
    noecho(); // disable echoing of characters on the screen
    keypad( menuw, TRUE ); // enable keyboard input for the window.
    curs_set( 0 ); // hide the default screen cursor.
     
       // get the input
    while(ch = wgetch(menuw)){ 
         
                // right pad with spaces to make the items appear with even width.
            sprintf(item, "%s",  list[i]); 
            mvwprintw( menuw, i+1, 2, "%s", item ); 
              // use a variable to increment or decrement the value based on the input.
            switch( ch ) {
                case CONTROL_UP:
                            i--;
                            i = ( i < 0 ) ? (NITEMS - 1) : i;
                            break;
                case CONTROL_DOWN:
                            i++;
                            i = ( i > (NITEMS - 1) ) ? 0 : i;
                            break;
		case CONTROL_NEXT : 	if(i == 0) {
						int lvl = 0;
						miw = newwin( 10, 25, 1, 30 );
						mvwprintw(miw, 1, 2, "Select Level :");
						mvwprintw(miw, 5, 2, "'%c' -> level up", CONTROL_UP);
						mvwprintw(miw, 6, 2, "'%c' -> level down", CONTROL_DOWN);
						mvwprintw(miw, 7, 2, "'%c' -> select", CONTROL_NEXT);
						mvwprintw(miw, 8, 2, "'%c' -> go back", CONTROL_BACK);
						box(miw, ACS_VLINE, ACS_HLINE);
						wrefresh(miw);
						while( dh = wgetch(miw)) {
							switch( dh ) {
								case CONTROL_DOWN : lvl--;
									lvl = (lvl < 0) ? (NO_LEVELS - 1) : lvl;
									break;
								case CONTROL_UP : lvl++;
									lvl = ( lvl > (NO_LEVELS - 1) ) ? 0 : lvl;
									break;
								case CONTROL_BACK : 
									return -2;
								case CONTROL_NEXT :
									return lvl;
						}
						mvwprintw(miw, 2, 5, " %d ", lvl);
						wrefresh(miw);
					}
				}

				if(i == 1) {
					miw = newwin( 10, 20, 1, 30 );
					mvwprintw(miw, 1, 2, "Controls :");
					mvwprintw(miw, 3, 2, "Move Left -> j");
					mvwprintw(miw, 4, 2, "Move Right -> l");
					mvwprintw(miw, 5, 2, "Move down -> k");
					mvwprintw(miw, 6, 2, "Rotate -> i");
					mvwprintw(miw, 7, 2, "Pause Game -> p");
					mvwprintw(miw, 8, 2, "Quit game -> v");
					box(miw, ACS_VLINE, ACS_HLINE);	
					wrefresh(miw);
				}

				if(i == 2) {
				/*	miw = newwin( 10, 20, 1, 30 );
					wclear(miw);
					mvwprintw(miw, 2,  2, "\"No Highscore\"");
					wrefresh(miw);
				*/
				disp_score();
					clear();
					refresh();
					print_menu(menuw);
				}
				
				if(i == 3) {
					miw = newwin( 10, 20, 1, 30 );
					wclear(miw);
					mvwprintw(miw, 2,  2, "Head Developer :\n   \"Shadab Khan\"");
					wrefresh(miw);
				}

				if(i == 4) {
					delwin( menuw );
					endwin();
					return -1;
				}
				break;
            }
            // now highlight the next item in the list.
            wattron( menuw, A_STANDOUT );
             
            sprintf(item, "%s",  list[i]);
            mvwprintw( menuw, i+1, 2, "%s", item);
            wattroff( menuw, A_STANDOUT );
    }
 
    delwin( menuw );
    endwin();
}
