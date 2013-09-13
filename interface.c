#include <ncurses.h>
#include <panel.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h>
#include <pthread.h>
#include <signal.h>

#include "message.h"
#include "common.h"
#include "cmd.h"
#include "net.h"
#include "list.h"
#include "local.h"

PANEL *pn_inf;
PANEL *pn_main;
PANEL *pn_prgs;

#define POPWIN_Y 12
#define POPWIN_X 50

static int pop_flag = 0;
static int rsp = 0;

void main_wind()
{
	char command[128];
	char **cmdl;
	
	WINDOW *win = panel_window(pn_main);

	while (1) {		
		waddstr(win, "ff>>");
		
		update_panels();
		doupdate();
		wgetstr(win, command);
		
		if (pop_flag == 0) {
			cmdl = cmdstr_to_list(command);
			
			if (cmdl != NULL) {
				cmd_handler(cmdl);
			}
		} else {
			pop_flag = 0;
			
			if (command[0] == 'Y' || command[0] == 'y')
				rsp = RSP_YES;
			else
				rsp = RSP_NO;

			pthread_kill(self->msg_handler_thr, SIGUSR1);
			
			update_panels();
			doupdate();
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

static void display_popinf(struct message *msg)
{
	int i;
	WINDOW *win = panel_window(pn_inf);
	wclear(win);	

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
		waddstr(win,msg->msfile.fname);
		wattroff(win, A_UNDERLINE);

		int size = msg->msfile.fsize;

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
		wprintw(win, "Requester: %s\n", msg->id.name);
	}
	
	wprintw(win, "\nDo you aceept [Y/n]:");
	
	show_panel(pn_inf);
	update_panels();
	doupdate();
	hide_panel(pn_inf);

}
static void sig_do_nothing(int signo)
{
}
int popwin_getrsp(void *data)
{
	signal(SIGUSR1, sig_do_nothing);
	
	display_popinf((struct message *)data);
	
	pop_flag = 1;
	pause();
	
	int ret = rsp;
	
	return ret;
}
void* show_progress(void *data)
{
	int i ;
	int *per = (int *)data;
	int cur_per;
	
	WINDOW *win = panel_window(pn_prgs);	
	show_panel(pn_prgs);
	wclear(win);
	
	wmove(win, 1, 0);
	for (i = 0; i < POPWIN_X; i++) {
		waddch(win, '-');
	}
	
	while ((cur_per = *per) <= 100) {
		wmove(win, 1, 0);
		for (i = 0; i < cur_per / 2; i++)
			waddch(win, ' ' | A_REVERSE);

		wmove(win, 0, 0);
		wprintw(win, "%d%", cur_per);
		
		update_panels();
		doupdate();

		if (*per == 100)
			break;
		
		usleep(100000);
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
	
	scrollok(win, 1);
}

void winds_quit()
{
	del_panel(pn_inf);
	del_panel(pn_main);
	del_panel(pn_prgs);
	endwin();
}

void print_chat_comment(char *name, int idnum, char *comment)
{
	WINDOW *win = panel_window(pn_main);
	time_t tt;
	
        tt = time(NULL);
	
	wattron(win, A_BOLD);
	m_printf("\n%s(%d) ", name, idnum);
	m_printf("%s---%s", ctime(&tt) + 11, comment);
	wattroff(win, A_BOLD);
	
	update_panels();
	doupdate();
}
