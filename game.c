#include <stdio.h>
#include <ncurses.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define WELL_HEIGHT	22
#define WELL_WIDTH	32
#define NO_LEVELS	10
#define NEXT_HEIGHT	10
#define NEXT_WIDTH	20
#define SCORE_HEIGHT	14
#define SCORE_WIDTH	32
int GAME_HEIGHT = WELL_HEIGHT + 4, GAME_WIDTH = WELL_WIDTH + NEXT_WIDTH + 4 + 20;

/* Points are stored in three parts : points, lines, levels
 *
 * DOT is the structure defining the blocks
 */
typedef struct points {
	unsigned int points;
	unsigned int lines;
	unsigned int level;
} POINTS;


typedef struct dot {
	unsigned char y;
	unsigned char x;
	unsigned char clr;
} DOT;

typedef struct sc_rec{
	char pname[11];
	POINTS points;
	struct sc_rec *next;
} SC_REC;


/* well_data defines the occupancy of well  in terms of a character array.
 * delay[] is the time delay in microseconds depending on the level no.
 */
char *well_data;
int delay[NO_LEVELS] = {1000000, 770000, 593000, 457000, 352000, 271000, 208000, 160000, 124000, 95000};
WINDOW *gamew, *wellw, *statw, *nextw, *instw, *lastw, *scorew;


/* block_data[types][orientations][dots]
 * defining the blocks
 * (y, x)
 */
const DOT block_data[7][4][4] =
{
	{
		{{2,0,1},{2,1,1},{2,2,1},{2,3,1}},	/*      */
		{{0,1,1},{1,1,1},{2,1,1},{3,1,1}},	/*      */
		{{2,0,1},{2,1,1},{2,2,1},{2,3,1}},	/* XXXX */
		{{0,1,1},{1,1,1},{2,1,1},{3,1,1}}	/*      */
	},
	{	
		{{1,1,2},{2,1,2},{1,2,2},{2,2,2}},	/*      */
		{{1,1,2},{2,1,2},{1,2,2},{2,2,2}},	/*  XX  */
		{{1,1,2},{2,1,2},{1,2,2},{2,2,2}},	/*  XX  */
		{{1,1,2},{2,1,2},{1,2,2},{2,2,2}}	/*      */
	},
	{
		{{1,0,3},{1,1,3},{1,2,3},{2,2,3}},	/*      */
		{{2,0,3},{0,1,3},{1,1,3},{2,1,3}},	/* XXX  */
		{{0,0,3},{1,0,3},{1,1,3},{1,2,3}},	/*   X  */
		{{0,1,3},{1,1,3},{2,1,3},{0,2,3}}	/*      */
	},
	{
		{{1,0,4},{2,0,4},{1,1,4},{1,2,4}},	/*      */
		{{0,0,4},{0,1,4},{1,1,4},{2,1,4}},	/* XXX  */
		{{1,0,4},{1,1,4},{0,2,4},{1,2,4}},	/* X    */
		{{0,1,4},{1,1,4},{2,1,4},{2,2,4}}	/*      */
	},
	{
		{{1,0,5},{1,1,5},{2,1,5},{2,2,5}},	/*      */
		{{1,0,5},{2,0,5},{0,1,5},{1,1,5}},	/* XX   */
		{{1,0,5},{1,1,5},{2,1,5},{2,2,5}},	/*  XX  */
		{{1,0,5},{2,0,5},{0,1,5},{1,1,5}}	/*      */
	},
	{	
		{{2,0,6},{1,1,6},{2,1,6},{1,2,6}},	/*      */
		{{0,0,6},{1,0,6},{1,1,6},{2,1,6}},	/*  XX  */
		{{2,0,6},{1,1,6},{2,1,6},{1,2,6}},	/* XX   */
		{{0,0,6},{1,0,6},{1,1,6},{2,1,6}}	/*      */
	},
	{
		{{1,0,7},{1,1,7},{2,1,7},{1,2,7}},	/*  X   */
		{{1,0,7},{0,1,7},{1,1,7},{2,1,7}},	/* XXX  */
		{{1,0,7},{0,1,7},{1,1,7},{1,2,7}},	/*      */
		{{0,1,7},{1,1,7},{2,1,7},{1,2,7}}	/*      */
	}
};		


