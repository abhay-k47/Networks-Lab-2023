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

#define BUFLEN 50

// recv until null-character is recieved, returns NULL if fails
char *recvString(int sockfd)
{
    int i, j, recvlen, capacity = 1;
    char buffer[BUFLEN];
    char *packet = (char *)malloc(capacity * sizeof(char));

    i = 0;
    memset(buffer, 0, BUFLEN);
    while (1)
    {
        recvlen = recv(sockfd, buffer, BUFLEN, 0);
        if (recvlen < 0)
        {
            perror("recv() failed");
            free(packet);
            return NULL;
        }
        if (recvlen == 0)
        {
            fprintf(stderr, "session terminated (Ctrl+C)\n");
            free(packet);
            return NULL;
        }
        if (capacity < i + recvlen)
        {
            capacity += recvlen;
            packet = (char *)realloc(packet, capacity * sizeof(char));
        }

        j = 0;
        while (j < recvlen)
        {
            packet[i++] = buffer[j++];
        }
        if (packet[i - 1] == '\0')
        {
            break;
        }
    }
    return packet;
}

int main(int argc, char* argv[])
{   
    if(argc != 2){
        fprintf(stderr, "Invalid arguments.\n");
        fprintf(stderr, "Usage: <client_executable> <lb_port_number>\n");
        exit(1);
    }
    
    int sockfd;
    char ip[] = "127.0.0.1";
    uint16_t port = (uint16_t) atoi(argv[1]);
	struct sockaddr_in serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed");
		exit(1);
	}

    fprintf(stdout, "Socket created\n");

	serv_addr.sin_family = AF_INET;
	inet_aton(ip, &serv_addr.sin_addr);
	serv_addr.sin_port = htons(port);

    fprintf(stdout, "Requesting %s:%u\n", ip, port);
 
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect() failed");
		exit(1);
	}
    
    fprintf(stdout, "Connected\n");

    char* time;
    if((time = recvString(sockfd)) == NULL)
    {
        exit(1);
    }

    printf("%s", time);
    
	close(sockfd);

    fprintf(stdout, "Connection closed\n");

	return 0;
}


