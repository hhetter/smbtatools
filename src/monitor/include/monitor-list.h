struct monitor_item {
	int id;
	int xpos;
	int ypos;
	enum monitor_fn type;
	char *data;

	/* for monitors that have to handle throughput, like TOTAL */
	unsigned long int thrput;
	unsigned long int oldval;

	struct monitor_item *next;
	int changed;
	char *title;
	struct backlog_list *backlog;
};

void monitor_list_initial_draw();
void monitor_list_init();
void monitor_list_change_results( char *data );
struct monitor_item *monlist_end;
struct monitor_item *monlist_start;
int monitor_list_add( int id, enum monitor_fn type,int xpos, int ypos, char *title );
struct monitor_item *monitor_list_get_by_id( int id );
void monitor_list_print_changed();
