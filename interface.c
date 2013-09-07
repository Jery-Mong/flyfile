#include <ncurses.h>
#include <panel.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>

#include "message.h"
#include "cmd.h"
#include "net.h"
#include "list.h"
#include "interface.h"

PANEL *pn_inf;
PANEL *pn_main;
PANEL *pn_prgs;

#define POPWIN_Y 12
#define POPWIN_X 50
	
void main_wind()
{
	char command[128];
	char **cmdl;
	
	WINDOW *win = panel_window(pn_main);
	//top_panel(pn_main);
	while (1) {
		waddstr(win, "(ff)");
		update_panels();
		doupdate();

		wgetstr(win, command);
		
		cmdl = cmdstr_to_list(command);
		if (cmdl != NULL) {
			cmd_handler(cmdl);
		}
	}
}

void m_addstr(char *str)
{
	WINDOW *win = panel_window(pn_main);
	waddstr(win, str);
	update_panels();
	doupdate();
}

void m_printf(char *fmt, ...)
{
	char buf[128];
	va_list ap;
	WINDOW *win = panel_window(pn_main);
	
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);

	wprintw(win, "%s", buf);
	update_panels();
	doupdate();
}

void m_attron(int attr)
{
	WINDOW *win = panel_window(pn_main);
	wattron(win, attr);
}
void m_attrof(int attr)
{
	WINDOW *win = panel_window(pn_main);
	wattroff(win, attr);
}
int popwin_getrsp(void *data)
{
	int i;
	WINDOW *win = panel_window(pn_inf);
	struct message *msg = (struct message *)data;
	wclear(win);
	
	show_panel(pn_inf);

	for (i = 0; i < POPWIN_X; i++)
		waddch(win, '=');
	
	wmove(win, POPWIN_Y -1, 0);
	
	for (i = 0; i < POPWIN_X; i++)
		waddch(win, '=');

	
	
	wmove(win, 1, POPWIN_X / 2 - 5);
	if (msg->type == MSG_FILE_RQST) {
		wprintw(win, "File Request\n\n");
		
		waddstr(win, "Requester: ");
		wattron(win, A_UNDERLINE);
		waddstr(win, msg->id.name);
		wattroff(win, A_UNDERLINE);

		waddstr(win, "\nName: ");
		wattron(win, A_UNDERLINE);
		waddstr(win,msg->ms_file.fname);
		wattroff(win, A_UNDERLINE);

		int size = msg->ms_file.fsize;

		waddstr(win, "\nSize :");
		wattron(win, A_UNDERLINE);
		if (size > 1024*1024 - 1)
			wprintw(win, "%.1f M\n", (float)size / (1024*1024));
		else if (size > 1024 - 1)
			wprintw(win, "%.1f K\n", (float)size / 1024);
		else
			wprintw(win, "%d B\n", size);
		wattroff(win, A_UNDERLINE);
		
	} else {
		wprintw(win, "Chat Request\n");
		wprintw(win, "Requester: %d\n", msg->id.name);
	}
	
	wprintw(win, "\nDo you aceept [Y/n]:");
	
	update_panels();
	doupdate();

	int ret = RSP_NO;
	switch (wgetch(win)) {
	case 'Y':
	case 'y':
		ret = RSP_YES;
	case 'N':
	case 'n':
		//case default:
		ret = RSP_NO;	
	}
	
	hide_panel(pn_inf);
	return ret;
}

void* show_progress(void *data)
{
	int i;
	int *prgs = (int *)data;
	WINDOW *win = panel_window(pn_prgs);
		
	show_panel(pn_prgs);

	wmove(win, 1, 0);
	for (i = 0; i < POPWIN_X; i++)
		waddch(win, '-');
	
	while (*prgs <= 100) {

		wmove(win, 0, 0);		
		wprintw(win, "%d%\n", *prgs);
		
		for (i = 0; i < *prgs / (100 / POPWIN_X); i++)
			waddch(win, ' ' | A_UNDERLINE);
		
		update_panels();
		doupdate();
		usleep(10000);
	}
	
	hide_panel(pn_prgs);
	return NULL;
}

void winds_init()
{
	WINDOW *win;
	initscr();

	win = newwin(2, POPWIN_X ,LINES / 2 - 2, COLS / 2 - POPWIN_X / 2);
	pn_prgs = new_panel(win);
	hide_panel(pn_prgs);
	
	win = newwin(POPWIN_Y, POPWIN_X ,LINES / 2 - POPWIN_Y / 2, COLS / 2 - POPWIN_X / 2);
	pn_inf = new_panel(win);
	hide_panel(pn_inf);
	
	win = newwin(LINES, COLS, 0, 0);
	pn_main = new_panel(win);

//	curs_set(0);
	scrollok(win, 1);
}

void winds_quit()
{
	del_panel(pn_inf);
	del_panel(pn_main);
	del_panel(pn_prgs);
	endwin();
}
/*
int main()
{
	face_init();

	pthread_t tid;
	int prgs = 0;

	struct message msg;

	msg.type = MSG_FILE_RQST;
	memcpy(msg.id.name, "mj", 3);
	memcpy(msg.ms_file.fname, "1.pdf", 6);
	msg.ms_file.fsize = 1024 * 1024;

	
	popwin_getrsp(&msg);

	main_wind();
	del_panel(pn_inf);
	del_panel(pn_main);

	endwin();
}
*/