// Converts (y, x) cordinates into pointer to particular location in well_data
char *yx2pointer(int y, int x) {
	return well_data + (y * WELL_WIDTH) + x;
}


//Redraws stat window
void update_stat(POINTS points) {
	box(statw, ACS_VLINE, ACS_HLINE);
	mvwprintw(statw, 2, 5, "Score : %d", points.points);
	mvwprintw(statw, 4, 5, "Lines : %d", points.lines);
	mvwprintw(statw, 6, 5, "Level : %d", points.level);
	mvwprintw(statw, 0, 6 , "# STATS #");
	wrefresh(statw);
}


//updates the instructions window
void update_inst() {
	mvwprintw(instw, 1, 2, "Controls :");
	mvwprintw(instw, 3, 2, "Move Left -> j");
	mvwprintw(instw, 4, 2, "Move Right -> l");
	mvwprintw(instw, 5, 2, "Move down -> k");
	mvwprintw(instw, 6, 2, "Rotate -> i");
	mvwprintw(instw, 7, 2, "Pause Game -> p");
	mvwprintw(instw, 8, 2, "Quit game -> v");
	box(instw, ACS_VLINE, ACS_HLINE);	
	wrefresh(instw);
}


//Redraws well
void update_well(int row) {
	int y = 0, x = 0, i;
	for(x = 0; x < WELL_WIDTH; x++) {
		for(y = 0; y < WELL_HEIGHT; y++) {
			if(*yx2pointer(y, x) > 0) {
				wattrset(wellw, COLOR_PAIR(*yx2pointer(y, x)));
				mvwprintw(wellw, y, 2 * x, "  ");
			}
			else {
				wattrset(wellw, COLOR_PAIR(9));
				mvwprintw(wellw, y, 2 * x, "  ");
			}
		}
	}
	wattroff(wellw, COLOR_PAIR(1));
	box(wellw, ACS_VLINE, ACS_HLINE);
	wrefresh(wellw);
}


// Removes the completely filled row from well
void remove_row(int row) {

	int i, j, k, x = 0, y = row;	
	for(x = 0; x < WELL_WIDTH; x++)
		*yx2pointer(y, x) = 0;
	
	for(i = row - 1; i > 0; i--) {
		for(x = 0; x < WELL_WIDTH/2; x++)
			*yx2pointer(i + 1, x) = *yx2pointer(i, x);
	}
	
	update_well(row);
}


// Checks if a particular row is filled
int check_row(int row) {
	int i, j, x = 0, y = row;
	for(x = 1; x < WELL_WIDTH/2; x++) {
		if(*yx2pointer(y, x) == 0)
			return 0;
	}
	return 1;
}


/* After fixing each block, checks if any lines are complete
 * If lines are complete, calls remove_row
 * Increases score according to number of lines removed in one turn.
 */ 
POINTS *check_lines(int start) {
	
	int y, count_row = 0;
	POINTS *temp = (POINTS *)malloc(sizeof(POINTS));
	temp->points = 0;
	temp->lines = 0;

	for (y = start; y < start + 4; y++)
		if(check_row(y)) {
			remove_row(y);
			count_row++;
		}
	switch(count_row) {

		case 1 : temp->points = 10;
			 temp->lines = 1;
			 break;

		case 2 : temp->points = 25;
			 temp->lines = 2;
			 break;

		case 3 : temp->points = 40;
			 temp->lines = 3;
			 break;

		case 4 : temp->points = 60;
			 temp->lines = 4;
			 break;

		default : break;
	}
	return temp;
}


