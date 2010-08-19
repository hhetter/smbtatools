/**
 * This contains the visualization of monitors
 *
 */
#include "include/includes.h"
#include <curses.h>

void visual_init(char *title)
{
	initscr();
	clear();
	box(stdscr,0,0);
	mvwprintw(stdscr,0,10,title);
	refresh();
}

/**
 * Draw function for the MONITOR_ADD monitor.
 */
void visual_monitor_add(struct monitor_item *entry)
{

	printf("%i\n",atoi(entry->data));
}


void visual_monitor_total(struct monitor_item *entry)
{
	WINDOW *win = newwin(3,25,entry->ypos,entry->xpos);
	box(win,0,0);
	unsigned long int nr = 0;
	if (entry->data != NULL) nr = atol(entry->data);

	char *mem;
	mem = common_make_human_readable(NULL,nr);
        mvwprintw(win,0, 1,entry->title);
        mvwprintw(win,1, 1,"%s (%05u B)",mem,nr);
        wrefresh(win);
        delwin(win);
	talloc_free(mem);
}

void visual_monitor_throughput(struct monitor_item *entry)
{
	WINDOW *win = newwin(3,25,entry->ypos,entry->xpos);
	box(win,0,0);
	unsigned long int nr = 0;
	if (entry->data != NULL) nr = atol(entry->data);
	char *mem;
	mem = common_make_human_readable(NULL,nr);
	wmove(win,entry->ypos + 2, entry->xpos+ 2);
	mvwprintw(win,0, 1,entry->title);
	mvwprintw(win,1, 1,"%s/s",mem);
	wrefresh(win);
	delwin(win);
	talloc_free(mem);
}
