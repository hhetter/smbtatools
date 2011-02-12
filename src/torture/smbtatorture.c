/*
 * smbtatorture
 * small testingsuite for smbta
 *
 * Copyright (C) Michael Haefner, 2009, 2010
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include "../../include/version.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <libsmbclient.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/times.h>
#include <errno.h>
#include <limits.h>

#include <netdb.h>
#include <stdlib.h>
#include <sys/time.h>
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
#include <string.h>
#include <stdio.h>
#include <talloc.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>



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




struct configuration_data {
        char *user;
        char *password;
        char *workgroup;
        char *share1;
        char *share2;
        char *record;
        char *replay;
        unsigned long int  size;
        int  copy;
        int  time;
        int  number;
        int verbose;

        FILE *recorder;
        FILE *player;

        int port;
        int sockfd;
        char *host;

} typedef conf;

conf config;




char **fnamelist;


// get random filenames, either from a
// smbtatorturesrv, or generate on it's own
char *get_random_filename() {
	int max_filenames = 0;
	int max_directories = 0;
	int nfilename = 0;
	int ndirectory = 0;
	int z = 0;
	char *strdat = NULL;
	char *fullstr = NULL;
	char *retstr = NULL;

	switch(config.port) {
	case 0: ;
		FILE *fnames = fopen("/usr/share/smbtatools/filenames.txt","r");
		FILE *dnames = fopen("/usr/share/smbtatools/directories.txt","r");
		if (fnames == NULL || dnames == NULL) {
			printf("ERROR: 	cannot open filenames list and/or\n");
			printf("	directory list. Exiting.\n");
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
	default:
		if (config.verbose==1)
			printf("Receiving a filename from smbtatorturesrv...\n");
		// get a filename from smbtatorturesrv
		fd_set wfd_set;
		FD_ZERO(&wfd_set);
		FD_SET(config.sockfd,&wfd_set);
		z = select(config.sockfd + 1,NULL,&wfd_set,NULL,NULL);
		if (FD_ISSET( config.sockfd,&wfd_set) ) {
			strdat=strdup("      ");
			strcpy(strdat,"0001f");
			send(config.sockfd,strdat,5,0);
		}
		printf("	Send request...\n");
		FD_ZERO(&wfd_set);
		FD_SET(config.sockfd,&wfd_set);
		z = select(config.sockfd+1,&wfd_set,NULL,NULL,NULL);
		if (FD_ISSET( config.sockfd,&wfd_set) ) {
			recv(config.sockfd,strdat,4,0);
			z = atoi(strdat);
		}
		printf("	Following answer will be %i bytes long...\n",
			z);

		FD_ZERO(&wfd_set);
		FD_SET(config.sockfd,&wfd_set);
		free(strdat);
		int zz=z;
		strdat = (char *) malloc( (sizeof(char)*z)+2);
		z = select(config.sockfd+1,&wfd_set,NULL,NULL,NULL);
		if (FD_ISSET( config.sockfd,&wfd_set)) {
			recv(config.sockfd,strdat,zz,0);
			strdat[zz]='\0';
			if (config.verbose==1)
				printf("Got this from smbtatorturesrv: > %s <\n",strdat);
			return strdat;
		}
		if (config.verbose==1)
			printf("WARNING: Unable to receive a filename from smbtatorturesrv!");		
		return NULL;
		}
}
	
	
	
		

unsigned long long int common_myatoi( char *num)
{
        char *endptr;
        errno = 0;
        unsigned long long val;
        val = strtoll(num, &endptr, 0); 
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                || (errno != 0 && val == 0)) {
                perror("strtol"); 
                exit(1);
        }
        if (endptr == num) {
               fprintf(stderr, "strtol: No digits were found\n");
               exit(1); 
           }    
        return (unsigned long long) val; 
}

static void
get_auth_data_fn(const char * pServer,
		const char * pShare,
		char * pWorkgroup,
		int maxLenWorkgroup,
		char * pUsername,
		int maxLenUsername,
		char * pPassword,
		int maxLenPassword) {


sprintf(pWorkgroup,"%s",config.workgroup);
sprintf(pUsername, "%s",config.user);
sprintf(pPassword,"%s",config.password);
}

struct timeval tv;

int mtime(void)
{
	gettimeofday(&tv, NULL);
	return (int)(tv.tv_sec*1000 + (tv.tv_usec / 1000));
}

void generate_files()
{
        /* on playback, load the number of files */
        if (config.replay!=NULL) {
                fscanf(config.player,"number of files: %i\n",&config.number);
	}
	int list[config.number];
	int i,z;
	int fd, fd2;
	int bytecount;
	int debug=0;
	char buffer[40000];
	srand(time(NULL) + getpid());
	for(i=0;i<config.number;i++)
	{
		list[i]=(rand() % (config.size-(config.number-i)));
		config.size=config.size-list[i];
	}
