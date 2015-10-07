#include <ncurses.h>
int main() {
	char c;
	initscr();
	noecho();
	
	WINDOW *w;
	w = newwin(20, 20, 0, 0);
	keypad(w, TRUE);
	init_pair(1, COLOR_BLACK, COLOR_RED);
	attron(COLOR_PAIR(1));
	mvwprintw(w, 1, 1, "000000000");
	wrefresh(w);
	while(c = getch() != 'q');
	delwin( w );
	endwin();
}
