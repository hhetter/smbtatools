#define __EXTENSIONS__

#define _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500
#define _GNU_SOURCE

#include "../../../include/version.h"
#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <libsmbclient.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/un.h>
#include <syslog.h>
#include <talloc.h>
#include <time.h>
#include <unistd.h>


struct configuration {
	int port;
	int sockfd;
	int verbose;
};

struct file_element {
	struct file_element *next;
	char *filename;
	int socket;
};

struct file_element *fnamelist_begin=NULL;
struct file_element *fnamelist_end=NULL;

struct conn_list {
	struct conn_list *next;
	int sockfd;
};

struct conn_list *conn_list_begin = NULL;
struct conn_list *conn_list_end = NULL;

static void add_conn( int socket)
{
        struct conn_list *entry = malloc(sizeof(struct conn_list));
        entry->sockfd=socket;
        entry->next = NULL;
        if (conn_list_begin==NULL) {
                conn_list_begin=entry;
                conn_list_end=entry;
        } else {
                conn_list_end->next=entry;
                conn_list_end=entry;
        }
}


static void delete_conn(int socket)
{
        struct conn_list *Searcher = conn_list_begin;
        struct conn_list *Prev = NULL;

        while (Searcher != NULL) {
                if ( Searcher->sockfd == socket ) {
                        if ( Prev == NULL ) {
                                /* first entry */
                                conn_list_begin = Searcher->next;
                                free(Searcher);
                                return;
                        }

                        Prev->next = Searcher->next;
                        if (Searcher == conn_list_end)
                                conn_list_end=Prev;
                        free(Searcher);
                        return;
                }
        Prev = Searcher;
        Searcher = Searcher->next;
        }
        return; 
}


static void recreate_fd_sets(  fd_set *active_read_fd_set,
                                        fd_set *active_write_fd_set)
{
        FD_ZERO(active_read_fd_set);
        FD_ZERO(active_write_fd_set);
        struct conn_list *Searcher = conn_list_begin;

        while (Searcher != NULL) {
                FD_SET(Searcher->sockfd, active_read_fd_set);
                FD_SET(Searcher->sockfd, active_write_fd_set);
                Searcher = Searcher->next;
        }
}



static int conn_list_max()
{
	struct conn_list *entry = conn_list_begin;
	int max=0;
	while (entry!=NULL) {
		if (max < entry->sockfd ) max=entry->sockfd;
		entry=entry->next;
	}
	return max;
}

static void add_filename( char *fname,int socket)
{
	struct file_element *entry = malloc(sizeof(struct file_element));
	entry->filename=fname;
	entry->socket = socket;
	entry->next = NULL;
	if (fnamelist_begin==NULL) {
		fnamelist_begin=entry;
		fnamelist_end=entry;
	} else {
		fnamelist_end->next=entry;
		fnamelist_end=entry;
	}
}
static void delete_all_filenames_of_sock( int sockfd )
{
        struct file_element *entry = fnamelist_begin;
        struct file_element *backup = fnamelist_begin;
        while (entry != NULL) {
                if ( entry->socket == sockfd ) {
                        free(entry->filename);
                        if (fnamelist_begin==entry) {
                                fnamelist_begin=entry->next;
				backup=entry->next;
			} else backup->next=entry->next;
                        free(entry);
			entry=fnamelist_begin;
			continue;
                }
                backup=entry;
                entry=entry->next;
        }
}

static void print_filename_list()
{
	struct file_element *entry= fnamelist_begin;
	while (entry != NULL) {
		entry=entry->next;
	}
}

// return 1 if the filename does already exist
static int check_if_filename_exists( char *fname)
{
	struct file_element *entry = fnamelist_begin;
	while (entry!= NULL) {
		if (strcmp(fname,entry->filename)==0)
			return 1;
		entry=entry->next;
	}
	return 0;
}

static char *get_random_filename() {
        int max_filenames = 1;
        int max_directories = 1;
        int nfilename = 0;
        int ndirectory = 0;
        int z = 0;
        char *strdat = NULL;
        char *fullstr = NULL;
        char *retstr = NULL;
        FILE *fnames = fopen("/usr/share/smbtatools/filenames.txt","r");
        FILE *dnames = fopen("/usr/share/smbtatools/directories.txt","r");
        if (fnames == NULL || dnames == NULL) {
		fnames = fopen("/usr/local/share/smbtatools/filenames.txt","r");
		dnames = fopen("/usr/local/share/smbtatools/directories.txt","r");
		if (fnames == NULL || dnames == NULL) {
                	printf("ERROR:  cannot open filename list and/or\n");
                	printf("        directory list. Exiting.\n");
			exit(1);
        	}
	}
        while ( !feof(fnames) ) {
                fscanf(fnames, "%ms\n", &strdat);
                max_filenames = max_filenames + 1;
                if (strdat != NULL) free(strdat);
        }
        while ( !feof(dnames) ) {
                fscanf(dnames, "%ms\n", &strdat);
                if (strdat != NULL) free(strdat);
                max_directories = max_directories + 1;
        }
        rewind(fnames);
        rewind(dnames);
        nfilename = rand() % max_filenames;
        ndirectory = rand() % max_directories;
	if (nfilename > 0) nfilename--;
	if (ndirectory > 0) ndirectory--;
	while ( z < ndirectory ) {
                z++;
                fscanf(dnames, "%ms\n", &strdat);
                if (strdat != NULL) free(strdat);
        }
        fscanf(dnames, "%ms\n",&fullstr);
        z = 0;
        while ( z < nfilename) {
                z++;
                fscanf(fnames, "%ms\n", &strdat);
                if (strdat != NULL) free(strdat);
        }
        fscanf(fnames,"%ms\n",&strdat);
        retstr = (char *) malloc( sizeof(char) * ( strlen(fullstr) + strlen(strdat) ) + 5 );
        strcpy(retstr, fullstr);
        strcat(retstr,"/");
        strcat(retstr, strdat);
        free(fullstr);
        free(strdat);
        fclose(fnames);
        fclose(dnames);
        return retstr;
}

