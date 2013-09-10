#ifndef __INTERFACE_H
#define __INTERFACE_H

void winds_quit();
void winds_init();
void main_wind();
void m_addstr();
void m_attron(int);
void m_attroff(int);
void m_printf(char *, ...);
int popwin_getrsp(void *);
void print_chat_comment(char *);
void* show_progress(void *);

#endif
