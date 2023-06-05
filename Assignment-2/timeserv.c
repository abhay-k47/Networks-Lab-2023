// A Simple UDP Server that sends a HELLO message
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define PORT 20000
#define BUFSIZE 50

int main()
{
    srand((unsigned)time(NULL));
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUFSIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socket created\n");

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Local address binded\n");

    fprintf(stdout, "Server Running....\n");

    while (1)
    {

        len = sizeof(cliaddr);

        if (recvfrom(sockfd, (char *)buffer, BUFSIZE, 0, (struct sockaddr *)&cliaddr, &len) < 0)
        {
            perror("recvfrom() failed");
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Client request recieved\n");

        memset(buffer, 0, BUFSIZE);

        time_t timer = time(NULL);
        asctime_r(localtime(&timer), buffer);

        /*
            Message send may or may not be recieved by client.
            This can be simulated by randomly choosing whether to send or not
        */

        // sleep(5);

        // if (rand() % 2 == 0)
        // {
        // if (sendto(sockfd, (char *)buffer, BUFSIZE, 0, (struct sockaddr *)&cliaddr, len) < 0)
        // {
        //     perror("sendto() failed");
        //     exit(EXIT_FAILURE);
        // }

        fprintf(stdout, "[+] Localtime sent to client\n\n");
        // }
    }

    close(sockfd);

    fprintf(stdout, "Socket closed\n");

    return 0;
}
