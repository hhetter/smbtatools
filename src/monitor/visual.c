/**
 * This contains the visualization of monitors
 *
 */
#include "include/includes.h"




/**
 * Draw function for the MONITOR_ADD monitor.
 */
void visual_monitor_add(struct monitor_item *entry)
{

	printf("%i\n",atoi(entry->data));
}
