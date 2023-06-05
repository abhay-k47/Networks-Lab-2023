#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define BUFLEN 50
#define TIMEOUT 5000
#define FDSETSIZE 1
// send size_n bytes, return -1 if fails
int sendString(int sockfd, const char *input, int size_n)
{
    int i, j;
    char buffer[BUFLEN];

    i = 0;
    while (i < size_n)
    {
        memset(buffer, 0, BUFLEN);
        j = 0;
        while (j < BUFLEN && i < size_n)
        {
            buffer[j++] = input[i++];
        }
        if (send(sockfd, buffer, j, 0) < 0)
        {
            perror("send() failed");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

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


// Handles client's request
void communicate(int sockfd_to_client, const char *serv_ip, uint16_t serv_port)
{
    int sockfd_to_serv;
    struct sockaddr_in serv_addr;
    if ((sockfd_to_serv = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Sending client request to %s:%u\n", serv_ip, serv_port);

    serv_addr.sin_family = AF_INET;
    inet_aton(serv_ip, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(serv_port);

    if (connect(sockfd_to_serv, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect() failed");
        exit(1);
    }

    if (sendString(sockfd_to_serv, "Send Time", 10) < 0)
    {
        exit(1);
    }

    char *time;
    if ((time = recvString(sockfd_to_serv)) == NULL)
    {
        exit(1);
    }

    close(sockfd_to_serv);

    if (sendString(sockfd_to_client, time, strlen(time) + 1) < 0)
    {
        exit(1);
    }

}

// connects to server and gets load from it
int getLoad(const char *serv_ip, uint16_t serv_port)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    inet_aton(serv_ip, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(serv_port);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect() failed");
        exit(1);
    }

    if (sendString(sockfd, "Send Load", 10) < 0)
    {
        exit(1);
    }

    uint32_t num = 0;
    if (recv(sockfd, &num, sizeof(num), 0) < 0)
    {
        perror("recv() failed");
        exit(1);
    }

    close(sockfd);

    return ntohl(num);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid arguments\n");
        fprintf(stderr, "Usage: <lb_executable> <port_number> <server_1_port> <server_2_port>\n");
        exit(1);
    }

    char ip[] = "127.0.0.1", serv1_ip[] = "127.0.0.1", serv2_ip[] = "127.0.0.1";
    uint16_t port = (uint16_t)atoi(argv[1]), serv1_port = (uint16_t)atoi(argv[2]), serv2_port = (uint16_t)atoi(argv[3]);
    int serv1_load, serv2_load, ret;

    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr, serv_addr;
    struct pollfd fdset[FDSETSIZE];
    time_t tStart, tEnd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socket created\n");

    serv_addr.sin_family = AF_INET;
    inet_aton(ip, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Local address binded\n");

    if (listen(sockfd, 5) < 0)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Load balancer %s:%u running.....\n\n", ip, port);

    while (1)
    {
        serv1_load = getLoad(serv1_ip, serv1_port);
        fprintf(stdout, "\nLoad received from %s:%u  %d\n", serv1_ip, serv1_port, serv1_load);

        serv2_load = getLoad("127.0.0.1", serv2_port);
        fprintf(stdout, "Load received from %s:%u  %d\n\n", serv2_ip, serv2_port, serv2_load);

        int timeout = TIMEOUT;

        while (1)
        {
            fdset[0].fd = sockfd;
            fdset[0].events = POLLIN;

            // fprintf(stderr, "poll for %d ms\n", timeout);

            time(&tStart);
            ret = poll(fdset, 1, timeout);
            time(&tEnd);

            if (ret < 0)
            {
                perror("poll() failed");
                exit(1);
            }
            else if (ret == 0)
            {
                break;
            }
            else
            {
                timeout -= 1000 * difftime(tEnd, tStart);

                clilen = sizeof(cli_addr);

                if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) < 0)
                {
                    perror("accept() failed");
                    exit(EXIT_FAILURE);
                }

                fprintf(stdout, "%s::%u  connected\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

                // child process
                if (fork() == 0)
                {
                    close(sockfd);

                    if (serv1_load < serv2_load)
                        communicate(newsockfd, serv1_ip, serv1_port);
                    else
                        communicate(newsockfd, serv2_ip, serv2_port);

                    close(newsockfd);

                    exit(EXIT_SUCCESS);
                }

                close(newsockfd);

            }
        
        }
        
    }

    close(sockfd);

    return 0;
}
