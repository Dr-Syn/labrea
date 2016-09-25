/* La Brea Daemon */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //Req'd for FD_SET, FD_ISSET, FD_ZERO

#define PORT 8888 // Listening port to sit the daemon on


int raisebanner();


int raisebanner(){
	//Variable declarations
	int opt = 1;
	int master_socket, addrlen, new_socket;
	int client_socket[32], max_clients=32;
	int activity, i, valread, sd;
	int max_sd;
	

	struct sockaddr_in address;

	char buffer[32]; //32 byte buffer - we aren't really using it
	
	fd_set readfds; // Req'd for socket descriptors

	char *message = "This is the song that doesn't end"; // banner - to start

	
	//initialize client_socket[] to 0

	for(i = 0; i < max_clients; i++){
		client_socket[i] = 0;
		}

	//Create a master socket
	if((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("Socket creation failed!");
		exit(1);
		}
	//Set master socket to allow multiple connections
	if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0){
		perror("Setsockopt: REUSEADDR failed");
		exit(1);
		}
	//Set master socket to try to keepalive because we want to consume resources
	if(setsockopt(master_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&opt, sizeof(opt)) < 0){
		perror("Setsockopt: KEEPALIVE failed");
		exit(1);
		}
	//Specify address details
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT); // This is currently set up above

	//Bind the socket to the port
	if(bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("Binding failed");
		exit(1);
	}
	printf("Listener bound to port %d \n", PORT);

	// Specify max connections for master socket: 3
	if(listen(master_socket, 3) < 0){
		perror("Listener creation failure");
		exit(1);
	}
	//Accept inbound connections
	addrlen = sizeof(address);
	printf("Connections live. Bring on the pain.\n");
	while(1){
		//Clear the socket set
		FD_ZERO(&readfds);
		//add the master socket to the set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		//add child sockets
		for(i = 0; i < max_clients; i++){
			//Socket descriptor...
			sd = client_socket[i];
			if(sd > 0){
				FD_SET(sd, &readfds);
				}
			if(sd > max_sd){
				max_sd = sd;
				}
			}
		//Wait for activity on the socket with no timeout
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if((activity < 0) && (errno!=EINTR)){
			perror("Select failure");
			exit(1);
		}
		//And when summat happens.....
		if(FD_ISSET(master_socket, &readfds)){
			if((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
			perror("Accept failure noted");
			exit(1);
			}
		for(i=0; i < max_clients; i++){ //Find an empty place to put this new fellow
			if(client_socket[i] == 0){
				client_socket[i] = new_socket;
				printf("Adding new victim as %d \n", i);
				break;
				}
			}
		}
		//Unless it's something un-useful - like closing
		for(i=0; i < max_clients; i++){
			sd = client_socket[i];
			if(FD_ISSET(sd, &readfds)){
				//Is it closing?
				if((valread = read(sd, buffer, 32)) == 0){
					getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
					printf("IP %s gave up!", inet_ntoa(address.sin_addr));
					//close socket; mark for reuse
					close(sd);
					client_socket[i] = 0;
				}
			else { // Send the current message to all open sockets
				buffer[valread] = '\0'; //Purge the buffer
				sendto(sd, message, strlen(message), MSG_MORE, NULL, 0);
				}
			}
		}
	}
	return 0;
}


int main(){
	
	pid_t process_id = 0; // Needed for daemonization
	pid_t sid = 0;
	int resultid = 0;
	//Dameonize via the standard fork method
	process_id = fork();
	if(process_id < 0){
		perror("Forking failed!");
		exit(1);
		}
	//Daemonized.
	resultid = raisebanner();
	return 0;
}
	
