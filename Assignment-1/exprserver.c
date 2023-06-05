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

#define PACKSIZE 50
#define VALSIZE 20
#define DIVISION_BY_ZERO -1e17
#define NOP '\0'

int isDigit(char d)
{
    return (d >= '0' && d <= '9');
}

int isOperator(char op)
{
    return (op == '+' || op == '-' || op == '*' || op == '/');
}

double evaluateParenthesizedExpr(char* expression, int exprlen, int *evallen)
{
    double value = 0, temp, exponent;
    char operator = NOP;
    int i = *evallen+1;

    while(expression[i] != ')')
    {
        if(isOperator(expression[i])) operator = expression[i];
        else
        {
            if(expression[i] == '('){
                temp = evaluateParenthesizedExpr(expression, exprlen, &i);
                if(temp==DIVISION_BY_ZERO) return DIVISION_BY_ZERO;
            }
            else                                        
            {
                temp = 0;
                while(isDigit(expression[i]))
                {
                    temp = temp*10 + (expression[i]-'0');
                    i++;
                }

                if(expression[i] == '.') i++;

                exponent = 0.1;
                while(isDigit(expression[i]))
                {
                    temp = temp + exponent*(expression[i]-'0');
                    exponent /= 10;
                    i++;
                }
                i--;                                    
            }
            
            // perform operation
            switch(operator)                           
            {
                case NOP :                             
                    value = temp;
                    break;
                case '+' :
                    value += temp;
                    break;
                case '-' :
                    value -= temp;
                    break;
                case '*' :
                    value *= temp;
                    break;
                case '/' :
                    if(abs(temp) < 1e-9) return DIVISION_BY_ZERO;
                    value /= temp;
                    break;
            }
        }
        i++;
    }

    *evallen = i;                                        
    return value;
}

double evaluateExpr(char* expression, int exprlen)
{
    double value = 0, temp, exponent;
    char operator = NOP;      
    int i=0;                        
    while(i<exprlen)
    {
        if(isOperator(expression[i])) operator = expression[i];
        else
        {
            if(expression[i] == '('){
                temp = evaluateParenthesizedExpr(expression, exprlen, &i);
                if(temp==DIVISION_BY_ZERO) return DIVISION_BY_ZERO;
            }
            else                                      
            {
                temp = 0;
                while(isDigit(expression[i]))
                {
                    temp = temp*10 + (expression[i]-'0');
                    i++;
                }
                if(expression[i] == '.') i++;
                exponent = 0.1;
                while(isDigit(expression[i]))
                {
                    temp = temp + exponent*(expression[i]-'0');
                    exponent /= 10;
                    i++;
                }
                i--;                                
            }

            // perform operation
            switch(operator)                         
            {
                case NOP : 
                    value = temp;
                    break;
                case '+' :
                    value += temp;
                    break;
                case '-' :
                    value -= temp;
                    break;
                case '*' :
                    value *= temp;
                    break;
                case '/' :
                    if(abs(temp) < 1e-9) return DIVISION_BY_ZERO;
                    value /= temp;
                    break;
            }
        }
        i++;
    }
    return value;
}


int main()
{
	int	sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in	cli_addr, serv_addr;

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

        fprintf(stdout, "Connection with client: %s port: %u accepted\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // while expression
        while(1)
        {
            size_t maxlen = 1, exprlen=0, packlen;
            char* expr = (char*) malloc(maxlen*sizeof(char));
            char pack[PACKSIZE];

            // while packets
            int connection_status=1;
            while(1)
            {
                packlen = recv(newsockfd, pack, PACKSIZE, 0);
                if(packlen < 0)
                {
                    fprintf(stderr, "recv() failed: %s\n", strerror(errno));
                    exit(1);
                }

                if(packlen==0)
                {
                    connection_status=0;
                    break;
                }

                if(maxlen<exprlen+packlen)
                {
                    maxlen = exprlen+packlen;
                    expr = (char*)realloc(expr, maxlen*sizeof(char));
                }

                for(int i=0; i<packlen; i++){
                    if(pack[i]!=' '){
                        expr[exprlen++]=pack[i];
                    }
                }

                if(pack[packlen-1]==0) break;
            }

            

            if( !connection_status )
            {   
                fprintf(stderr, "Client forcefully ");
                free(expr);
                break;            
            }

            if(exprlen==3 && expr[0]=='-' && expr[1]=='1')
            {   
                free(expr);
                break;            
            }

            printf("%ld bytes recieved\n",maxlen);


            double value = evaluateExpr(expr, exprlen-1);
            char val[VALSIZE];
            
            if(value==DIVISION_BY_ZERO) sprintf(val, "DIVISION BY ZERO");
            else sprintf(val, "%lf", value);
            
            if(send(newsockfd, val, VALSIZE, 0) < 0)
            {
                fprintf(stderr, "recv() failed: %s\n", strerror(errno));
                exit(1);
            }

            fprintf(stdout, "Expression evaluated and sent to client\n");

            free(expr);
 
        }

        close(newsockfd);

        fprintf(stdout, "closed connection \n\n");
	}

	return 0;
}
