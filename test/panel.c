#include <panel.h>
#include <unistd.h>
#include <ncurses.h>

int main()
{
	initscr();
	WINDOW *pop = newwin(12, 40, LINES / 2 - 6, COLS / 2 - 20);

	
	PANEL *pptr = new_panel(pop);

	WINDOW *mainw = newwin(LINES, COLS, 0, 0);
	PANEL *mp = new_panel(mainw);
	
	int i, j;
	for (i = 0; i < LINES; i++) {
		wmove(mainw, i, 0);
		for (j = 0; j < COLS; j++) {
			waddch(mainw, ' ' | A_REVERSE);
		}
		
	}
	
again:
//	waddch(mainw, 'w');
	top_panel(mp);
	update_panels();
	doupdate();

	sleep(1);
//	wmove(pop, 12/2, 20);
	//waddch(pop, i++);
	top_panel(pptr);

	update_panels();
	doupdate();
	sleep(1);
	goto again;
	

	del_panel(pptr);
	delwin(pop);
	endwin();
}