// Fixes the block in a particular location in well
void fix_block(int y, int x, int type,int orient) {
	int i;
	DOT dot;
	for (i = 0; i < 4; i++) {
		dot = block_data[type][orient][i];
		*yx2pointer(y + dot.y, x + dot.x) = dot.clr;
	}
}


//checks if the block can be moved to the next location
int check_pos(int y, int x, int type, int orient) {
	int i;
	DOT dot;
	for(i = 0; i < 4; i++) {
		dot = block_data[type][orient][i];
		if ((y + dot.y > WELL_HEIGHT - 1)		||
		    (x + dot.x < 1) 				||
		    (x + dot.x > WELL_WIDTH/2 - 1)		||
		    ((*yx2pointer(y + dot.y, x + dot.x) > 0))
		   )
		        	return 0;
	}
	return 1;	
}


// Draws or erases the block depending on the delete value
void draw_block(WINDOW *win, int y, int x, int type, int orient, char delete) {	
	int i;
	DOT dot; 
		
	for (i = 0; i < 4; i++) {
		dot = block_data[type][orient][i];
		wattron(win, COLOR_PAIR(delete ? 9 : dot.clr));
		mvwprintw(win, y + dot.y, 2*(x + dot.x), "  ");
	}
	if (delete == 0)
		wrefresh(win);
	wattroff(win, COLOR_PAIR(delete ? 9 : 1));
	box(win, ACS_VLINE, ACS_HLINE);
	wrefresh(win);
}

//Redraws the window showing next block
void update_next(int next, int del) {
	mvwprintw(nextw, 0, 4, "NEXT BLOCK");
	if(del == 1)
		draw_block(nextw, 3, 3, next, 0, 1);
	if(del == 0)
		draw_block(nextw, 3, 3, next, 0, 0);
	box(nextw, ACS_VLINE, ACS_HLINE);	
	mvwprintw(nextw, 0, 3 , "# NEXT BLOCK #");
	wrefresh(nextw);

}


/* Drops block till it reaches either well floor or another line
 * Uses time variables
 * Controls the block while it falls
 */
int drop_block(int type, int level) {
	int mid = WELL_WIDTH / 2 - 2;
	int y = 0;
	int x = mid/2;
	int orient = 0;
	int pause = 0;
	char ch, dh;
	fd_set t1, t2;
	struct timeval timeout;
	int sel_ret;
	if (0 == check_pos(y, x, type, orient))			//check if game over
		return -1;

	timeout.tv_sec = 0;
	timeout.tv_usec = delay[level];

	FD_ZERO(&t1);			//initialise
	FD_SET(0, &t1);
	
	draw_block(wellw, y, x, type, orient, 0);
	
	while(1) {
		t2 = t1;
		sel_ret = select(FD_SETSIZE, &t2, (fd_set *) 0, (fd_set *) 0, &timeout);
		ch = getch();
		switch (ch) {

			case 'j':							//-------------------------------move left
				if(check_pos(y, x - 1, type, orient)) {
					draw_block(wellw, y, x, type, orient, 1);
					draw_block(wellw, y, --x, type, orient, 0);
				}
				break;

			case 'l':							//-------------------------------move right
				if(check_pos(y, x + 1, type, orient)) {
					draw_block(wellw, y, x, type, orient, 1);
					draw_block(wellw, y, ++x, type, orient, 0);
				}
				break;

			case 'i':							//-----------------------------------rotate
				if (check_pos(y, x, type,	orient + 1 == 4 ? 0 : orient + 1)) {
					draw_block(wellw, y, x, type, orient, 1);
					++orient == 4 ? orient = 0 : 0;
					draw_block(wellw, y, x, type, orient, 0);
				}
				break;

			case 'k':		//------------------------------------------------------------------------move down
				sel_ret = 0;
				break;

			case 'p':
				{
					lastw = newwin(10, 20, WELL_HEIGHT - 10, WELL_WIDTH + NEXT_WIDTH + 4);
					wattrset(lastw, COLOR_PAIR(2));
					box(lastw, ' ', ' ');
					wattroff(lastw, COLOR_PAIR(2));
					mvwprintw(lastw, 1, 2, ".. Game Paused ..");
					mvwprintw(lastw, 3, 3, "Press 'p'");
					mvwprintw(lastw, 4, 3, "to continue");
					wrefresh(lastw);
				}
				while((dh = wgetch(wellw)) != 'p');
				delwin(lastw);
				update_inst();
				break;

			case 'v':		//-----------------------------------------------------------------------------quit
				return -1;
		}
		
		if(sel_ret == 0) {
			if(check_pos(y + 1, x, type, orient)) {			//----------------------------------moves block down
				draw_block(wellw, y, x, type, orient, 1);
				draw_block(wellw, ++y, x, type, orient, 0);
			}
			else {							//--------------------------------fix block in place
				fix_block(y, x, type, orient);
				return y;
			}		
			timeout.tv_sec = 0;
			timeout.tv_usec = delay[level];
		}
	}
}


