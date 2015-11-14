/****************************************************************************
 *		 	### SK's Tetris : Sketris ###
 *
 *  Copyright (C) 2015 Shadab Khan shadabk14.comp@coep.ac.in
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/> 
 *  or write to the Free Software Foundation Inc., 51 Franklin Street,
 *  Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include <ncurses.h>
#include "alldefine.h"

WINDOW *menuw, *miw, *titlew;

void print_menu(WINDOW *menuw) {
	
	wattrset(titlew, COLOR_PAIR(6));
	box( titlew, ACS_BULLET, ACS_BULLET ); // sets default borders for the window
	mvwprintw(titlew, 1, 1, " SK's Tetris : Sketris ");
	wattroff(titlew, COLOR_PAIR(6));
	wrefresh(titlew);
    
	int i;
	char item[20];
char list[NITEMS][20] = {	"New Game",
				"Load/Delete Game",
				"Instructions",
				"High Scores",
				"About Developers",
				"Exit" };
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

// Prints the main menu of the game

int menu() {
	char list[NITEMS][20] = { 	"New Game",
				"Load/Delete Game",
				"Instructions",
				"High Scores",
				"About Developers",
				"Exit" };
	char item[20];
	int ch, dh, i = 0, width = 7;

	initscr(); // initialize Ncurses
	menuw = newwin( 15, 25, 4, 1 ); // create a new window
	box( menuw, ACS_VLINE, ACS_HLINE ); // sets default borders for the window

	titlew = newwin( 3, 25, 1, 1);
	wattrset(titlew, COLOR_PAIR(6));
	box( titlew, ACS_BULLET, ACS_BULLET ); // sets default borders for the window
	mvwprintw(titlew, 1, 1, " SK's Tetris : Sketris ");
	wattroff(titlew, COLOR_PAIR(6));
	wrefresh(titlew);
    
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
			case CONTROL_NEXT : if(i == 0) {
						int lvl = 0;
						miw = newwin( 10, 45, 4, 30 );
						wrefresh(miw);
						miw = newwin( 10, 25, 4, 30 );
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
					return -3;
				}

				if(i == 2) {
					miw = newwin( 10, 45, 4, 30 );
					mvwprintw(miw, 1, 2, "Controls :");
					mvwprintw(miw, 3, 2, "Move Left -> j");
					mvwprintw(miw, 4, 2, "Move Right -> l");
					mvwprintw(miw, 5, 2, "Move down -> k");
					mvwprintw(miw, 6, 2, "Rotate -> i");
					mvwprintw(miw, 7, 2, "Pause Game -> p");
					mvwprintw(miw, 8, 2, "Quit game -> v");
					wrefresh(miw);
				}

				if(i == 3) {
				disp_score(NULL);
					clear();
					refresh();
					print_menu(menuw);
				}
				
				if(i == 4) {
					miw = newwin( 10, 45, 4, 30 );
					wclear(miw);
					mvwprintw(miw, 2,  2, "Game Developer :");
					mvwprintw(miw, 4,  2, "Contact info :");
					mvwprintw(miw, 6,  2, "For Source code visit :");
					wattrset(miw, COLOR_PAIR(12));
					mvwprintw(miw, 3,  2, "  \"Shadab Khan\"");
					mvwprintw(miw, 5,  2, "  \"shadabk14.comp@coep.ac.in\"");
					mvwprintw(miw, 7,  2, "  \"https://github.com/shadabk/tetris-minip\"");
					wattroff(miw, COLOR_PAIR(12));
					wrefresh(miw);
				}

				if(i == 5) {
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
