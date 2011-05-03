#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#define LOCAL_ADDR "127.0.0.1"
#define S_PORT 0x7375
#define C_PORT 0x7573
#define LISTEN_PORT 0x6E6E
#define BUF_SIZE 4096
char buf[BUF_SIZE];
void talk(int fd);
void talk_fork(int fd);
int main(int argc, char *argv[])
{
        int listen_fd,out_fd,in_fd,talk_fd;
        struct sockaddr_in listen_addr,in_addr,out_addr;
        struct addrinfo *ai;
        char connect_addr[INET_ADDRSTRLEN];
        socklen_t len;
        bzero(&(listen_addr.sin_zero),sizeof(listen_addr.sin_zero));
        bzero(&(in_addr.sin_zero), sizeof(in_addr.sin_zero));
        bzero(&(out_addr.sin_zero), sizeof(out_addr.sin_zero));

        listen_fd = socket(AF_INET, SOCK_STREAM, 0);

        in_fd = socket(AF_INET, SOCK_STREAM, 0);
        out_fd = socket(AF_INET, SOCK_STREAM, 0);
        if((!listen_fd) || (!in_fd) || (!out_fd))
                printf("fd error!\n");


        if(argc == 2 && (!strcmp(argv[1], "-s"))) /* start as a server */
        {
                listen_addr.sin_family = AF_INET;
                listen_addr.sin_addr.s_addr = INADDR_ANY;
                listen_addr.sin_port = htons(C_PORT);
                len = sizeof(struct sockaddr);
                if(bind(listen_fd, (struct sockaddr *)&listen_addr, len) != -1)
                        printf("Talk2Gals : Bind successed!\n");
                if(listen(listen_fd, 5) != -1)
                        printf("Talk2Gals : Starting to listening , fd = %d\n",listen_fd);

                printf("Talk2Gals : Starting as server!\n");
                printf("Talk2Gals : Waiting for connector!\n");

                in_fd = accept(listen_fd, (struct sockaddr *)&listen_addr, &len);
                printf("Talk2Gals : Talk2Gals : Accepted a Connection!\n");
                /* then connect to the connector , PORT = S*/
                if(getpeername(in_fd, (struct sockaddr *)&out_addr, &len) != -1)
                {
                        inet_ntop(AF_INET, &out_addr, connect_addr, INET_ADDRSTRLEN);
                        out_addr.sin_port = S_PORT;
                        printf("Talk2Gals : Connector addr : %s , port = %d\n",connect_addr, out_addr.sin_port);
                }
                printf("Talk2Gals : Connect to the connector!\nWe're good to talk!\n");
//*/             talk_fork(out_fd);
                talk(in_fd);
        }
        else if(argc == 3 && (!strcmp(argv[1], "-c"))) /* we need to check if argv[2] is an address */
        {
                if(!getaddrinfo(argv[2], NULL, NULL, &ai))
                {
                        out_addr = *((struct sockaddr_in *)(ai->ai_addr));
                        out_addr.sin_port = htons(C_PORT);
                        inet_ntop(AF_INET, &out_addr, connect_addr, INET_ADDRSTRLEN);
                        printf("Talk2Gals : Getaddrinfo successed!\n");
                        printf("Talk2Gals : Connecting addr = %s , port = %d\n", connect_addr ,out_addr.sin_port);
                        len = sizeof(struct sockaddr);
                        connect(out_fd, (struct sockaddr *)&out_addr, len);
                        printf("Talk2Gals : Connection established\nWe're good to talk\n");
//*                        talk_fork(out_fd);
                        talk(out_fd);
                }
                else 
                        goto ERROR;
        }
        else
        {
        ERROR:
                printf("Argument Error! USEAGE:\n./Talk2Gals -s Starting as server\n");
                printf("./Talk2Gals -c HOST_ADDR Starting as Client\n");
        }
        /* Close all sockets and exit */
        close(listen_fd);
        close(in_fd);
        close(out_fd);
        return 0;
}

void talk(int fd)
{
        fd_set fd_in,fd_out;
        struct timeval tv;
        int retval;
        int send_len;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        while(1)
        {
                FD_ZERO(&fd_in);
                FD_ZERO(&fd_out);
                FD_SET(fd,&fd_in);
                FD_SET(0, &fd_in);
                if((retval = select(fd + 1, &fd_in, NULL, NULL, NULL)) == -1)
                        printf("Talk2Gals : select error!\n");
                if(FD_ISSET(0, &fd_in))
                {

                        memset(buf, 0,BUF_SIZE);
                        fgets(buf, 4096, stdin);
                        send_len = strlen(buf);
                        send(fd, buf, send_len, 0);
                        if(!strcmp("exit\n",buf))
                        {
                                printf("You're offline now!\n");
                                goto DONE;
                        }
                }
                if(FD_ISSET(fd, &fd_in))
                {
                        memset(buf, 0,BUF_SIZE);
                        printf("Got Message :\n");
                        recv(fd, buf, BUF_SIZE, 0);
                        printf("%s",buf);
                        if(!strcmp("exit\n",buf))
                        {
                                printf("She's offline now!\n");
                                goto DONE;
                        }
                }
        }
DONE:
        return;
}
void talk_fork(int fd)
{
        int len;
        if(!fork())
        {
                while(1)
                {
                        if(recv(fd, buf, BUF_SIZE, 0) > 0)
                                printf("\nGot Message :\n%s\n",buf);
                        if(!strcmp("exit\n",buf))
                        {
                                printf("She's offline!\n");
                                return;
                        }
                        memset(buf, 0,BUF_SIZE);
                }
        }
        else
        {
                while(1)
                {
                        if(fgets(buf, BUF_SIZE, stdin) != NULL)
                        {
                                len = strlen(buf);
                                send(fd, buf, len, 0);
                                if(!strcmp("exit\n",buf))
                                {
                                        printf("You're offline!\n");
                                        return;
                                }
                                memset(buf, 0,BUF_SIZE);
                        }
                }
        }
}