/*
	//check here if there are sizes under 2000 bytes
	for (i=0;i<config.number;i++) {
		if (list[i]<config.number) {
			int z;
			for (z=0;z<config.number;z++) {
				if (list[z]>2000) {
					//srand( time(NULL) );
					int a=rand() % (list[z]);
					list[z]=list[z]-a;
					list[i]=list[i]+a;
					break;
				}
			}
		}
	}
*/
	list[config.number-1]=config.size;

	/* when recording, store the list of files and sizes */
        if (config.record!=NULL) {
                /* when recording, store the time to wait here          */
                fprintf(config.recorder,"number of files: %i\n",config.number);
		for (z=0;z<config.number;z++) {
			fprintf(config.recorder,"file size: %i\n",list[z]);
		}
        }
	/* on playback, load the list of files and sizes */
        if (config.replay!=NULL) {
		for (z=0;z<config.number;z++) {
			fscanf(config.player,"file size: %i\n",&list[z]);
		}
        }


	if (config.verbose == 1)
	{
		printf("Stage 1: creating now the files on both shares\n");
	}

	for (i=0;i<config.number;i++) {
		char *t = NULL;
		if (config.replay == NULL) t = get_random_filename();
                if (config.replay != NULL) {
                        fscanf(config.player,"Filename: %ms\n",&t);
                        }

		char *Dateiname1 = (char *) malloc(sizeof(char) * (strlen(config.share1)+strlen(t)+5));
		char *Dateiname2 = (char *) malloc(sizeof(char) * (strlen(config.share2)+strlen(t)+5));
		/* when recording, write the list of filenames + paths */
		if (config.record!=NULL) {
			fprintf(config.recorder,"Filename: %s\n", t);
			}
                strcpy(Dateiname1,config.share1);
                strcpy(Dateiname2,config.share2);
                strcat(Dateiname1,t);
                strcat(Dateiname2,t);
		fnamelist[i]=t;
		// create directories
                smbc_init(get_auth_data_fn, debug);
		int z= 0;
		char *sname = (char *) malloc(sizeof(char) * (strlen(config.share1)+strlen(t)+5 + strlen(config.share2)));
		while (z<strlen(t)) {
			if (t[z]=='/') {
				t[z]='\0';
				strcpy(sname,config.share1);
				strcat(sname,t);
				printf("Creating Directory: %s\n",sname);
				smbc_mkdir(sname,0777);
				strcpy(sname,config.share2);
				strcat(sname,t);
				printf("Creating Directory: %s\n",sname);
				smbc_mkdir(sname,0777);
				t[z]='/';
				
			}
			z++;
		}
		free(sname);
	
		if ((fd = smbc_open(Dateiname1,
			O_WRONLY | O_CREAT | O_TRUNC, 0)) < 0)
			{
			perror("smbc_open: writing failed");
			}

		bytecount=list[i];
		while (bytecount>40000) {
			smbc_write( fd,buffer,40000);
			bytecount=bytecount-40000;
		}

		if (bytecount<40000) {
			smbc_write(fd,buffer,bytecount);
			bytecount=0;
		}

		smbc_close(fd);

		if ((fd2 = smbc_open(Dateiname2, 
			O_WRONLY | O_CREAT | O_TRUNC, 0) < 0))

			{
			perror("smbc_open: failed");
			exit(1);
			}

		bytecount=list[i];
		while (bytecount>40000) {
			smbc_write(fd, buffer, 40000);
			bytecount=bytecount-40000;
		}

		if (bytecount<40000) {
			smbc_write(fd, buffer, bytecount),
			bytecount=0;
		}

		if(config.verbose==1)
		{
			printf("File %s written with %u Bytes\n", Dateiname1, list[i]);
		}

		if(i==config.number-1)
		{
			printf("All files successfully written."
				" Now it's torture time!\n");
		}

		smbc_close(fd2);
		free(Dateiname1);
		free(Dateiname2);
	}

}

