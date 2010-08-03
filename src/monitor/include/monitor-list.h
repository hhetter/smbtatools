struct monitor_item {
	int id;
	char *data;
	struct monitor_item *next;
	int changed;
};

void monitor_list_init();
void monitor_list_change_results( char *data );
struct monitor_item *monlist_end;
struct monitor_item *monlist_start;
int monitor_list_add( int id );
struct monitor_item *monitor_list_get_by_id( int id );
void monitor_list_print_changed();
