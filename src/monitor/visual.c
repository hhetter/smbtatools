/**
 * This contains the visualization of monitors
 *
 */
#include "include/includes.h"
#include <curses.h>
#include "include/vfs_smb_traffic_analyzer.h"

#define _GNU_SOURCE
#include <stdio.h>

int monitor_timer_flag = 0;
pthread_mutex_t monitor_timer_lock;
pthread_t thread2;

void monitor_timer( void *var);

void visual_list_initial_draw();

void visual_init(char *title)
{
        pthread_mutex_init(&monitor_timer_lock, NULL);
        pthread_create(&thread2,NULL,(void *)&monitor_timer,NULL);
        initscr();
        clear();
        box(stdscr,0,0);
        mvwprintw(stdscr,0,1,title);
        refresh();
}


void monitor_timer( void *var)
{
        pthread_detach(pthread_self());
        unsigned long int micro = 1000*1000; /* microseconds */
        unsigned long int div = micro / 20;
        while ( 1 == 1) {
                usleep(div);
                pthread_mutex_lock(&monitor_timer_lock);
                monitor_timer_flag = 1;
                pthread_mutex_unlock(&monitor_timer_lock);
                visual_list_initial_draw();
        }
}

/**
 * draw the monitors initially after being initialized
 */
void visual_list_initial_draw()
{
        struct monitor_item *entry = monlist_start;
        while (entry != NULL) {
                switch(entry->type) {
                case MONITOR_ADD: ;
                        visual_monitor_add(entry);
                        break;
                case MONITOR_READ:
		case MONITOR_WRITE: ;
                        visual_monitor_read_write(entry);
                        break;
                case MONITOR_LOG:
                        visual_monitor_log(entry);
                        break;
                default: ;
                }
        entry = entry -> next;
        }
}





/**
 * Draw function for the MONITOR_ADD monitor.
 */
void visual_monitor_add(struct monitor_item *entry)
{
	WINDOW *win = newwin(3,26,entry->ypos,entry->xpos);
	box(win,0,0);
	unsigned long int nr = 0;
	if (entry->data != NULL) nr = atol(entry->data);
	mvwprintw(win,0,1,entry->title);
	mvwprintw(win,1,1,"%u VFS ops",nr);
	wrefresh(win);
	delwin(win);
}


void visual_monitor_read_write(struct monitor_item *entry)
{
	WINDOW *win = newwin(4,26,entry->ypos,entry->xpos);
	box(win,0,0);
	unsigned long int nr = 0;
	if (entry->data != NULL) nr = atol(entry->data);

	char *mem,*mem2;
	mem = common_make_human_readable(NULL,nr);
        mvwprintw(win, 0, 1,entry->title);
        mvwprintw(win, 1, 1,"%s (%05u B)",mem,nr);
	mem2 = common_make_human_readable(NULL,entry->thrput);
	mvwprintw(win, 2, 1,"%s / second",mem2);
        wrefresh(win);
        delwin(win);

	if (entry->showtotal == 1) {
		WINDOW *win2 = newwin(4,26,entry->totaly,entry->totalx);
		box(win2,0,0);
		mem = common_make_human_readable(mem,entry->totalsum);
		mvwprintw(win2,0,1,entry->totaltitle);
		mvwprintw(win2,1,1,"%s (%05u B)",mem,entry->totalsum);
		// FIXME: Throughput w/ partners !!
		wrefresh(win2);
		delwin(win2);
	}
	talloc_free(mem);
	talloc_free(mem2);
}

void visual_monitor_log_calc(struct monitor_item *entry)
{
        char *ctx = talloc(NULL,char);
        char *tmp = result_get_monitor_element(ctx,0,entry->data);
        int id = (int) common_myatoi(tmp);
        char *username = result_get_monitor_element(ctx,1,entry->data);
        char *share = result_get_monitor_element(ctx,2,entry->data);
        char *filename = result_get_monitor_element(ctx,3,entry->data);
        char *domain = result_get_monitor_element(ctx,4,entry->data);
        char *timestamp = result_get_monitor_element(ctx,5,entry->data);

        char *to_add = NULL;
        switch(id) {
        case vfs_id_write:
        case vfs_id_pwrite:
                to_add = talloc_asprintf(ctx,
			"%s: User %s wrote to file %s on share %s (%s).",
			timestamp,
			username,
			filename,
			share,
			domain);
                backlog_add_str(to_add,entry);
                break;
        case vfs_id_close:
                to_add = talloc_asprintf(ctx,
			"%s: User %s closed file %s on share %s (%s).",
			timestamp,
			username,
			filename,
			share,
			domain);
                backlog_add_str(to_add,entry);
                break;
        case vfs_id_open:
                to_add = talloc_asprintf(ctx,
			"%s: User %s opened file %s on share %s (%s).",
			timestamp,
			username,
			filename,
			share,
			domain);
                backlog_add_str(to_add,entry);
                break;
        case vfs_id_read:
        case vfs_id_pread:
                to_add = talloc_asprintf(ctx,
			"%s: User %s read from file %s on share %s (%s).",
			timestamp,
			username,
			filename,
			share,
			domain);
                backlog_add_str(to_add,entry);
                break;
        default: ;
        }
	TALLOC_FREE(ctx);
}


void visual_monitor_log(struct monitor_item *entry)
{
	WINDOW *ma = newwin(14,78,entry->ypos,entry->xpos);
	WINDOW *win = subwin(ma,11, 70,entry->ypos+1,entry->xpos+1);
	box(ma,0,0);
	wrefresh(ma);
	if (entry == NULL  || entry->data == NULL) {
		mvwprintw(win,1,1,"Empty Log.");
		wrefresh(win);
		delwin(win);
		return;
	}

	int count = 0;
	wmove(win,1,0);
	struct backlog_item *item = entry->backlog->end;
	while (item != NULL && count < 5) {
		count = count + 1;
		wprintw(win,"%s\n",item->data);
		item=item->before;
	}
	wrefresh(win);	
		

	delwin(win);
	delwin(ma);
}