void copy()
{
        int fd, fd2;
        int ret;
	int i;
        int debug=0;
        int savedErrno;
	int rTime,justread;
        char buffer[40048];
	i=( random() % config.number);
	char *Dateiname1 = (char *) malloc(sizeof(char) * (strlen(config.share1)+strlen(config.share2)+strlen(fnamelist[i])));
	char *Dateiname2 = (char *) malloc(sizeof(char) * (strlen(config.share1)+strlen(config.share2)+strlen(fnamelist[i])));
	rTime= (random() % 2);
	justread= (random() %2); // wether to write to the target share or not
	switch (rTime)
	{
		case 0:
		strcpy(Dateiname1, config.share1);
		strcpy(Dateiname2, config.share2);
		break;
		case 1:
		strcpy(Dateiname1, config.share2);
		strcpy(Dateiname2, config.share1);
		break;
	}
        /* at this point we have the full filenames. In case of 
         * recording, we save them here. In case of replay, we  
         * replace them with the strings from the file.
         */
        if (config.record!=NULL) {
                fprintf(config.recorder,"file1: %i\n",i);
                fprintf(config.recorder,"file2: %i\n",i);
                fprintf(config.recorder,"just read: %i\n",justread);
        }
        if (config.replay!=NULL) {
                fscanf(config.player,"file1: %i\n",&i);
                fscanf(config.player,"file2: %i\n",&i);
                fscanf(config.player,"just read: %i\n",&justread);
        }

	strcat(Dateiname1, fnamelist[i]);
	strcat(Dateiname2, fnamelist[i]);

	smbc_init(get_auth_data_fn, debug);

	if ((fd = smbc_open(Dateiname1, O_RDONLY, 0)) < 0)
	{	
		perror("smbc_open failed");
		exit(1);
	}

	
	if (justread!=1 && (fd2 = smbc_open(Dateiname2, 
		O_WRONLY | O_CREAT | O_TRUNC, 0)) < 0)
	{
		perror("smbc_open: writing failed");
		exit(1);
	}

	do
	{
		ret = smbc_read(fd, buffer, sizeof(buffer));
	
		savedErrno = errno;
		if (ret > 0 && justread!=1) smbc_write(fd2, buffer, ret);
	} while (ret > 0);

	smbc_close(fd);
	if (justread!=1) smbc_close(fd2);

	if (config.time==0) rTime=0; 
	else
		rTime = (random() % config.time)+1;

	if (config.record!=NULL) {
		/* when recording, store the time to wait here 		*/
		fprintf(config.recorder,"wait: %i\n",rTime);
	}
	if (config.replay!=NULL) {
		/* when in playback, load the time to wait here 	*/
		fscanf(config.player,"wait: %i\n",&rTime);
	}
        if(config.verbose == 1)
        {	if (justread!=1) printf("File %s copied to %s\n", Dateiname1, Dateiname2);
			else printf("Read file %s into memory.\n",Dateiname1);
		
		printf("Sleeping %i seconds...\n", rTime);
        }

	sleep(rTime);
	free(Dateiname1);
	free(Dateiname2);
	if (config.verbose ==1) printf("Transferring data...\n");
}

void help()
{

	printf("smbtatorture version %s - small testingsuite for smbta	\n", SMBTATORTURE_VERSION);
	printf("\n");
	printf("(C) 2009-2010 Michael Haefner\n");
	printf("(C) 2009-2010 Holger Hetterich\n");
	printf("%s\n",SMBTA_LICENSE);
	printf("-u --username 		The user smbtatorture should	\n");
	printf("			operate on.			\n");
	printf("			Default: john_doe		\n");
	printf("-w --workgroup		The workgroup smbtatorture should\n");
	printf("			operate on.			\n");
	printf("			Default: WORKGROUP		\n");
	printf("-p --password		Password for the user.		\n");
	printf("			Default: password		\n");
	printf("-h --help		This help text.			\n");
	printf("-1 --share1             SMB URL of first share to be 	\n");
	printf("			used.				\n");
	printf("-2 --share2		SMB URL of second share to be 	\n");
	printf("			used.				\n");
	printf("-c --copy		How many files will be copied   \n");
	printf("			Default: infinite		\n");
	printf("-r --record		filename of saved torture-set   \n");
	printf("			won't work actually		\n");
	printf("-t --time		max time in seconds we should   \n");
	printf("			wait between each copy action.  \n");
	printf("-s --size		size of all files, e.g. 500M or \n");
	printf("			1.5G				\n");
	printf("-n --number		Number of generated files	\n");
	printf("			Default: 20			\n");
	printf("-v --verbose		be verbose			\n");
	printf("-e --replay		filename of the file to replay.	\n");
	printf("\n");
	printf("When running networked with smbtatorturesrv:		\n");
	printf("-P --port		Port to connect to.		\n");
	printf("-H --host		hostname to connect to.		\n");
}

