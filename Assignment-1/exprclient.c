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
#define VALSIZE 20

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

    fprintf(stdout, "Connected to server: %s port: %u\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

    char* expr = NULL;
    size_t n = 0;
    fprintf(stdout,"Enter valid expression of +,-,*,/,(,) and numbers (spaces ignored) and hit return key\nEnter \'-1\' to exit\n");
    while(1){
        ssize_t exprlen = getline(&expr, &n, stdin), sent_bits;
        expr[exprlen-1]='\0';

        if(exprlen==0){
		    fprintf(stderr, "Empty expression, Try again\n");
            continue;
        }

        int index=0;

        while(1){
            sent_bits=send(sockfd, expr+index, (exprlen-index), 0);
            if(sent_bits < 0){
                fprintf(stderr, "send() failed: %s\n", strerror(errno));
                exit(1);
            }
            index+=sent_bits;
            if(index == exprlen) break;
        }
        fprintf(stdout, "%ld bytes sent to server\n", exprlen);

        if(exprlen==3 && expr[0]=='-' && expr[1]=='1'){
            break;
        }
        char val[VALSIZE];
        if(recv(sockfd, val, VALSIZE, 0) < 0)
        {
            fprintf(stderr, "recv() failed: %s\n", strerror(errno));
            exit(1);
        }

        printf("%s\n\n", val);
    }

	close(sockfd);
    
    fprintf(stdout, "Connection closed\n");

	return 0;
}