/* Displays the score from highscores.txt file
 * Can be called anytime
 */
void disp_score() {
	char ch, str[15];
	wclear(gamew);
	POINTS points;
	scorew = newwin(GAME_HEIGHT - 2, GAME_WIDTH, 0, 0);
	box(scorew, ACS_VLINE, ACS_HLINE);
	wrefresh(scorew);
	
	FILE *fp;
	fp = fopen("highscores.txt", "r");
	if(fp == NULL) {
		mvwprintw(scorew, 10, 10, "Highscores file not found.");
		wrefresh(scorew);
		ch = getch();
		return;
	}
	
	wattrset(scorew, COLOR_PAIR(14));
	mvwprintw(scorew, 2, 25, "## Highscores ##");
	wattroff(scorew, COLOR_PAIR(14));
	mvwprintw(scorew, 4, 10, "Player Name");
	mvwprintw(scorew, 4, 27, "Points");
	mvwprintw(scorew, 4, 37, "Lines");
	mvwprintw(scorew, 4, 47, "Level");
	mvwprintw(scorew, 6, 7, "1.");

	int i = 10, j = 6, rank = 2;
	while((ch = fgetc(fp)) != EOF) {
		if(ch == '\t') {
			i = (((i + 10) / 10 ) * 10);
			continue;
		}
		if(ch == '\n') {
			i = 10;
			j++;
			mvwprintw(scorew, j, 7, "%d.", rank++);	
			continue;
		}		
		mvwprintw(scorew, j, i, "%c", ch);
		i++;
		wrefresh(scorew);
	}

	ch = getch();
	nodelay(stdscr, TRUE);
}


/* Reads score, compares arg score with stored scores
 * Returns 1, if arg score can be stored and stores it
 * Return 0, if arg score is too low
 */