void define_config_defaults()
{
        config.user=malloc( sizeof(char)*( strlen("john_doe") +1 ));
        strcpy(config.user,"john_doe");
        config.password=malloc( sizeof(char) * (strlen("password")
                +1));
        strcpy(config.password,"password");
	config.workgroup=malloc(sizeof(char)* (strlen("WORKGROUP")+1));
	strcpy(config.workgroup,"WORKGROUP");
	config.share1=malloc(sizeof(char)*
		(strlen("smb://somehost/someshare1/")+1));
	strcpy(config.share1,"smb://somehost/someshare1/");
	config.share2=malloc(sizeof(char)*
		(strlen("smb://somehost/someshare2/")+1));
	strcpy(config.share2,"smb://somehost/someshare2/");
	config.record=NULL;
	config.size=900000;
	config.copy=0;
	config.time=15;
	config.number=20;
	config.verbose=0;
	config.replay=NULL;
	config.port = 0;
	config.sockfd = 0;
	config.host = NULL;

}

int connect_smbtatorturesrv(int iport,char *hostname)
{
        if ( hostname == NULL ) {
                printf("ERROR: no hostname given.\n");
                exit(1);
        }

        struct addrinfo *ai;   
        struct addrinfo hints;
        memset(&hints,'\0',sizeof(hints));
     
        hints.ai_flags=AI_ADDRCONFIG;
        hints.ai_socktype=SOCK_STREAM;
        char port[255]; 
        sprintf(port,"%i",iport);
        int e=getaddrinfo(hostname,port,&hints,&ai);
        if (e!=0) {
                printf("ERROR: error getaddrinfo\n");
                exit(1);
        } 
        int sockfd=socket(ai->ai_family,ai->ai_socktype,ai->ai_protocol);
        if (sockfd==-1) {
                printf("ERROR: error in socket operation!\n");
                exit(1);
        };
        int result=connect(sockfd,ai->ai_addr,ai->ai_addrlen);
        if (result==-1) {
                printf("ERROR: error in connect operation!\n");
                exit(1);
        };

        return sockfd;

}


