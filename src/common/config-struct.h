#ifndef SQLITE_FOUND
        #include "../query/sqlite/sqlite3.h"
#else
        #include <sqlite3.h>
#endif

#include <dbi.h>

enum smbta_monitor_object {
        SMBTA_SHARE = 0,
        SMBTA_USER,
        SMBTA_FILE,
	SMBTA_DOMAIN,
	SMBTA_NONE
};

enum smbta_query_output {
	QUERY_ASCII = 0,
	QUERY_HTML = 1
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
        unsigned char key[25];
        /* AES Keyfile */
        char *keyfile;
	/* 1, if a unix socket is used */
	int unix_socket;
        /* runtime data */
        int socket;
	/* when reading commands from a file, */
	/* use this filename		      */
	char *file;
	/* if 0, identification is not done. This is */
	/* needed when smbtad is not maintaining the */
	/* database. */
	int identify;

        /* for smbtaquery */
        char *query;

	/* DBI support */
	/* host to connect to */
	char *dbhost;
	/* name of the database */
	char *dbname;
	/* user of the db */
	char *dbuser;
	/* driver for DBI */
	char *dbdriver;
	/* password of the user */
	char *dbpassword;

	/* for SQLITE support by DBI */
	char *dbpath;

	/* DBI connection */
	dbi_conn DBIconn;

	char *query_xmlfile;
	FILE *xml_handle;
	enum smbta_query_output query_output;
	sqlite3 *db;
        /* for smbtamonitor */
        /* object type specifies the kind of object we are monitoring */
        enum smbta_monitor_object object_type;
        /* this stores the name of the object given by cmdline */
        char *object_name;
        /* this is the filter pattern to correctly monitor the object */
        char *pattern[5];

	/* for rrdtool */
	char *database;
	int timer;
	char *rrdtool_setup;

};



