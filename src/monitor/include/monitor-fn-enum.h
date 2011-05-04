


/*
 * The monitor protocol
 *
 * Function Number, RW flag, file, user, share, domain
 *
 */


enum monitor_fn {
	MONITOR_ADD = 0, 	/* Simple Adder, begins with 0, for testing */
	MONITOR_SMBTA_LOAD,	/* sends smbta load in percent */
	MONITOR_LOG,		/* send a log of what happens in VFS */
	MONITOR_READ,		/* reacts when a READ/PREAD operation happened and */
				/* sends the number of bytes read */
	MONITOR_WRITE,		/* reacts when a WRITE/PWRITE operations happened and */
				/* sends the number of bytes written */
	MONITOR_UNDEFINED = 255
};
	
