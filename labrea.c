/* La Brea Daemon */


#include "labrea.h"

int make_socket(uint16_t port){
	int sock;
	struct sockaddr_in name;
	//Create the socket
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		perror("Socket creation failed");
		exit(1);
		}
	//Name the socket
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock,(struct sockaddr *) &name, sizeof(name)) < 0){
		perror("Binding error");
		exit(1);
		}
	return sock;
}

int read_from_client(int filedes, char c){
	char buffer[512];
	int nbytes;
	nbytes = read(filedes, buffer, 512);
	if (nbytes < 0){
		perror("Read error");
//		send(filedes, &c, sizeof(char), 0);
		printf("Read error\n");
		return -1;
		}
	else if (nbytes == 0){
		return(0);
		}
	else {
		//Reading data
//		send(filedes, &c, sizeof(char), 0);
		return 0;
	}
	send(filedes, &c, sizeof(char), 0);
}

int raisebanner(){
	//Variable declarations
	
	int fd;
	char * readFifo = "/tmp/litpipe";
	char buf[32];

	extern int make_socket(uint16_t port);
	int sock;
	fd_set active_fd_set, read_fd_set;
	int i;
	struct sockaddr_in clientname;
	size_t size;
	fd_set readfds; // Req'd for socket descriptors
	int new;

	FILE *readfile;
	char character;

	//Create socket & let it accept connections
	sock = make_socket(PORT);
	if(listen(sock,1) < 0){
		perror("Listen error");
		exit(1);
		}
	//Initialize the set of active sockets
	FD_ZERO (&active_fd_set);
	FD_SET(sock, &active_fd_set);

	// Initialize read from FIFO
//	fd = open(readFifo, O_RDONLY);
	readfile = fopen(readFifo, "r");
	printf("Connections live. Bring on the pain.\n");
	while(1){
		//Grab the character from the pipe
		character = fgetc(readfile);
		//Block until input arrives
		read_fd_set = active_fd_set;
		if(select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
			perror("Select error");	
			exit(1);
			}
		//Service sockets with inputs
		for(i = 0; i < FD_SETSIZE; ++i){
			if (FD_ISSET (i, &read_fd_set)){
				if (i == sock){
					printf("Orig sock detected\n");
					//Connection on orig. socket
					new = 0;
					size = sizeof(clientname);
					new = accept(sock, NULL, NULL);
					if (new < 0){
						perror("Accept error");
						printf("Accept error\n");
						exit(1);
						}
					send(new, &character, sizeof(char), 0);
					FD_SET (new, &active_fd_set);
					}
				else {

				//Data on an established socket
				if (read_from_client(i, character) < 0) {
					printf("closing connection\n");
					close(i);
					FD_CLR(i, &active_fd_set);
					}
				}
			}
		}
	}
	printf("Stopping\n");
	return 0;
}

void *literature(void * parm){
	int errval;
	printf("literature entered\n");
	errval = makePipe();
	}


int main(){
	
	pid_t process_id = 0; // Needed for daemonization
	pid_t sid = 0;
	int resultid = 0;

	pthread_t litloop[2];
	int thread; //thread ID
	int err;
	printf("Program start\n");
	//Dameonize via the standard fork method

	process_id = fork();
	if(process_id < 0){
		perror("Forking failed!");
		exit(1);
		}
	if (process_id > 0){
		printf("Process ID of child process %d\n", process_id);
		exit(0);
		}
	//Daemonized.
	
	err = pthread_create(&(litloop[0]), NULL, &literature, NULL);
	printf("Err val %d", err);
	if (err != 0){
		perror("Failed to create thread");
		exit(1);
		}
	resultid = raisebanner();
	return 0;
}
	
