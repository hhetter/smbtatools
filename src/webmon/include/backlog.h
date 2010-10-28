struct backlog_item {
	char *data;
	struct backlog_item *next;
	struct backlog_item *before;
};

struct backlog_list {
	struct backlog_item *begin;
	struct backlog_item *end;
	int backlog_limit;
	int backlog_count;
};

int backlog_add(struct monitor_item *item);
int backlog_add_str(char *data, struct monitor_item *item);
