#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define TEST_ADDR "www.baidu.com"
#define SERVER_ADDR "1.2.3.4"
#define SERVER_PORT 0x7375
#define BUF_SIZE 4096
char buf[BUF_SIZE];
int main()
{
        int sockfd,newfd;
        struct sockaddr_in hostaddr;
        struct sockaddr_in client_addr;
        char paddr[INET_ADDRSTRLEN];
        socklen_t client_addr_len;
        hostaddr.sin_family = AF_INET;
        hostaddr.sin_port = htons(SERVER_PORT);
        hostaddr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(hostaddr.sin_zero), sizeof(hostaddr.sin_zero));
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                printf("SERVER : Get socket error!\n");
        else
                printf("SERVER : Get socket %d\n",sockfd);
        if(bind(sockfd, (struct sockaddr *)&hostaddr, sizeof(struct sockaddr)) == -1)
                printf("SERVER : bind error! &hostaddr = %08x , sizeof(struct sockaddr) = %d\n",(uint32_t)&hostaddr,sizeof(struct sockaddr));
        else
                printf("SERVER : bind successed!\n");
        if(listen(sockfd, 5) == -1)
                printf("SERVER : listen fail!");
        else
                printf("SERVER : listen successed!,ready to accept!\n");
        client_addr_len = sizeof(struct sockaddr_in);
        if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) > 0)
        {
                printf("SERVER : accept! newfd = %d\n",newfd);
                printf("SERVER : CLIENT : %s connected port = %d!\n",
                inet_ntop(AF_INET, (struct sockaddr *)&client_addr, paddr, INET_ADDRSTRLEN), client_addr.sin_port);
        }
        while(1)
        {
                if(recv(newfd, buf, BUF_SIZE, 0) > 0)
                        printf("RECV : %s",buf);
                if(strcmp("exit\n",buf) == 0)
                {
                        printf("SERVER : CLIENT EXIT!\n");
                        goto EXIT;
                }
        }
EXIT:
        close(newfd);
        close(sockfd);
        return 0;
}