int store_score(POINTS argp) {/*
open highscores.txt
read the stored table
store read info in a data table
if(no. of scores saved < 10)
	store score
	closes file
	return 1
if(no. of scores saved > 10)
	compares score
	if possible
		store score
		closes file
		return 1
	else
		closes file

typedef struct sc_rec{
	char pname[11];
	POINTS points;
	struct sc_rec *next;
} SC_REC;
*/
	char ch, tempno[10];
char name[11];
	int i, rows = 0;
	SC_REC *score, *head, *prev;
	head = NULL;
	prev = NULL;
	FILE *fp;
	fp = fopen("highscores.txt", "r");
	if(fp != NULL)
		{

//	ch = fgetc(fp);
	while((ch = fgetc(fp)) != EOF) {
		score = (SC_REC *)malloc(sizeof(SC_REC));
	/*	for(i = 0; i < 10; i++) 
			score->pname[i] = ' ';		
	*/	score->next = NULL;
		if(head == NULL)
			head = score;		
		if(prev != NULL)
			prev->next = score;
		i = 0;
		while(ch != '\t') {		
			score->pname[i] = ch;
//name[i] = ch;
			ch = fgetc(fp);
			i++;
		}

//name[10] = '\0';
//strcpy(score->pname, name);
//score->pname[1] = name[1];
		(score->pname)[10] = '\0';
		i = 0;
		ch = fgetc(fp);
		while(ch != '\t') {
			tempno[i++] = ch;
			ch = fgetc(fp);			
		}
		tempno[i] = '\0';
		(score->points).points = atoi(tempno);
		i = 0;
		ch = fgetc(fp);
		while(ch != '\t') {
			tempno[i++] = ch;
			ch = fgetc(fp);		
		}
		tempno[i] = '\0';
		(score->points).lines = atoi(tempno);
		i = 0;
		ch = fgetc(fp);
		while((ch != '\n') && (ch != EOF)) {
			tempno[i++] = ch;
			ch = fgetc(fp);			
		}
		tempno[i] = '\0';
		(score->points).level = atoi(tempno);
		prev = score;
		rows++;
	}
	fclose(fp);
}
	rows = 0;
	SC_REC *temp, new;
	temp = head;
	while(temp) {
		if(temp->points.points > argp.points) {
			prev = temp;
			rows++;
		}
		temp = temp->next;
	}
	if(rows == 9)
		return 0;

///////////////////////////////////////////////
	strcpy(new.pname, "test");

	scorew = newwin(GAME_HEIGHT - 2, GAME_WIDTH, 0, 0);
	box(scorew, ACS_VLINE, ACS_HLINE);
	wrefresh(scorew);

	mvwprintw(scorew, 10, 20,"Please enter your name : [");
	mvwprintw(scorew, 10, 20 + 10 + strlen("Please enter your name : ["),"]");
	wrefresh(scorew);
	nodelay(stdscr, FALSE);
	int letter_count = 0;
	char tempname[11];
	for(i = 0; i < 10; i++)
		tempname[i] = ' ';
	tempname[10] = '\0';
	do {
		ch = wgetch(scorew);
		mvwprintw(scorew, 10, 21 + letter_count + strlen("Please enter your name : ["),"%c", ch);
		tempname[letter_count] = ch;
		letter_count++;
		wrefresh(scorew);
	}
	while((ch != '+') && (letter_count != 10));
	if(ch == '+')
		tempname[letter_count - 1] = ' ';
	strcpy(new.pname, tempname);

///////////////////////////////////////////////
	new.points.points = argp.points;
	new.points.lines = argp.lines;
	new.points.level = argp.level;

	if(rows == 0) {
		new.next = head;
		head = &new;
	}
	else {
		new.next = prev->next;
		prev->next = &new;
	}
///////////////////////////////////
rows = 0;
temp = head;
while(temp->next) {
	prev = temp;
	temp = temp->next;
	rows++;
}
if(rows == 10)
	prev->next = NULL;
free(temp);

///////////////////////////////////


	temp = head;
	fp = fopen("highscores.txt", "wr+");
	while(temp) {
	fprintf(fp, "%s", temp->pname);
	fprintf(fp, "\t");
	fprintf(fp, "%d", temp->points.points);
	fprintf(fp, "\t");
	fprintf(fp, "%d", temp->points.lines);
	fprintf(fp, "\t");
	fprintf(fp, "%d", temp->points.level);
	fprintf(fp, "\n");
	
	temp = temp->next;
	}
	fclose(fp);

		return 1;
}


/* Actual loop of the game.
 * initialises well_data, calls drop_block, sets score
 *
 */