int main(int argc, char *argv[])
{
	define_config_defaults();
	int c, i;
	int set_rand_seed=0;
	if (argc==1)
	{
		help();
		return 0;
	}

	while (1)
                {
                int option_index=0;
                static struct option long_options[] =
                        {
                         { "user",1,NULL,'u' },
			 { "port",1,NULL,'P' },
			 { "host",1,NULL,'H' },
                         { "workgroup",1,NULL,'w' },
                         { "password",1,NULL,'p'},
                         { "help",0,NULL,'h' },
			 { "share1",1,NULL,'1'},
			 { "share2",1,NULL,'2'},
			 { "copy",1,NULL,'c'},
			 { "record",1,NULL,'r'},
			 { "size",1,NULL,'s'},
			 { "time",1,NULL,'t'},
			 { "number",1,NULL,'n'},
			 { "verbose",0,NULL,'v'},
			 { "replay",1,NULL,'e'},
                         { 0,0,0,0 }
                        };

                c=getopt_long(argc,argv,
                        "e:u:w:p:h1:2:c:r:s:t:n:vH:P:",
                        long_options,&option_index);
                if (c==-1) break;
                switch(c)
                	{
                	case 'u':
				if (config.user!=NULL) free(config.user);
                        	config.user=(char *) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.user,optarg);
                        	break;
			case 'w':
				if (config.workgroup!=NULL) 
					free(config.workgroup);
				config.workgroup=(char *) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.workgroup,optarg);
				break;
			case 'p':
				if (config.password!=NULL) 
					free(config.password);
				config.password=(char *) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.password,optarg);
				break;
			case 'P':
				config.port=atoi(optarg);
				break;
			case 'H':
				config.host=(char *) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.host,optarg);
				break;
			case 'h':
				help();
				return 0;
			case '1':
				if (config.share1!=NULL) free(config.share1);
				config.share1=(char*) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.share1,optarg);
				break;
			case '2':
				if (config.share2!=NULL) free(config.share2);
				config.share2=(char*) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.share2,optarg);
				break;
			case 'c':
				config.copy = (int) common_myatoi(optarg);
				break;
			case 'r':
				if (config.record!=NULL) free(config.record);
				config.record=(char*) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.record,optarg);
				break;
			case 's':
				if(strstr(optarg, "M")!=NULL) 
				{
					config.size=(int) common_myatoi(optarg)*1024*1024;
				}
				else if(strstr(optarg, "G")!=NULL)
				{
					config.size=(int) common_myatoi(optarg)*1024*1024*1024;
				}
				else if(strstr(optarg, "K")!=NULL)
				{
					config.size=(int) common_myatoi(optarg)*1024;
				}
				else
				 	config.size=(int) common_myatoi(optarg);

				break;
			case 't':
				config.time = (int) common_myatoi(optarg);
				break;
			case 'n':
				config.number = (int) common_myatoi(optarg);
				break;
			case 'v':
				config.verbose = 1;
				break;
			case 'e':
				if (config.replay!=NULL) free(config.replay);
				config.replay=(char *) malloc(sizeof(char)*
					strlen(optarg));
				strcpy(config.replay,optarg);
				break;
			default :
				printf("Error: unkown option.\n\n");
				help();
				return 0;
		}
		}


	/* check the mode */
	if (config.replay!=NULL && config.record!=NULL) {
		printf("ERROR: please either record or playback!\n");
		return 0;
	}

	if (config.replay!=NULL && config.port!=0) {
		printf("FYI: 	Since we replay a torture run, we will not\n");
		printf("	connect to a smbtatorturesrv process.\n");
		config.port = 0;
	}

	if (config.record!=NULL && config.copy==0) {
		printf("ERROR: recording can't be infinite.\n");
		return 0;
	}

	/* open the file according to the mode */
	if (config.replay!=NULL) {
		config.player=fopen(config.replay,"r");
		if (config.player==NULL) {
			printf("ERROR: playback file cannot be opened.\n");
			return 0;
		}
	}
	if (config.record!=NULL) {
		config.recorder=fopen(config.record,"w");
		if (config.recorder==NULL) {
			printf("ERROR: recorder file cannot be"
				" opened for writing.\n");
		return 0;
		}
	}
	if (config.port != 0) {
		// connect to smbtatorturesrv
		if (config.host==NULL) {
			printf("ERROR: please specify a host to connect to.\n");
			return 0;
		}
		config.sockfd=
			connect_smbtatorturesrv(config.port, config.host);
	}
        /* init fnamelist */
        fnamelist = (char **) malloc(sizeof(char *) * (config.number + 1));
	generate_files();
	if (config.replay!=NULL) {
		/* when playing back, get the number of copies first */
		fscanf(config.player,"copy = %i\n",&config.copy);
		/* when playing back, get the number of files that have been
		   created. */
		fscanf(config.player,"number = %i\n",&config.number);
	}

	/* initialize rand */
	time_t rand_time;
	time(&rand_time);
	srandom( (unsigned int) rand_time);

	/* endless copying until we are stopped */
	if(config.copy==0)
	{
		for(;;)
		{
			copy();
			set_rand_seed++;
			//srand(time(NULL)+set_rand_seed);
	                if (set_rand_seed==100) {
        	                //srand(time(NULL));
                	        set_rand_seed=0;
                	}

		}
	}

	if (config.record!=NULL) {
		// when recording, save the number of copies as the first
		// parameter
		fprintf(config.recorder,"copy = %i\n",config.copy);

		// then the number of files that have been created
		fprintf(config.recorder,"number = %i\n",config.number);
	}
	static clock_t start_time;
	static clock_t end_time;
	static struct tms start_cpu;
	static struct tms end_cpu;
	unsigned long int clockticks = sysconf(_SC_CLK_TCK);
	start_time=times(&start_cpu);
	for(i=0;i<config.copy;i++)
	{
		copy();
		set_rand_seed++;
		//srand(time(NULL)+set_rand_seed);
		if (set_rand_seed==100) {
			//srand(time(NULL));
			set_rand_seed=0;
		}
	}
	end_time=times(&end_cpu);
	double timet = (double) (end_time - start_time) / (double) clockticks;	
	printf("Time required : %f seconds.\n", timet);
	// free allocated memory
	free(config.user);
	free(config.workgroup);
	free(config.password);
	free(config.share1);
	free(config.share2);
	if (config.host != NULL) free(config.host);

	if (config.record!=NULL) {
		free(config.record);
		fclose(config.recorder);
	}
	if (config.replay!=NULL) {
		free(config.replay);
		fclose(config.player);
	}
	if (config.port!=0) close(config.sockfd);		

	return 0;

}
