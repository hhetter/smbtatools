#include <sqlite3.h>

enum smbta_monitor_object {
        SMBTA_SHARE = 0,
        SMBTA_USER,
        SMBTA_FILE
};



struct configuration_data {
        /* Number of the port to use */
        int port;
        /* hostname of server to connect to */
        char *host;
        /* debug level */
        int debug_level;
        /* configuration file */
        char *config_file;
        /* AES Key */
        unsigned char key[20];
        /* AES Keyfile */
        char *keyfile;
	/* 1, if a unix socket is used */
	int unix_socket;
        /* runtime data */
        int socket;
	/* when reading commands from a file, */
	/* use this filename		      */
	char *file;


        /* for smbtaquery */
        char *query;
        sqlite3 *db;

        /* for smbtamonitor */
        /* object type specifies the kind of object we are monitoring */
        enum smbta_monitor_object object_type;
        /* this stores the name of the object given by cmdline */
        char *object_name;
        /* this is the filter pattern to correctly monitor the object */
        char *pattern[5];

};



