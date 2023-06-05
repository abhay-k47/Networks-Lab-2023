#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 50

int main()
{
	int sockfd;
	struct sockaddr_in	serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		exit(1);
	}

    fprintf(stdout, "Socket created\n");

	serv_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port = htons(20000);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "connect() failed: %s\n", strerror(errno));
		exit(1);
	}

    fprintf(stdout, "Connected to server\n");

	char buf[BUFSIZE];
	for(int i=0; i < BUFSIZE; i++) buf[i] = '\0';
    
    ssize_t size;
    if((size = recv(sockfd, buf, 100, 0)) < 0)
    {
        fprintf(stderr, "recv() failed: %s\n", strerror(errno));
        exit(1);
    }

	fprintf(stdout, "%ld bytes recieved\n", size);
    
    printf("%s", buf);

	close(sockfd);

    fprintf(stdout, "Connection closed\n");

	return 0;
}

