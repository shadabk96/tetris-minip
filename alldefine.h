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

#define WELL_HEIGHT	22
#define WELL_WIDTH	32
#define NO_LEVELS	10
#define NEXT_HEIGHT	10
#define NEXT_WIDTH	20
#define SCORE_HEIGHT	14
#define SCORE_WIDTH	32

#define CONTROL_UP	'i'
#define CONTROL_DOWN	'k'
#define CONTROL_LEFT	'j'
#define CONTROL_RIGHT	'l'
#define CONTROL_NEXT	's'
#define CONTROL_BACK	'a'

#define NITEMS		6
#define NO_LEVELS	10

/* Points are stored in three parts : points, lines, levels.
 * DOT is the structure defining the blocks : (y, x) coordinates, colour. 
 * SC_REC is the record of a score row : player_name, POINTS, next row pointer.
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


// Prototypes of all the functions used in the complete code :
void print_menu(WINDOW *menuw);
int menu();
char *yx2pointer(int y, int x);
void update_stat(POINTS points);
void update_inst();
void update_well();
void remove_row(int row);
int check_row(int row);
POINTS *check_lines(int start);
void fix_block(int y, int x, int type,int orient);
int check_pos(int y, int x, int type, int orient);
void draw_block(WINDOW *win, int y, int x, int type, int orient, char delete);
void update_next(int next, int del);
int drop_block(int type, int level);
void disp_score(char *message);
int store_score(POINTS argp);
void play_game(int level, int y);
void init_windows();
void initialise_colors();
POINTS *loadgame(int *curr, int *next);
void savegame(POINTS points, int curr, int next);