void play_game(int level) {

	POINTS points, *temp;
	int i, curr, next, y, count_lines_rem;
	char ch;

	well_data = (unsigned char *)malloc(WELL_HEIGHT * WELL_WIDTH);
	for(i = 0; i < (WELL_HEIGHT * WELL_WIDTH); i++)
		well_data[i] = 0;

	points.points = 0;
	points.lines = 0;
	points.level = level;
	
	temp = &points;
	count_lines_rem = level * 3;

	curr = rand() % 7;

	update_stat(points);
	
	while(y != -1) {
		update_next(curr, 1);
		next = rand() % 7;
		update_next(next, 0);
		y = drop_block(curr, points.level);
		if(y > 0) 
			temp = check_lines(y);
		if(temp->points > 0) {
			points.points = points.points + temp->points;
			points.lines = points.lines + temp->lines;
			count_lines_rem++;
			points.level = (count_lines_rem / 3);
		}
		update_stat(points);
		curr = next;
		update_inst();

		if(y == -1) {
			lastw = newwin(10, 20, 8, 35);
			wattrset(lastw, COLOR_PAIR(4));
			box(lastw, ' ', ' ');
			wattroff(lastw, COLOR_PAIR(4));
			mvwprintw(lastw, 1, 2, "# Game Over #");
			mvwprintw(lastw, 2, 2, "<- Your Score ->");
			mvwprintw(lastw, 3, 4, "Score : %d", points.points);
			mvwprintw(lastw, 4, 4, "Lines : %d", points.lines);
			mvwprintw(lastw, 5, 4, "Level : %d", points.level);
			mvwprintw(lastw, 7, 2, "Save Score ? y/n");
			wrefresh(lastw);
			nodelay(stdscr, FALSE);
			ch = getch();
			if(ch == 'n') {
				delwin(wellw);
				endwin();
			}
			else if(ch == 'y') {
				if(store_score(points))
					disp_score();
				else {
			//		wattrset(lastw, COLOR_PAIR(5));
					mvwprintw(lastw, 7, 1, "Your score is not");
					mvwprintw(lastw, 8, 2, "in the Top 10");
			//		wattroff(lastw, COLOR_PAIR(5));
					wrefresh(lastw);
					ch = getch();
					disp_score();
				}
			}
		}	
	}
	return;
}


//initiliases the windows for the first time
void init_windows() {
	POINTS points;
	points.points = 0;
	points.lines = 0;
	points.level = 0;
	int STAT_HEIGHT = WELL_HEIGHT / 2;
	gamew = newwin( GAME_HEIGHT, GAME_WIDTH, 0, 0);
	wellw = newwin( WELL_HEIGHT + 1, WELL_WIDTH + 2, 1, 1 );
	statw = newwin( STAT_HEIGHT, NEXT_WIDTH, NEXT_HEIGHT + 2, WELL_WIDTH + 3 );
	nextw = newwin( NEXT_HEIGHT, NEXT_WIDTH, 2, WELL_WIDTH + 3 );
	instw = newwin( WELL_HEIGHT + 1, 20, 1, WELL_WIDTH + NEXT_WIDTH + 4 );
	update_stat(points);
	update_next(1, 1);
	update_inst();
	box(statw, ACS_VLINE, ACS_HLINE);
	wrefresh( wellw );
	wrefresh( statw );
}

void initialise_colors() {
	start_color();
	init_pair(9, COLOR_BLACK, COLOR_BLACK);
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(11, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_CYAN);
	init_pair(12, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_BLUE);
	init_pair(13, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_BLACK, COLOR_YELLOW);
	init_pair(14, COLOR_YELLOW, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);
	init_pair(15, COLOR_WHITE, COLOR_BLACK);
	init_pair(6, COLOR_BLACK, COLOR_GREEN);
	init_pair(16, COLOR_GREEN, COLOR_BLACK);
	init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
	init_pair(17, COLOR_MAGENTA, COLOR_BLACK);
}

int main() {
	int ch, level;
	keypad(gamew, TRUE);
	initscr();
	while(1) {
		initialise_colors();
		bkgd(COLOR_PAIR(9));
		refresh();
		int play = menu();
		if(play == -1) {delwin( gamew );
			endwin();
			return 0;}
		if(play == -2)
			continue;
		init_windows();
	
		nodelay(stdscr, TRUE);
		level = play;
		play_game(level);
		
		clear();
		refresh();
	}
	delwin( gamew );
	endwin();
}
