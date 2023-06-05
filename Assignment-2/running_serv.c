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
#include <dirent.h>

#ifndef PORT
#define PORT 20000
#endif
#define MAXCLIENT 5
#define BUFSIZE 50
#define MAXLEN 26

int validateUsername(char *input)
{
    char username[MAXLEN];
    FILE *fp = fopen("./user.txt", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "fopen() failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (fgets(username, MAXLEN, fp) != NULL)
    {
        username[strcspn(username, "\n")] = '\0';
        if (strcmp(input, username) == 0)
        {
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int main()
{
    printf("%d", PORT);
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr, serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Socket created\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "bind() failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Local address binded\n");

    listen(sockfd, MAXCLIENT);

    fprintf(stdout, "Server listening.....\n\n");

    while (1)
    {

        clilen = sizeof(cli_addr);

        if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) < 0)
        {
            fprintf(stderr, "accept() failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Connected to client: %s port: %u accepted\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // child process
        if (fork() == 0)
        {
            close(sockfd);
            char *buffer = (char *)malloc(BUFSIZE * sizeof(char));

            memset(buffer, 0, BUFSIZE);
            strcpy(buffer, "LOGIN: ");
            send(newsockfd, buffer, strlen(buffer) + 1, 0);

            memset(buffer, 0, BUFSIZE);
            recv(newsockfd, buffer, BUFSIZE, 0);
            printf("recv: \'%s\'\n", buffer);

            int isValid = validateUsername(buffer);
            memset(buffer, 0, BUFSIZE);
            strcpy(buffer, isValid ? "FOUND" : "NOT-FOUND");
            send(newsockfd, buffer, strlen(buffer) + 1, 0);

            if(!isValid){
                free(buffer);
                exit(EXIT_FAILURE);
            }
            
            memset(buffer, 0, BUFSIZE);
            recv(newsockfd, buffer, BUFSIZE, 0);
            printf("recv: \'%s\'\n", buffer);

            char command[10];
            if (sscanf(buffer, "%9s", command) > 0)
                buffer += strcspn(buffer += strspn(buffer, " \t"), " \t");

            if (strcmp(command, "pwd") == 0)
            {
                printf("pwd\n");
                char *d_name = getcwd(NULL, 0);
                // while (sscanf(d_name, "%50s", buffer) > 0)
                // {
                //     send(newsockfd, buffer, sizeof(buffer), 0);
                //     d_name += strcspn(d_name += strspn(d_name, " \t"), " \t");
                // }
                printf("%s\n", d_name);
            }

            else if (strcmp(command, "dir") == 0)
            {
                printf("dir\n");
                while (sscanf(buffer, "%s", command) > 0)
                {
                    printf("directory: \'%s\'\n", command);
                    DIR *pDir = opendir(command);
                    if(pDir==NULL) printf("ERROR\n");
                    struct dirent *pDirent;
                    while ((pDirent = readdir(pDir)) != NULL)
                    {
                        char *d_name = pDirent->d_name;
                        printf("%s\n", d_name);
                        // while (sscanf(d_name, "%50s", buffer) > 0)
                        // {
                        //     send(newsockfd, buffer, sizeof(buffer), 0);
                        //     d_name += strcspn(d_name += strspn(d_name, " \t"), " \t");
                        // }
                    }

                    buffer += strcspn(buffer += strspn(buffer, " \t"), " \t");
                }
            }
            else if (strcmp(command, "cd") == 0)
            {
                printf("cd\n");
                sscanf(buffer, "%9s", command);
                chdir(command);
            }
            else
            {
                printf("error\n");
                memset(buffer, 0, BUFSIZE);
                strcpy(buffer, "$$$$");
                printf("%s\n", buffer);
                // send(newsockfd, buffer, strlen(buffer) + 1, 0);
            }

            // free(buffer);
            exit(EXIT_SUCCESS);
        }

        close(newsockfd);
    }
    close(sockfd);
    return 0;
}
