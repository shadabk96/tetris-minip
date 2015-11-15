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
#include "alldefine.h"

int GAME_HEIGHT = WELL_HEIGHT + 4, GAME_WIDTH = WELL_WIDTH + NEXT_WIDTH + 4 + 20;

// well_data defines the occupancy of well  in terms of a character array.
char **well_data;

//delay[] is the time delay in microseconds depending on the level no.
int delay[NO_LEVELS] = {1000000, 770000, 593000, 457000, 352000, 271000, 208000, 160000, 124000, 95000};

/* Defined all global windows : 1. Game window		: gamew
 * 				2. Well window		: wellw
 *				3. Stats window		: statw
 * 				4. Next window		: nextw
 *				5. Instructions window	: instw
 *				6. Last Window		: lastw
 *				7. Highscores Window	: scorew
 */
WINDOW *gamew, *wellw, *statw, *nextw, *instw, *lastw, *scorew;


/* block_data[types][orientations][dots]
 * defining the blocks :
 *		(y, x, color no.)
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
	return &well_data[y][x];
}

// All colour definitions are defined here :
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
	mvwprintw(instw, 3, 2, "Move Left -> %c", CONTROL_LEFT);
	mvwprintw(instw, 4, 2, "Move Right -> %c", CONTROL_RIGHT);
	mvwprintw(instw, 5, 2, "Move down -> %c", CONTROL_DOWN);
	mvwprintw(instw, 6, 2, "Rotate -> %c", CONTROL_UP);
	mvwprintw(instw, 7, 2, "Pause Game -> p");
	mvwprintw(instw, 8, 2, "Quit game -> v");
	box(instw, ACS_VLINE, ACS_HLINE);	
	wrefresh(instw);
}


//Redraws well
void update_well() {
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
	
	update_well();
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

// Saves game to savefile.csp
void savegame(POINTS points, int curr, int next) {
	FILE *fp;
	int i, j;
	char ch;

//Section for taking input.
//--------------------------------------------------------------------------------------------------
	scorew = newwin(GAME_HEIGHT - 2, GAME_WIDTH, 0, 0);
	box(scorew, ACS_VLINE, ACS_HLINE);
	wrefresh(scorew);

	mvwprintw(scorew, 10, 20,"Please enter the savefile codename : [");
	mvwprintw(scorew, 10, 20 + 10 + strlen("Please enter the savefile codename : ["),"]");
	mvwprintw(scorew, 13, 30,"Press '+' to continue");
	mvwprintw(scorew, 14, 30,"Press '-' for backspace");
	mvwprintw(scorew, 15, 30,"Press '/' to cancel");
	wrefresh(scorew);
	nodelay(stdscr, FALSE);
	int letter_count = 0;
	char tempname[11];
	for(i = 0; i < 10; i++)
		tempname[i] = ' ';
	tempname[10] = '\0';
	do {
		ch = wgetch(scorew);
		tempname[letter_count] = ch;
		if(ch == '-') {			
			tempname[letter_count ] = ' ';
			(letter_count > 0 ) ? ( letter_count = letter_count - 2) : letter_count;
			tempname[letter_count + 1] = ' ';	
		}
		if(ch == '/')
			return;
		letter_count++;
		wattrset(scorew, COLOR_PAIR(14));
		mvwprintw(scorew, 10, 21 + strlen("Please enter the savefile codename : ["),"%s", tempname);		
		wattroff(scorew, COLOR_PAIR(14));		
		mvwprintw(scorew, 10, 20 + 10 + strlen("Please enter the savefile codename : ["),"]");
		wrefresh(scorew);
	} while((ch != '+') && (letter_count != 10));
	if((ch == '+') || (ch == '#'))
		tempname[letter_count - 1] = ' ';
	if(ch == '#') {
		int status;
		status = remove(tempname);
		if(status == 0)
			mvwprintw(scorew, 18, 30,"File deleted successfully");	
		else	
			mvwprintw(scorew, 16, 30,"File doesn't exist");			
	}
	nodelay(stdscr, TRUE);
	wclear(scorew);
	wrefresh(scorew);
	update_inst();
//--------------------------------------------------------------------------------------------------


	fp = fopen(tempname, "w");
	for(i = 0; i < WELL_WIDTH; i++)
		for(j = 0; j < WELL_HEIGHT; j++) {
			ch = well_data[i][j];
			fputc(ch, fp);
		}
	ch = '\n';
	fputc(ch, fp);
	fprintf(fp, "%d\n", points.points);
	fprintf(fp, "%d\n", points.lines);
	fprintf(fp, "%d\n", points.level);
	fprintf(fp, "%d\n", curr);
	fprintf(fp, "%d\n", next);
	fclose(fp);
}

// Loads game from savefile.csp
POINTS *loadgame(int *curr, int *next) {
	FILE * fp;
	int i, j;
	char ch;
	POINTS *temp;
	temp = (POINTS *)malloc(sizeof(POINTS));

//Section for taking input.
//--------------------------------------------------------------------------------------------------
	scorew = newwin(GAME_HEIGHT - 2, GAME_WIDTH, 0, 0);
	box(scorew, ACS_VLINE, ACS_HLINE);
	wrefresh(scorew);

	mvwprintw(scorew, 10, 20,"Please enter the savefile codename : [");
	mvwprintw(scorew, 10, 20 + 10 + strlen("Please enter the savefile codename : ["),"]");
	mvwprintw(scorew, 13, 30,"Press '+' to continue");
	mvwprintw(scorew, 14, 30,"Press '-' for backspace");
	mvwprintw(scorew, 15, 30,"Press '#' to delete savefile");
	mvwprintw(scorew, 16, 30,"Press '/' to cancel");
	wrefresh(scorew);
	nodelay(stdscr, FALSE);
	int letter_count = 0;
	char tempname[11];
	for(i = 0; i < 10; i++)
		tempname[i] = ' ';
	tempname[10] = '\0';
	do {
		ch = wgetch(scorew);
		tempname[letter_count] = ch;
		if(ch == '-') {			
			tempname[letter_count ] = ' ';
			(letter_count > 0 ) ? ( letter_count = letter_count - 2) : letter_count;
			tempname[letter_count + 1] = ' ';	
		}
		letter_count++;
		if(ch == '#')
			break;
		if(ch == '/') {
			temp->points = -2;
			return temp;
		}
		wattrset(scorew, COLOR_PAIR(14));
		mvwprintw(scorew, 10, 21 + strlen("Please enter the savefile codename : ["),"%s", tempname);		
		wattroff(scorew, COLOR_PAIR(14));		
		mvwprintw(scorew, 10, 20 + 10 + strlen("Please enter the savefile codename : ["),"]");
		wrefresh(scorew);
	} while((ch != '+') && (letter_count != 10));
	if((ch == '+') || (ch == '#'))
		tempname[letter_count - 1] = ' ';
	if(ch == '#') {
		int status;
		status = remove(tempname);
		if(status == 0)
			mvwprintw(scorew, 18, 30,"File deleted successfully");	
		else	
			mvwprintw(scorew, 18, 30,"File doesn't exist");				
		ch = wgetch(scorew);
		nodelay(stdscr, TRUE);
		temp->points = -2;
		return temp;
	}
	nodelay(stdscr, TRUE);
	wclear(scorew);
	wrefresh(scorew);
	update_inst();
//--------------------------------------------------------------------------------------------------

	fp = fopen(tempname, "r");
	if(fp == NULL)
		temp->points = -1;
	else {
		for(i = 0; i < WELL_WIDTH; i++)
			for(j = 0; j < WELL_HEIGHT; j++) {
				well_data[i][j] = fgetc(fp);
			}
		fgetc(fp);
		fscanf(fp, "%d%d%d%d%d", &(temp->points), &(temp->lines), &(temp->level), curr, next);
	}
	return temp;
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
	if (0 == check_pos(y + 1, x, type, orient))			//check if game over
		return -1;
	if (0 == check_pos(y + 1, x + 1, type, orient))			//check if game over
		return -1;
	if (0 == check_pos(y + 1, x + 2, type, orient))			//check if game over
		return -1;
	if (0 == check_pos(y + 1, x + 3, type, orient))			//check if game over
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

			case CONTROL_LEFT:						//-------------------------------move left
				if(check_pos(y, x - 1, type, orient)) {
					draw_block(wellw, y, x, type, orient, 1);
					draw_block(wellw, y, --x, type, orient, 0);
				}
				break;

			case CONTROL_RIGHT:						//-------------------------------move right
				if(check_pos(y, x + 1, type, orient)) {
					draw_block(wellw, y, x, type, orient, 1);
					draw_block(wellw, y, ++x, type, orient, 0);
				}
				break;

			case CONTROL_UP:						//-----------------------------------rotate
				if (check_pos(y, x, type,	orient + 1 == 4 ? 0 : orient + 1)) {
					draw_block(wellw, y, x, type, orient, 1);
					++orient == 4 ? orient = 0 : 0;
					draw_block(wellw, y, x, type, orient, 0);
				}
				break;

			case CONTROL_DOWN:	//------------------------------------------------------------------------move down
				sel_ret = 0;
				break;

			case 'p':		//----------------------------------------------------------------------------pause
				{
					lastw = newwin(10, 20, WELL_HEIGHT - 10, WELL_WIDTH + NEXT_WIDTH + 4);
					wattrset(lastw, COLOR_PAIR(2));
					box(lastw, ' ', ' ');
					wattroff(lastw, COLOR_PAIR(2));
					mvwprintw(lastw, 1, 2, ".. Game Paused ..");
					mvwprintw(lastw, 3, 3, "Press 'p'");
					mvwprintw(lastw, 4, 3, "to continue");
					mvwprintw(lastw, 5, 3, "Press 's'");
					mvwprintw(lastw, 6, 3, "to save game");
					mvwprintw(lastw, 7, 3, "Press 'l'");
					mvwprintw(lastw, 8, 3, "to load game");
					wrefresh(lastw);
					dh = wgetch(wellw);
					if(dh == 's') {
						delwin(lastw);
						return -2;
					} 
					if(dh == 'l') {
						return -3;
					}
				}
				while(dh != 'p');
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
 * Can be called anytime.
 */
