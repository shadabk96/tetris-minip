#include <ncurses.h>
 
int menu() {
     
    WINDOW *w, *w2, *w1;
    char list[3][12] = { "New Game", "High Scores", "Exit" };
    char item[12];
    int ch, i = 0, width = 7;
 
    initscr(); // initialize Ncurses
    w = newwin( 11, 25, 1, 1 ); // create a new window
    box( w, ACS_VLINE, ACS_HLINE ); // sets default borders for the window
     
// now print all the menu items and highlight the first one
    for( i = 0; i < 3; i++ ) {
        if( i == 0 ) 
            wattron( w, A_STANDOUT ); // highlights the first item.
        else
            wattroff( w, A_STANDOUT );
        sprintf(item, "%s",  list[i]);
        mvwprintw( w, i+1, 2, "%s", item );
    }

    mvwprintw( w, 8, 2, "Press 's' to select " );
    wrefresh( w ); // update the terminal screen
 
    i = 0;
    noecho(); // disable echoing of characters on the screen
    keypad( w, TRUE ); // enable keyboard input for the window.
    curs_set( 0 ); // hide the default screen cursor.
     
       // get the input
    while(( ch = wgetch(w)) != 'q'){ 
         
                // right pad with spaces to make the items appear with even width.
            sprintf(item, "%s",  list[i]); 
            mvwprintw( w, i+1, 2, "%s", item ); 
              // use a variable to increment or decrement the value based on the input.
            switch( ch ) {
                case KEY_UP:
                            i--;
                            i = ( i<0 ) ? 2 : i;
                            break;
                case KEY_DOWN:
                            i++;
                            i = ( i>2 ) ? 0 : i;
                            break;
		case 's' : 	if(i == 0) {return 1;}
				if(i == 1) {mvwprintw(w, 5, 2, "\"no highscore\"");}
				if(i == 2) {delwin( w );
					endwin();
					return 0;}
				break;
            }
            // now highlight the next item in the list.
            wattron( w, A_STANDOUT );
             
            sprintf(item, "%s",  list[i]);
            mvwprintw( w, i+1, 2, "%s", item);
            wattroff( w, A_STANDOUT );
    }
 
    delwin( w );
    endwin();
}
