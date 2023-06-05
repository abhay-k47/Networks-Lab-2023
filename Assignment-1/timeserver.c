#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	int	sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in	cli_addr, serv_addr;
    char buf[BUFSIZE];


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
		exit(1);
	}

    fprintf(stdout, "Socket created\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(20000);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "bind() failed: %s\n", strerror(errno));
		exit(1);
	}

    fprintf(stdout, "Local address binded\n");

	listen(sockfd, 5);

    fprintf(stdout, "Server listening.....\n\n");

	while (1)
    {		
		clilen = sizeof(cli_addr);

		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
        {
            fprintf(stderr, "accept() failed: %s\n", strerror(errno));
		    exit(1);
        }

        fprintf(stdout, "Client connection accepted\n");

        time_t rawtime;
        struct tm* timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strcpy(buf, asctime(timeinfo));

	    if(send(newsockfd, buf, strlen(buf)+1, 0) < 0){
            fprintf(stderr, "send() failed: %s\n", strerror(errno));
            exit(1);
        }

        fprintf(stdout, "Localtime sent to client\n");
		
        close(newsockfd);

        fprintf(stdout, "Connection closed\n\n");
	}

	return 0;
}