void disp_score(char *message) {
	nodelay(stdscr, FALSE);
	char ch, str[15];
	wclear(gamew);
	POINTS points;
	scorew = newwin(GAME_HEIGHT - 2, GAME_WIDTH, 0, 0);
	box(scorew, ACS_VLINE, ACS_HLINE);
	wrefresh(scorew);


	if(message != NULL) {
		wattrset(scorew, COLOR_PAIR(1));
		mvwprintw(scorew, 17 , 19, "## %s ##", message);
		wattroff(scorew, COLOR_PAIR(1));
	}
	FILE *fp;
	fp = fopen("highscores.txt", "r");
	if(fp == NULL) {
		mvwprintw(scorew, 10, 10, "Highscores file not found.");
		wrefresh(scorew);
		ch = getch();
		return;
	}
	
	wattrset(scorew, COLOR_PAIR(14));
	mvwprintw(scorew, 2, 23, "## Highscores ##");	
	mvwprintw(scorew, 19, 17, "## Press any key to exit ##");
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
int store_score(POINTS argp) {

	char ch, tempno[10];
	int i, rows = 0;
	SC_REC *score, *head, *prev;
	head = NULL;
	prev = NULL;

//----------------------------------------------------------------------reads from stored highscore file.
	FILE *fp;
	fp = fopen("highscores.txt", "r");
if(fp != NULL) {
	while((ch = fgetc(fp)) != EOF) {
		score = (SC_REC *)malloc(sizeof(SC_REC));
		score->next = NULL;
		if(head == NULL)
			head = score;		
		if(prev != NULL)
			prev->next = score;
		i = 0;
		while(ch != '\t') {		
			score->pname[i] = ch;
			ch = fgetc(fp);
			i++;
		}

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

//----------------------------------------------------------------------checks if current is in top 10.
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
	if(rows == 10)
		return 0;

//----------------------------------------------------------------------asks user for name.
	scorew = newwin(GAME_HEIGHT - 2, GAME_WIDTH, 0, 0);
	box(scorew, ACS_VLINE, ACS_HLINE);
	wrefresh(scorew);

	mvwprintw(scorew, 10, 20,"Please enter your name : [");
	mvwprintw(scorew, 10, 20 + 10 + strlen("Please enter your name : ["),"]");
	mvwprintw(scorew, 13, 30,"Press '+' to continue");
	mvwprintw(scorew, 14, 30,"Press '-' for backspace");
	mvwprintw(scorew, 15, 30,"Press '/' to cancel");
	wrefresh(scorew);
	nodelay(stdscr, FALSE);
	int letter_count = 0;
	char tempname[11];
	for(i = 0; i < 10; i++)
		tempname[i] = ' ';
	tempname[10] = '\0';
	do {
		ch = wgetch(scorew);
		tempname[letter_count] = ch;
		if(ch == '-') {			
			tempname[letter_count ] = ' ';
			(letter_count > 0 ) ? ( letter_count = letter_count - 2) : letter_count;
			tempname[letter_count + 1] = ' ';	
		}
		if(ch == '/')
			return 1;
		letter_count++;
		wattrset(scorew, COLOR_PAIR(14));
		mvwprintw(scorew, 10, 21 + strlen("Please enter your name : ["),"%s", tempname);		
		wattroff(scorew, COLOR_PAIR(14));		
		mvwprintw(scorew, 10, 20 + 10 + strlen("Please enter your name : ["),"]");
		wrefresh(scorew);
	} while((ch != '+') && (letter_count != 10));
	if(ch == '+')
		tempname[letter_count - 1] = ' ';
	strcpy(new.pname, tempname);

	new.points.points = argp.points;
	new.points.lines = argp.lines;
	new.points.level = argp.level;

//----------------------------------------------------------adds current score to the stored table of scores.
	if(rows == 0) {
		new.next = head;
		head = &new;
	}
	else {
		new.next = prev->next;
		prev->next = &new;
	}

	rows = 0;
	temp = head;
	while(temp->next) {
		prev = temp;
		temp = temp->next;
		rows++;
	}
	if(rows == 10) {
		prev->next = NULL;
		free(temp);
	}

//----------------------------------------------------------------------writes the table to highscores.txt file
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
void play_game(int level, int y) {

	POINTS points, tpts, *temp;
	int i, j, curr, next, count_lines_rem;
	char ch;

	well_data = (char **)malloc((sizeof(char *)) * WELL_WIDTH);
	for(i = 0; i < WELL_WIDTH; i++) {
		well_data[i] = (char *)malloc((sizeof(char)) * WELL_HEIGHT);
		for(j = 0; j < WELL_HEIGHT; j++)
			well_data[i][j] = 0;
	}
	points.points = 0;
	points.lines = 0;
	points.level = level;
	
	temp = &tpts;
	count_lines_rem = level * 3;

	curr = rand() % 7;

	update_stat(points);
	
	while(y > 0 || y == -3) {
		temp->points = 0;
		temp->lines = 0;
		update_next(curr, 1);
		next = rand() % 7;
		if(y == -3) {
			temp = loadgame(&curr, &next);
			if((temp->points == -1) || (temp->points == -2)) {
				char *message;
				message = (char *)malloc(strlen("No Savefile Found") + 1 );
				strcpy(message, "No Savefile Found");
				if(temp->points == -2)
					message = NULL;
				disp_score(message);
				delwin(wellw);
				endwin();
				return;
			}
			update_well();
			points.points = temp->points;
			points.lines = temp->lines;
			points.level = temp->level;
			update_stat(points);
		}
		update_next(next, 0);
		y = drop_block(curr, points.level);
	
		if(y == -2) {
			savegame(points, curr, next);
			return;
		}
	
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
				disp_score(NULL);
				delwin(wellw);
				endwin();
			}
			else if(ch == 'y') {
				if(store_score(points))
					disp_score(NULL);
				else {
					wattrset(lastw, COLOR_PAIR(15));
					mvwprintw(lastw, 7, 1, "Your score is not");
					mvwprintw(lastw, 8, 2, "in the Top 10");
					wattroff(lastw, COLOR_PAIR(15));
					wrefresh(lastw);
					ch = getch();
					disp_score(NULL);
				}
			}
		}
	}
	return;
}


// Initiliases the windows for the first time :
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

// The main function :
int main() {
	int ch, level, y;
	keypad(gamew, TRUE);
	initscr();
	while(1) {
		y = 1;
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
		if(play == -3) {
			y = -3;
			level = 0;
		}
		else 
			level = play;
		nodelay(stdscr, TRUE);
		play_game(level, y);
		
		clear();
		refresh();
	}
	delwin( gamew );
	endwin();
}
