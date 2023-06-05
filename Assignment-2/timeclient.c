// A Simple Client Implementation
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#define IP "127.0.0.1"
#define PORT 20000
#define TIMEOUT 3000
#define MAX_TRIAL 5
#define FDSIZE 1
#define BUFSIZE 50

int main()
{
    srand((unsigned)time(NULL));

    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len;
    char buffer[BUFSIZE];
    struct pollfd fdset[FDSIZE];
    nfds_t ret;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_aton(IP, &servaddr.sin_addr);

    strcpy(buffer, "CLIENT: REQUEST FOR LOCAL TIME");

    int trial = 1;
    while (trial++ <= MAX_TRIAL)
    {
        /*
            Message send may or may not be recieved by client.
            This can be simulated by randomly choosing whether to send or not
        */

        // if (rand() % 2 == 0)
        // {
        if (sendto(sockfd, (char *)buffer, BUFSIZE, 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("sendto() failed");
            exit(EXIT_FAILURE);
        }
        // }

        fprintf(stdout, "Requesting server....\n");

        fdset[0].fd = sockfd;
        fdset[0].events = POLLIN;
        time_t start, end;
        time(&start);
        ret = poll(fdset, 1, TIMEOUT);
        time(&end);
        printf("TIme = %lf\n", difftime(start, end));
        if (ret < 0)
        {
            perror("poll() failed");
            exit(EXIT_FAILURE);
        }

        else if (ret > 0)
        {
            memset(buffer, 0, BUFSIZE);
            len = sizeof(servaddr);

            if (recvfrom(sockfd, (char *)buffer, BUFSIZE, 0, (struct sockaddr *)&servaddr, &len) < 0)
            {
                perror("recvfrom() failed");
                exit(EXIT_FAILURE);
            }

            fprintf(stdout, "%s", buffer);

            break;
        }

        fprintf(stdout, "[-] Request failed\n");
    }

    if (trial > MAX_TRIAL)
    {
        fprintf(stdout, "Timeout exceeded\n");
    }

    close(sockfd);

    fprintf(stdout, "Socket closed\n");

    return 0;
}