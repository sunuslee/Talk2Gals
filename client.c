#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define TEST_ADDR "www.baidu.com"
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 0x7375
char buf[4096];
int main()
{
        int sockfd;
        int msglen;
        char paddr[INET_ADDRSTRLEN];
        struct sockaddr_in *addr;
        struct addrinfo *ailist;
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                printf("CLIENT : Get socket error!\n");
        else
                printf("CLIENT : Get socket %d\n",sockfd);
        if(!getaddrinfo(SERVER_ADDR, NULL, NULL, &ailist))
        {
                addr = (struct sockaddr_in *)ailist->ai_addr;
                addr->sin_port = htons(SERVER_PORT);
                printf("CLIENT : af = %d , AF_INET = %d , port = %d\n",addr->sin_family,AF_INET,addr->sin_port);
        }
        if(connect(sockfd, (struct sockaddr *)addr, sizeof(struct sockaddr)) == 0)
        {
                printf("CLIENT : connect to %s successed , port : %d\n", inet_ntop(AF_INET, &addr, paddr, INET_ADDRSTRLEN),addr->sin_port);
        }
        while(1)
        {
                if(fgets(buf, 4096, stdin) != NULL)
                {
                        msglen = strlen(buf);
                        printf("CLIENT : ready to send msg %s , len = %d\n",buf,msglen);
                        send(sockfd, (const void *)buf, msglen+1, 0);
                        if(strcmp("exit\n",buf) == 0)
                        {
                                printf("CLIENT : EXIT\n");
                                goto EXIT;
                        }
                }
        }
EXIT:
        return 0;
}
