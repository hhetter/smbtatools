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

} typedef conf;

conf config;



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
	int list[config.number];
	int i;
	int fd, fd2;
	int bytecount;
	int debug=0;
	int time1=0, time2=0;
	char buffer[40000];
	for(i=0;i<config.number;i++)
	{
		list[i]=(random() % (config.size-(config.number-i)));
		config.size=config.size-list[i];
	}

	//check here if there are sizes under 2000 bytes
	for (i=0;i<config.number;i++) {
		if (list[i]<config.number) {
			int z;
			for (z=0;z<config.number;z++) {
				if (list[z]>2000) {
					//srand( time(NULL) );
					int a=random() % (list[z]);
					list[z]=list[z]-a;
					list[i]=list[i]+a;
					break;
				}
			}
		}
	}

	list[config.number-1]=config.size;

	if (config.verbose == 1)
	{
		printf("Stage 1: creating now the files on both shares\n");
	}

	//creating now the files on share1
	for (i=0;i<config.number;i++) {
		char Dateiname1[strlen(config.share1)+50];
		char Dateiname2[strlen(config.share2)+50];
		strcpy(Dateiname1,config.share1);
		strcpy(Dateiname2,config.share2);
		char Zahl[50];
		sprintf(Zahl,"%i",i);
		strcat(Dateiname1, config.user);
		strcat(Dateiname2, config.user);
		strcat(Dateiname1, Zahl);
		strcat(Dateiname2, Zahl);
		time1=mtime();
		smbc_init(get_auth_data_fn, debug);
	
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
			perror("smbc_open: writing failed");
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

		time2=mtime();

		if(config.verbose==1)
		{
			printf("File %s written with %i Bytes\n", Dateiname1, list[i]);
			printf("in about %i milliseconds\n", (time2-time1));
		}

		if(i==config.number-1)
		{
			printf("All files successfully written."
				" Now it's torture time!\n");
		}

		smbc_close(fd2);
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
	char Zahl[50];
	char Dateiname1[strlen(config.share1)+50];
	char Dateiname2[strlen(config.share2)+50];
	i=( random() % config.number);
	sprintf(Zahl,"%i",i);
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

	strcat(Dateiname1, config.user);
	strcat(Dateiname2, config.user);
	strcat(Dateiname1, Zahl);
	strcat(Dateiname2, Zahl);
	/* at this point we have the full filenames. In case of 
	 * recording, we save them here. In case of replay, we	
	 * replace them with the strings from the file.
	 */
	if (config.record!=NULL) {
		fprintf(config.recorder,"file1: %s\n",Dateiname1);
		fprintf(config.recorder,"file2: %s\n",Dateiname2);
		fprintf(config.recorder,"just read: %i\n",justread);
	}
	if (config.replay!=NULL) {
		fscanf(config.player,"file1: %s\n",Dateiname1);
		fscanf(config.player,"file2: %s\n",Dateiname2);
		fscanf(config.player,"just read: %i\n",&justread);
	}

	smbc_init(get_auth_data_fn, debug);

	if ((fd = smbc_open(Dateiname1, O_RDONLY, 0)) < 0)
	{	
		perror("smbc_open failed");
	}

	
	if (justread!=1 && (fd2 = smbc_open(Dateiname2, 
		O_WRONLY | O_CREAT | O_TRUNC, 0)) < 0)
	{
		perror("smbc_open: writing failed");
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
		
		printf("sleeping now for %i seconds\n", rTime);
        }

	sleep(rTime);

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
                        "e:u:w:p:h1:2:c:r:s:t:n:v",
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
				config.copy = atoi(optarg);
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
					config.size=atoi(optarg)*1024*1024;
				}
				else if(strstr(optarg, "G")!=NULL)
				{
					config.size=atoi(optarg)*1024*1024*1024;
				}
				else if(strstr(optarg, "K")!=NULL)
				{
					config.size=atoi(optarg)*1024;
				}
				else
				 	config.size=atoi(optarg);

				break;
			case 't':
				config.time = atoi(optarg);
				break;
			case 'n':
				config.number = atoi(optarg);
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

	/* don't generate the files if we are in playback mode */
	if (config.replay==NULL) generate_files();

	/* check the mode */
	if (config.replay!=NULL && config.record!=NULL) {
		printf("ERROR: please either record or playback!\n");
		return 0;
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
	long double timet = (long double) (end_time - start_time) / clockticks;	
	printf("Time required for the copying: %lu seconds\n",
		(unsigned long int) timet);
	// free allocated memory
	free(config.user);
	free(config.workgroup);
	free(config.password);
	free(config.share1);
	free(config.share2);
	if (config.record!=NULL) {
		free(config.record);
		fclose(config.recorder);
	}
	if (config.replay!=NULL) {
		free(config.replay);
		fclose(config.player);
	}
		

	return 0;

}
