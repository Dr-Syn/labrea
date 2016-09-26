/*litloop.c
This routine loads a text file from the current directory and loops over it at a given rate.
Each character is made available one at a time on a named pipe.

 */

#include "litloop.h"


int makePipe(){
	int fd;
	char * litPipe = "/tmp/litpipe";
	FILE *readfile;
	int c;

	mkfifo(litpipe, 0666);
	
	if (!(readfile = fopen(MOBY, "rt"))){
		perror("Unable to open file");
		exit(1);
	}
	while (1) {
		while ((c = fgetc(readfile)) != EOF){
			write(fd, c, sizeof(c));
			usleep(1000000);
			}
		rewind(readfile);
	}
	
}
