/*litloop.c
This routine loads a text file from the current directory and loops over it at a given rate.
Each character is made available one at a time on a named pipe.

 */

#include "litloop.h"


int makePipe(){
	int fd;
	char * litPipe = "/tmp/litpipe";
	char buffer[32];
	FILE *readfile;
	int c;

	mkfifo(litPipe, 0666);
	fd = open(litPipe, O_WRONLY);
	printf("Commencing read\n");
	if (!(readfile = fopen(MOBY, "rt"))){
		perror("Unable to open file");
		exit(1);
	}
	while (1) {
		while ((c = fgetc(readfile)) != EOF){
			write(fd, readfile, sizeof(readfile));
			usleep(10000);
			}
		rewind(readfile);
	}
	
}
