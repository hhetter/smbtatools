#include "include/includes.h"

int backlog_add(struct monitor_item *item)
{
	struct backlog_item *entry = (struct backlog_item *)
		malloc(sizeof(struct backlog_item));
	entry->next = NULL;
	entry->data = strdup(item->data);
	if (item->backlog->begin == NULL) {
		item->backlog->begin = entry;
		item->backlog->end =entry;
		item->backlog->begin->before = NULL;
	} else {
		entry->before = item->backlog->end;
		item->backlog->end->next = entry;
		item->backlog->end = entry;
	}
	
	item->backlog->backlog_count++;
	if (item->backlog->backlog_count == item->backlog->backlog_limit + 1) {
		struct backlog_item *backup = item->backlog->begin->next;
		free(item->backlog->begin->data);
		free(item->backlog->begin);
		item->backlog->begin = backup;
	}
	return 0;
}

int backlog_add_str(char *data, struct monitor_item *item)
{
        struct backlog_item *entry = (struct backlog_item *)
                malloc(sizeof(struct backlog_item));
        entry->next = NULL;
        entry->data = strdup(data);
        if (item->backlog->begin == NULL) {
                item->backlog->begin = entry;
                item->backlog->end =entry;
		item->backlog->begin->before = NULL;
        } else {
		entry->before = item->backlog->end;
                item->backlog->end->next = entry;
                item->backlog->end = entry;
        }

        item->backlog->backlog_count++;
        if (item->backlog->backlog_count == item->backlog->backlog_limit + 1) {
                struct backlog_item *backup = item->backlog->begin->next;
                free(item->backlog->begin->data);
                free(item->backlog->begin);
                item->backlog->begin = backup;
        }
        return 0;
}

