/*labrea.h*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h> //Req'd for FD_SET, FD_ISSET, FD_ZERO
#include "litloop.h"

#define PORT 8888 // Listening port to sit the daemon on

int make_socket(uint16_t port);
int read_from_client(int filedes, char c);
int raisebanner();
void *literature(void * parm);
int main();

