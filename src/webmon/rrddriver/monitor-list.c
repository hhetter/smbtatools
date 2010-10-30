#include "include/includes.h"

pthread_mutex_t monitor_list_lock;


/*
 * For monitor_list, we don't use talloc. We
 * need to remove single elements from the list,
 * making the use of malloc easier in this case
 */



/*
 * init the monitor system */
void monitor_list_init( ) {
        pthread_mutex_init(&monitor_list_lock, NULL);
}




/**
 * add a monitor item
 * int id 		-> id of the monitor received by smbtad
 * enum monitor_fn	-> Function of the monitor
 * int xpos, ypos	-> x and y position in a full curses window
 * char *title		-> title to be printed above the monitor data
 */
int monitor_list_add( int id, enum monitor_fn func )
{
	pthread_mutex_lock(&monitor_list_lock);

	struct monitor_item *entry;
	if (monlist_start == NULL) {
		monlist_start = (struct monitor_item *) malloc( sizeof(struct monitor_item));
		if (monlist_start == NULL) {
			printf("ERROR: could not malloc.\n");
			exit(1);
		}
		entry = monlist_start;
		monlist_end = entry;
		entry->id = id;
		entry->changed = 0;
		entry->data = NULL;
		entry->next = NULL;
		entry->type = func;
		pthread_mutex_unlock(&monitor_list_lock);
		return 0;
	}
	entry = (struct monitor_item *) malloc(sizeof(struct monitor_item));
	if (entry == NULL) {
		printf("ERROR: could not malloc.\n");
		exit(1);
	}
	monlist_end->next = entry;
	monlist_end = entry;
	entry->next = NULL;
	entry->data = NULL;
	entry->id = id;
	entry->changed = 0;
	pthread_mutex_unlock(&monitor_list_lock);
	return 0;
}


/**
 * return a struct monitor_item by a given id
 */
struct monitor_item *monitor_list_get_by_id( int id )
{
	struct monitor_item *entry = monlist_start;
	while (entry != NULL) {
		if (id == entry->id) {
			return entry;
		}
		entry=entry->next;
	}
	return NULL;
}


void monitor_list_change_results( char *data )
{
	pthread_mutex_lock(&monitor_list_lock);
	struct monitor_item *entry;
	char *ctx = talloc(NULL, char);
	char *tmp = NULL;
	tmp = result_get_element(ctx,0,data);
	int id = atoi(tmp);
	entry = monitor_list_get_by_id(id);
	tmp = result_get_element(ctx,1,data);
	entry->data = strdup(tmp);
	entry->changed = 1;
	switch(entry->type) {
/*	case MONITOR_ADD: ;
		visual_monitor_add(entry);
		break;
	case MONITOR_TOTAL: ;
		visual_monitor_total(entry);
		break;
	case MONITOR_THROUGHPUT: ;
		visual_monitor_throughput(entry);
		break;
*/
	case MONITOR_LOG: ;
		visual_monitor_log_calc(entry);
		break;
	default: ;
	}
	pthread_mutex_unlock(&monitor_list_lock);	
	talloc_free(ctx);
}

void monitor_list_print_changed() {
	pthread_mutex_lock(&monitor_list_lock);
	struct monitor_item *entry = monlist_start;
	printf("-----> monitor list begin <-----\n");
	while( entry != NULL) {
		if (entry->changed == 1) {
			printf("Entry id %i changed: %s\n",
				entry->id, entry->data);
			entry->changed = 0;
		}
		entry = entry->next;
	}
	printf("-----> monitor list end   <-----\n");
	pthread_mutex_unlock(&monitor_list_lock);
}
	

