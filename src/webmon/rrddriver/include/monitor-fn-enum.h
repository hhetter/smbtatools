


/*
 * The monitor protocol
 *
 * Function Number, RW flag, file, user, share, domain
 *
 */


enum monitor_fn {
	MONITOR_ADD = 0, 	/* Simple Adder, begins with 0, for testing */
	MONITOR_TOTAL,		/* Show the totals of an object */
	MONITOR_THROUGHPUT,	/* Throughput per Second */
	MONITOR_SMBTA_LOAD,	/* sends smbta load in percent */
	MONITOR_LOG,		/* send a log of what happens in VFS */
	MONITOR_READ,		/* sends the bytes that have been read on a single function */
	MONITOR_WRITE,		/* sends the bytes that have been read on a single function */
};
	