static void send_data(char *str,int sock)
{
	int h=strlen(str);
	char *tosend=(char *) malloc(sizeof(char)*strlen(str)+10);
	sprintf(tosend,"%04i%s",h,str);
	fd_set write_fd_set;
	FD_ZERO(&write_fd_set);
	FD_SET(sock,&write_fd_set);
	select(sock+1,NULL,&write_fd_set,NULL,NULL);
	send(sock,tosend,strlen(tosend),0);
	free(tosend);
}

static void handle_data(int sock, struct configuration *config)
{
	char prefix[5];
	char *inp=NULL;
	char *fname=NULL;
	int r,t;
	t = recv( sock, prefix, 4, 0);
	if (t != 4 && t != 0) { 
		printf("ERROR: wrong format!\n");
		exit(1);
	}
	if (t == 0) { 
		delete_all_filenames_of_sock(sock);
		delete_conn(sock);
		return; 
	}
	int l = atoi(prefix);
	inp = (char *) malloc(sizeof(char) * (l+1));
	t = recv( sock, inp,l,0);
	// interpret commands
	r = 1;
	switch ( inp[0] )
	{
	case 'f':
		// request for a filename
		while( r==1 ) {
			fname=get_random_filename();
			r=check_if_filename_exists(fname);
		}
		add_filename(fname, sock);
		send_data(fname,sock);
		print_filename_list();
		break;
	default:
		printf("ERROR: wrong command.\n");
		exit(1);
	}
	free(inp);
}	

		

static void handle_network( struct configuration *config )
{
	int maxconn=0;
	socklen_t t = sizeof(struct sockaddr_in);
	int z,i;
        struct sockaddr_in remote_inet;

        fd_set read_fd_set;
        fd_set write_fd_set;

        FD_ZERO(&read_fd_set );
        FD_ZERO(&write_fd_set );
	add_conn(config->sockfd);
	for (;;) 
	{
		maxconn=conn_list_max()+1;
		recreate_fd_sets(&read_fd_set,&write_fd_set);
		z = select( maxconn,
                        &read_fd_set, NULL, NULL,NULL);
	                for( i = 0; i < maxconn; ++i) {
                        	if (FD_ISSET(i,&read_fd_set)) {
                                	int sr; 
                                	if ( i == config->sockfd) {
                                        	sr = accept( config->sockfd,
							(struct sockaddr *) &remote_inet,&t);
						if (sr == -1) {
							printf("ERROR: Could not accept connection!\n");
							exit(1);
						}
						add_conn(sr);
					} else handle_data(i,config);
                        	}
                	}
	}
}

/* 
* Create a listening internet socket on a port.
 * int port             The port-number.
 */
static int network_create_socket( int port )
{
        int sock_fd;
        struct sockaddr_in6 my_addr;

        if ( (sock_fd = socket(AF_INET6, SOCK_STREAM,0)) == -1 ) {
                syslog( LOG_DEBUG, "ERROR: socket creation failed." );
                exit(1);
        }

        int y;
        if ( setsockopt( sock_fd, SOL_SOCKET, SO_REUSEADDR, &y,
                sizeof( int )) == -1 ) {
                syslog( LOG_DEBUG, "ERROR: setsockopt failed." );
                exit(1);
        }

        bzero (&my_addr, sizeof (my_addr));
        my_addr.sin6_family = AF_INET6;
        my_addr.sin6_port = htons( port );
        my_addr.sin6_addr = in6addr_any;

        if (bind(sock_fd,
                (struct sockaddr *)&my_addr,
                sizeof(my_addr)) == -1 ) {
                syslog( LOG_DEBUG, "ERROR: bind failed." );
                exit(1);
        }

        if ( listen( sock_fd, 50 ) == -1 ) {
                syslog( LOG_DEBUG, "ERROR: listen failed." );
                exit(1);
        }

        return sock_fd;
}

int main(int argc, char *argv[])
{
	struct configuration config;
	int c;
        if (argc==1)
        {
                printf("ERROR. smbtatorturesrv -h to get help\n");
                return 0;
        }

	config.port = 3493;
	config.verbose = 1;

        while (1)
                {
                int option_index=0;
                static struct option long_options[] =
                        {
                         { "port",1,NULL,'p' },
                         { "verbose",0,NULL,'v' },
                         { "version",1,NULL,'V'},
			 { "help",0,NULL,'h'},
                         { 0,0,0,0 }
                        };

                c=getopt_long(argc,argv,
                        "p:vVh",
                        long_options,&option_index);
                if (c==-1) break;
                switch(c)
		{
		case 'p':
			config.port = atoi(optarg);
			break;
		case 'V':
			printf("smbtatorturesrv %s\n",SMBTATORTURESRV_VERSION);
			printf("(C) 2011 Holger Hetterich\n");
			exit(0);
			break;
		case 'v':
			config.verbose = 1;
			break;
		case 'h':
			printf("-p NUMBER	set the portnumber to listen on\n");
			printf("-v --verbose	be verbose.\n");
			printf("-V --version	print version.\n");
			printf("-h --help	this help text.\n");
			exit(0);
			break;
                default :
                        printf("ERROR: unkown option.\n\n");
                        printf("torturesrv -h to get help.\n");
                        return -1;

		}		

	}

	config.sockfd = network_create_socket( config.port );
	if (config.verbose==1)
		printf("Listening on port %i.",config.port);
	handle_network( &config );	
	return 0;
}

