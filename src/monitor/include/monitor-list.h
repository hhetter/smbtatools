struct monitor_item {
	int id;
	int xpos;
	int ypos;
	enum monitor_fn type;
	char *data;

	/* for monitors that have to handle throughput and sums, like MONITOR_READ */
	unsigned long int thrput;
	unsigned long int oldval;
	unsigned long int sum;

	/* each MONITOR_READ or MONITOR WRITE can optionally show a total sum */
	/* which is the sum of it's own VFS op numbers plus the sum of a */
	/* "partner" given as a monitor item struct. */
	int showtotal; // 0 = normal op, 1 = show totals
	int totalx, totaly; // x and y coords where to display the totals
	struct monitor_item *total_partner; // "partner" struct
	char *totaltitle; // title for the total view
	unsigned long int totalsum; 

	struct monitor_item *next;
	int changed;
	char *title;
	struct backlog_list *backlog;
};

int monitor_item_set_total( struct monitor_item *item, int x, int y, char *title, struct monitor_item *partner);
void monitor_list_initial_draw();
void monitor_list_init();
void monitor_list_change_results( char *data );
struct monitor_item *monlist_end;
struct monitor_item *monlist_start;
int monitor_list_add( int id, enum monitor_fn type,int xpos, int ypos, char *title );
struct monitor_item *monitor_list_get_by_id( int id );
void monitor_list_print_changed();
