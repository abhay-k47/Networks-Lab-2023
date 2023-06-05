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

#define BUFLEN 50

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

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Invalid arguments\n");
        fprintf(stderr, "Usage: <server_executable> <port_number>\n");
        exit(EXIT_FAILURE);
    }

    int sockfd, newsockfd, curr_load;
    char ip[] = "127.0.0.1";
    uint16_t port = (uint16_t)atoi(argv[1]);
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    srand((unsigned)(port));

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

    fprintf(stdout, "Server %s:%u running...\n\n", ip, port);

    while (1)
    {
        clilen = sizeof(cli_addr);

        if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) < 0)
        {
            perror("accept() failed");
            break;
        }

        fprintf(stdout, "%s:%u connected\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        char *request;
        if ((request = recvString(newsockfd)) == NULL)
        {
            break;
        }

        if (strcmp(request, "Send Load") == 0)
        {
            curr_load = 1 + rand() % 100;
            // char buf[BUFLEN];
            // sprintf(buf, "%d", curr_load);
            // if (sendString(newsockfd, buf, strlen(buf) + 1) < 0)
            // {
            //     break;
            // }
            uint32_t num = htonl(curr_load);
            if (send(newsockfd, &num, sizeof(num), 0) < 0)
            {
                perror("send() failed");
                break;
            }

            fprintf(stdout, "Load sent: %d\n", curr_load);
        }

        else if (strcmp(request, "Send Time") == 0)
        {
            char buf[BUFLEN];
            memset(buf, 0, BUFLEN);
            time_t rawtime = time(NULL);
            asctime_r(localtime(&rawtime), buf);

            if (sendString(newsockfd, buf, strlen(buf) + 1) < 0)
            {
                break;
            }

            fprintf(stdout, "Localtime sent\n");
        }

        close(newsockfd);

        fprintf(stdout, "Connection closed\n\n");
    }

    close(sockfd);
    fprintf(stdout, "\nServer closed\n");

    return 0;
}

