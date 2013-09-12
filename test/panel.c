#include <panel.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>

WINDOW *pop;
WINDOW *mainw;
PANEL *mp;
PANEL *pptr;


int pop_flag = 0;
int rsp_flag = 0;
pthread_t tid1, tid2;

void *main_func(void *data)
{
	
	char buf[32];
	while(1) {
		update_panels();
		doupdate();

		wgetstr(mainw, buf);
		if (pop_flag == 0) {
			wprintw(mainw, "%s\n", buf);
		} else {
			rsp_flag = buf[0];
			
			if (buf[0] == 'Y' || buf[0] == 'y' )
				rsp_flag = 1;
			else
				rsp_flag = 0;
			
			pop_flag = 0;
			pthread_kill(tid2, SIGUSR1);
		}
			
			
	}
}
void sig_nothing(int signo)
{
}
void *pop_func(void *data)
{
	char buf[32];

	show_panel(pptr);
	box(pop, '|', '-');	
	waddstr(pop, "\ndo you accept [Y/n]: ");
	update_panels();
	doupdate();
	pop_flag = 1;

	pause();

	wprintw(mainw, "your answer is %d\n", rsp_flag);
	update_panels();
	doupdate();

	hide_panel(pptr);
}


int main()
{
	initscr();
	pop = newwin(12, 40, LINES / 2 - 6, COLS / 2 - 20);
	pptr = new_panel(pop);
	hide_panel(pptr);

	mainw = newwin(LINES, COLS, 0, 0);
	mp = new_panel(mainw);
	
	signal(SIGUSR1, sig_nothing);
	pthread_create(&tid1, NULL, main_func, NULL);
	sleep(5);

	pthread_create(&tid2, NULL, pop_func, NULL);
	
	pause();
	del_panel(pptr);
	del_panel(mp);
	endwin();
}
