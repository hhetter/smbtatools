#include <stdlib.h> 
#include <stdio.h>
#include "include/monitor-list.h"

int monitor_list_add( int id )
{
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
		entry->data = NULL;
		entry->next = NULL;
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
	return 0;
}

struct monitor_item *monitor_list_get_by_id( int id )
{
	struct monitor_item *entry = monlist_start;
	while (entry != NULL) {
		if (id == entry->id) return entry;
		entry=entry->next;
	}
	return NULL;
}


