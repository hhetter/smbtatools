/**
 * This contains the visualization of monitors
 *
 */
#include "include/includes.h"
#include <curses.h>
#include "include/vfs_smb_traffic_analyzer.h"

#define _GNU_SOURCE
#include <stdio.h>

void visual_init(char *title)
{
	initscr();
	clear();
	box(stdscr,0,0);
	mvwprintw(stdscr,0,1,title);
	refresh();
}

/**
 * Draw function for the MONITOR_ADD monitor.
 */
void visual_monitor_add(struct monitor_item *entry)
{
	WINDOW *win = newwin(3,25,entry->ypos,entry->xpos);
	box(win,0,0);
	unsigned long int nr = 0;
	if (entry->data != NULL) nr = atol(entry->data);
	mvwprintw(win,0,1,entry->title);
	mvwprintw(win,1,1,"%u VFS ops",nr);
	wrefresh(win);
	delwin(win);
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

void visual_monitor_log(struct monitor_item *entry)
{
	WINDOW *main = newwin(14,75,entry->ypos,entry->xpos);
	WINDOW *win = subwin(main,11, 70,entry->ypos+1,entry->xpos+1);
	box(main,0,0);
	wrefresh(main);
	if (entry == NULL  || entry->data == NULL) {
		mvwprintw(win,1,1,"Empty Log.");
		wrefresh(win);
		delwin(win);
		return;
	}
	char *ctx = talloc(NULL,char);
	char *tmp = result_get_element(ctx,0,entry->data);
	int id = atoi(tmp);
	char *username = result_get_element(ctx,1,entry->data);
	char *share = result_get_element(ctx,2,entry->data);
	char *filename = result_get_element(ctx,3,entry->data);
	char *domain = result_get_element(ctx,4,entry->data);
	char *timestamp = result_get_element(ctx,5,entry->data);

	char *to_add = NULL;
	switch(id) {
	case vfs_id_write:
	case vfs_id_pwrite:
		asprintf(&to_add,"%s: User %s wrote to file %s on share %s",timestamp,username,filename,share);
		backlog_add_str(to_add,entry);
		break;
	case vfs_id_close:
		asprintf(&to_add,"%s: User %s closed file %s on share %s.",timestamp,username,filename,share);
		backlog_add_str(to_add,entry);
		break;
	case vfs_id_open:
		asprintf(&to_add,"%s: User %s opened file %s on share %s",timestamp,username,filename,share);
		backlog_add_str(to_add,entry);
		break;
	case vfs_id_read:
	case vfs_id_pread:
		asprintf(&to_add,"%s: User %s read from file %s on share %s",timestamp,username,filename,share);
		backlog_add_str(to_add,entry);
		break;
	default: ;
	}

	int count = 0;
	wmove(win,1,1);
	struct backlog_item *item = entry->backlog->end;
	while (item != NULL && count < 5) {
		count = count + 1;
		wprintw(win,"%s\n",item->data);
		item=item->before;
	}
	wrefresh(win);	
		

	if (to_add != NULL) free(to_add);	
	delwin(win);
	delwin(main);
}
