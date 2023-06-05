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

#define IP "127.0.0.1"
#ifndef PORT
#define PORT 20000
#endif
#define MAXLEN 26
#define BUFLEN 50
#define CMDLEN 256

// send size_n bytes, return -1 if fails
int sendToServer(int sockfd, const char *input, int size_n)
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
char *recvFromServer(int sockfd)
{
    int i, j, recvlen, capacity = 1;
    char buffer[BUFLEN];
    char *packet = (char *)malloc(capacity * sizeof(char));

    i = 0;
    memset(buffer, 0, BUFLEN);
    while (1)
    {
        recvlen = recv(sockfd, buffer, BUFLEN, 0);
        if (recvlen <= 0)
        {
            perror("recv() failed");
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

int main()
{
    int sockfd, recvlen, nargs;
    struct sockaddr_in serv_addr;
    char username[MAXLEN], buffer[BUFLEN], command[CMDLEN], check[5], *input;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socket created\n");

    serv_addr.sin_family = AF_INET;
    inet_aton(IP, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Connected to server: %s port: %u\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

    if ((input = recvFromServer(sockfd)) == NULL)
        exit(EXIT_FAILURE);
    fprintf(stdout, "%s", input);

    memset(username, 0, MAXLEN);
    scanf("%25[^\n]s", username);
    getchar();
    sendToServer(sockfd, username, strlen(username) + 1);

    if ((input = recvFromServer(sockfd)) == NULL)
        exit(EXIT_FAILURE);
    if (strcmp(input, "NOT-FOUND") == 0)
    {
        fprintf(stdout, "Invalid username\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        fprintf(stdout, "\n%s@%s::%u: $ ",username, inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

        nargs = scanf("%255[^\n]s", command);
        getchar();

        if (nargs == 0)
            continue;

        if (sendToServer(sockfd, command, strlen(command) + 1) < 0)
            exit(EXIT_FAILURE);
        memset(buffer, 0, BUFLEN);
        while ((recvlen = recv(sockfd, buffer, BUFLEN - 1, 0)) > 0)
        {
            buffer[recvlen] = '\0';
            if (strcmp(buffer, "$$$$") == 0)
            {
                fprintf(stdout, "Invalid command");
            }
            else if (strcmp(buffer, "####") == 0)
            {
                fprintf(stdout, "Error in running command");
            }
            else
            {
                fprintf(stdout, "%s", buffer);
            }

            if (buffer[recvlen - 1] == '\0')
                break;
            memset(buffer, 0, BUFLEN);
        }
        if (sscanf(command, "%s%4s", buffer, check) == 1 && strcmp(buffer, "exit") == 0)
            break;
    }

    close(sockfd);

    fprintf(stdout, "Connection closed\n");

    return 0;
}
