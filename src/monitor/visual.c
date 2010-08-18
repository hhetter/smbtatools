/**
 * This contains the visualization of monitors
 *
 */
#include "include/includes.h"


void visual_init()
{
	initscr();
	refresh();
}

/**
 * Draw function for the MONITOR_ADD monitor.
 */
void visual_monitor_add(struct monitor_item *entry)
{

	printf("%i\n",atoi(entry->data));
}


void visual_monitor_total(struct monitor_item *entry)
{
	printf("%i\n",atoi(entry->data));
}

void visual_monitor_throughput(struct monitor_item *entry)
{
	move(entry->ypos, entry->xpos);
	printw("%10u",atol(entry->data));
	refresh();
}
