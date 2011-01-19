#ifdef SOLARIS
    #define _XOPEN_SOURCE 500
#else
    #define _XOPEN_SOURCE
#endif

#define __EXTENSIONS__

#define _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500
#define _GNU_SOURCE

#ifdef SOLARIS
        #include <netinet/in.h>
        #include <sys/ddi.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/select.h>
#include <dlfcn.h>



#define SMBTATORTURESRV_VERSION "1.2.2"

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

void add_conn( int socket)
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

void delete_conn( int socket )
{
        struct conn_list *entry = conn_list_begin;
        struct conn_list *backup = conn_list_begin;
        while (entry != NULL) {
                if ( socket == entry->sockfd) {
                        if (conn_list_begin==entry) {
                                conn_list_begin=entry->next;
                                if (conn_list_end==conn_list_begin)
                                        conn_list_end=entry->next;
                        } else  backup->next=entry->next;
                        free(entry);
			return;
                        }
                backup=entry;
                entry=entry->next;
        }
}

void recreate_fd_sets(  fd_set *active_read_fd_set,
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



int conn_list_max()
{
	struct conn_list *entry = conn_list_begin;
	int max=0;
	while (entry!=NULL) {
		if (max < entry->sockfd ) max=entry->sockfd;
		entry=entry->next;
	}
	return max;
}

void add_filename( char *fname,int socket)
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

void delete_filename( char *fname )
{
	struct file_element *entry = fnamelist_begin;
	struct file_element *backup = fnamelist_begin;
	while (entry != NULL) {
		if ( strcmp(fname,entry->filename) == 0) {
			free(entry->filename);
			if (fnamelist_begin==entry) {
				fnamelist_begin=entry->next;
				if (fnamelist_end==fnamelist_begin)
					fnamelist_end=entry->next;
			} else 	backup->next=entry->next;
			free(entry);
			return;
			}
		backup=entry;
		entry=entry->next;
	}
}

// return 1 if the filename does already exist
int check_if_filename_exists( char *fname)
{
	struct file_element *entry = fnamelist_begin;
	while (entry!= NULL) {
		if (strcmp(fname,entry->filename)==0)
			return 1;
		entry=entry->next;
	}
	return 0;
}

char *get_random_filename() {
        int max_filenames = 0;
        int max_directories = 0;
        int nfilename = 0;
        int ndirectory = 0;
        int z = 0;
        char *strdat = NULL;
        char *fullstr = NULL;
        char *retstr = NULL;
        FILE *fnames = fopen("/usr/share/smbtatools/filenames.txt","r");
        FILE *dnames = fopen("/usr/share/smbtatools/directories.txt","r");
        if (fnames == NULL || dnames == NULL) {
                printf("ERROR:  cannot open filename list and/or\n");
                printf("        directory list. Exiting.\n");
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

void send_data(char *str,int sock)
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

void handle_data(int sock, struct configuration *config)
{
	char prefix[5];
	char *inp=NULL;
	char *fname=NULL;
	int r,t;
	t = recv( sock, prefix, 4, 0);
	if (t != 4) { 
		printf("ERROR: wrong format!\n");
		exit(1);
	}
	if (t == 0) { // FIXME Client exited to something 
	}
	int l = atoi(prefix);
	inp = (char *) malloc(sizeof(char) * (l+1));
	t = recv( sock, inp,l,0);
	// interpret commands
	printf("Got command: %c\n",inp[0]);
	r = 1;
	switch ( inp[0] )
	{
	case 'f':
		// request for a filename
		while( r==1 ) {
			fname=get_random_filename();
			r=check_if_filename_exists(fname);
		}
		printf("Sending : %s\n",fname);		
		send_data(fname,sock);
		break;
	default:
		printf("ERROR: wrong command.\n");
		exit(1);
	}
	free(inp);
}	

		

void handle_network( struct configuration *config )
{
	int maxconn=0;
	socklen_t t;
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
							&remote_inet,&t);
						printf("Accepted Client.\n");
						add_conn(sr);
						printf("Connection added.\n");
					} else handle_data(i,config);
                        	}
                	}
	}
}

/* 
* Create a listening internet socket on a port.
 * int port             The port-number.
 */
int network_create_socket( int port )
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

