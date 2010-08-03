#include <stdlib.h> 
#include <stdio.h>
#include "include/includes.h"

pthread_mutex_t monitor_list_lock;


/*
 * For query_list, we don't use talloc. We
 * need to remove single elements from the list,
 * making the use of malloc easier in this case
 */



/*
 * init the monitor system */
void monitor_list_init( ) {
        pthread_mutex_init(&monitor_list_lock, NULL);
}





int monitor_list_add( int id )
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

struct monitor_item *monitor_list_get_by_id( int id )
{
	pthread_mutex_lock(&monitor_list_lock);
	struct monitor_item *entry = monlist_start;
	while (entry != NULL) {
		if (id == entry->id) {
			pthread_mutex_unlock(&monitor_list_lock);
			return entry;
		}
		entry=entry->next;
	}
	pthread_mutex_unlock(&monitor_list_lock);
	return NULL;
}

void monitor_list_change_results( char *data )
{
	pthread_mutex_lock(&monitor_list_lock);
	struct monitor_item *entry;
	char *tmp;
	tmp = result_get_element(tmp,0,data);
	int id = atoi(tmp);
	entry = monitor_list_get_by_id(id);
	tmp = result_get_element(tmp,1,data);
	entry->data = strdup(tmp);
	entry->changed = 1;
	talloc_free(tmp);
	pthread_mutex_unlock(&monitor_list_lock);
}


