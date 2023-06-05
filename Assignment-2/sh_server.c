#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#ifndef PORT
#define PORT 20000
#endif
#define MAXCLIENT 5
#define BUFLEN 50
#define MAXLEN 26
#define DIRLEN 201

// send size_n bytes, return -1 if fails
int sendToClient(int sockfd, const char *input, int size_n)
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
char *recvFromClient(int sockfd)
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

// validate username from client, exits process on any errors
void validateUser(int sockfd)
{
    FILE *fp;
    char username[MAXLEN], *input;
    int isValid;

    if (sendToClient(sockfd, "LOGIN: ", 8) < 0)
    {
        exit(EXIT_FAILURE);
    }

    if ((input = recvFromClient(sockfd)) == NULL)
    {
        exit(EXIT_FAILURE);
    }

    if ((fp = fopen("./user.txt", "r")) == NULL)
    {
        perror("fopen() failed");
        exit(EXIT_FAILURE);
    }

    isValid = 0;
    while (fgets(username, MAXLEN, fp) != NULL)
    {
        username[strcspn(username, "\n")] = '\0';
        if (strcmp(input, username) == 0)
        {
            isValid = 1;
            break;
        }
    }

    fclose(fp);

    if (isValid == 0)
    {
        sendToClient(sockfd, "NOT-FOUND", 10);
        exit(EXIT_FAILURE);
    }

    if (sendToClient(sockfd, "FOUND", 6) < 0)
    {
        exit(EXIT_FAILURE);
    }
}

void communicate(int newsockfd)
{
    while (1)
    {
        char op[10], dir[DIRLEN], check[5];
        char *command = recvFromClient(newsockfd);
        
        if (sscanf(command, "%9s", op) == EOF)
        {
            sendToClient(newsockfd, "\0", 1);
            continue;
        }

        command += strspn(command, " \t");
        command += strcspn(command, " \t");

        int nargs = sscanf(command, "%200s%4s", dir, check);

        if (strcmp(op, "pwd") == 0)
        {
            if (nargs != EOF)
            {
                if (sendToClient(newsockfd, "$$$$", 5) < 0)
                    exit(EXIT_FAILURE);
                continue;
            }

            char *d_name = getcwd(NULL, 0);
            if (d_name == NULL)
            {
                if (sendToClient(newsockfd, "####", 5) < 0)
                    exit(EXIT_FAILURE);
            }
            else if (sendToClient(newsockfd, d_name, strlen(d_name) + 1) < 0)
                exit(EXIT_FAILURE);
        }

        else if (strcmp(op, "dir") == 0)
        {
            DIR *pDir;
            struct dirent *pDirent;
            char *d_name;

            // take current directory by default (as in linux)
            if (nargs == EOF)
                strcpy(dir, ".");

            if ((pDir = opendir(dir)) == NULL)
            {
                if (sendToClient(newsockfd, "####", 5) < 0)
                    exit(EXIT_FAILURE);
                continue;
            }

            while ((pDirent = readdir(pDir)) != NULL)
            {
                d_name = pDirent->d_name;
                if (sendToClient(newsockfd, d_name, strlen(d_name)) < 0)
                    exit(EXIT_FAILURE);
                if (sendToClient(newsockfd, "\n", 1) < 0)
                    exit(EXIT_FAILURE);
            }

            if (sendToClient(newsockfd, "\0", 1) < 0)
                exit(EXIT_FAILURE);

            closedir(pDir);
        }

        else if (strcmp(op, "cd") == 0)
        {
            if (nargs > 1)
            {
                if (sendToClient(newsockfd, "$$$$", 5) < 0)
                    exit(EXIT_FAILURE);
                continue;
            }

            // take to home directory by default (as in linux)
            if (nargs == EOF)
            {
                strcpy(dir, getpwuid(getuid())->pw_dir);
            }

            if (chdir(dir) < 0)
            {
                if (sendToClient(newsockfd, "####", 5) < 0)
                    exit(EXIT_FAILURE);
            }
            else if (sendToClient(newsockfd, "\0", 1) < 0)
                exit(EXIT_FAILURE);
        }

        else if (strcmp(op, "exit") == 0)
        {

            if (nargs != EOF)
            {
                if (sendToClient(newsockfd, "$$$$", 5) < 0)
                    exit(EXIT_FAILURE);
                continue;
            }

            if (sendToClient(newsockfd, "\0", 1) < 0)
                exit(EXIT_FAILURE);
            return;
        }

        else
        {
            if (sendToClient(newsockfd, "$$$$", 5) < 0)
                exit(EXIT_FAILURE);
            continue;
        }
    }
}

int main()
{
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr, serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socket created\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Local address binded\n");

    if (listen(sockfd, MAXCLIENT) < 0)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server listening.....\n\n");

    while (1)
    {
        clilen = sizeof(cli_addr);

        if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) < 0)
        {
            perror("accept() failed");
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "%s::%u ### connected\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // child process
        if (fork() == 0)
        {
            close(sockfd);
            validateUser(newsockfd);
            fprintf(stdout, "%s::%u ### logged-in\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            communicate(newsockfd);
            close(newsockfd);
            fprintf(stdout, "%s::%u ### disconnected\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            exit(EXIT_SUCCESS);
        }

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
