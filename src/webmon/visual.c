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
                case MONITOR_TOTAL: ;
                        visual_monitor_total(entry);
                        break;
                case MONITOR_THROUGHPUT: ;
                        visual_monitor_throughput(entry);
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


void visual_monitor_total(struct monitor_item *entry)
{
	WINDOW *win = newwin(3,26,entry->ypos,entry->xpos);
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
	WINDOW *win = newwin(3,26,entry->ypos,entry->xpos);
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

void visual_monitor_log_calc(struct monitor_item *entry)
{
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
                asprintf(&to_add,"%s: User %s wrote to file %s on share %s (%s).",timestamp,username,filename,share,domain);
                backlog_add_str(to_add,entry);
                break;
        case vfs_id_close:
                asprintf(&to_add,"%s: User %s closed file %s on share %s (%s).",timestamp,username,filename,share,domain);
                backlog_add_str(to_add,entry);
                break;
        case vfs_id_open:
                asprintf(&to_add,"%s: User %s opened file %s on share %s (%s).",timestamp,username,filename,share,domain);
                backlog_add_str(to_add,entry);
                break;
        case vfs_id_read:
        case vfs_id_pread:
                asprintf(&to_add,"%s: User %s read from file %s on share %s (%s).",timestamp,username,filename,share,domain);
                backlog_add_str(to_add,entry);
                break;
        default: ;
        }
        if (to_add != NULL) free(to_add);
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
