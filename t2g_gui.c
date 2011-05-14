#include <gtk/gtk.h>
#include <glib.h>
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
char timee[32];
const char *usend = "\n\nsend : ";
const char *urecv = "\n\nreceive : ";
#define GET_TIME()                      \
({                                      \
 time_t t;                              \
(time(&t) == -1 ? NULL : ctime(&t));    \
 })

char buf[BUF_SIZE];
void talk(int fd);
void send_msg();
void recv_msg();

GtkWidget *window;
GtkWidget *button_enter;
GtkWidget *button_client;
GtkWidget *button_server;
GtkWidget *table;
GtkWidget *messagebox;
GtkWidget *messageinput;
GtkWidget *messagebox_scroll;
GtkWidget *messageinput_scroll;
GtkTextBuffer *messagebox_buffer;
GtkTextBuffer *messageinput_buffer;
int fd_talk;

int destory_event()
{
        gtk_main_quit();
        return FALSE;
}

void start_as_server()
{
        int in_fd,listen_fd;
        struct sockaddr_in listen_addr , out_addr ; 
        socklen_t len; 
        char connect_addr[INET_ADDRSTRLEN];
        
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        in_fd = socket(AF_INET, SOCK_STREAM, 0);
        if((!listen_fd) || (!in_fd))
        {
                printf("fd error!\n");
                return ;
        }
        memset(&(listen_addr.sin_zero), 0, sizeof(listen_addr.sin_zero));

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
                printf("Talk2Gals : Connect to the connector!\nWe're good to talk!\n");
                fd_talk = in_fd;
        }
        gdk_input_add(fd_talk, GDK_INPUT_READ, recv_msg, NULL);
        return ;
}


void start_as_client(int *ret)
{
        int out_fd;
        char connect_addr[INET_ADDRSTRLEN];
        char host[INET_ADDRSTRLEN] = {0};
        struct sockaddr_in out_addr;
        struct addrinfo *ai;
        socklen_t len;
        /* POP-UP A DIALOG TO INPUT HOST ADDRESS */
        GtkWidget *dialog;
        GtkWidget *label;
        GtkWidget *entry_addr;
        GtkEntryBuffer *entry_addr_buffer;
        label = gtk_label_new("enter server address");
        entry_addr_buffer = gtk_entry_buffer_new("192.168.1.111", -1);
        entry_addr = gtk_entry_new_with_buffer(entry_addr_buffer);
        dialog = gtk_dialog_new_with_buttons("server addr ?", GTK_WINDOW (window),
                        GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
                        "connect",
                        GTK_RESPONSE_ACCEPT,
                        GTK_STOCK_OK,
                        GTK_RESPONSE_OK,
                        NULL);
        gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), label);
        gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), entry_addr);
        gtk_widget_show_all(dialog);
        gtk_dialog_run(GTK_DIALOG(dialog));
        strcpy(host, gtk_entry_buffer_get_text(entry_addr_buffer));
        if(host[0])
        {
                gtk_widget_destroy(dialog);
                printf("host = %s\n",host);
        }

        /* POP-UP A DIALOG TO INPUT HOST ADDRESS */
        out_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(getaddrinfo(host, NULL, NULL, &ai) != 0)
        {
                printf("Talk2Gals : Host error!\n");
                while(1) ;
        }
        out_addr = *((struct sockaddr_in *)(ai->ai_addr));
        out_addr.sin_port = htons(C_PORT);
        inet_ntop(AF_INET, &out_addr, connect_addr, INET_ADDRSTRLEN);
        printf("Talk2Gals : Getaddrinfo successed!\n");
        printf("Talk2Gals : Connecting addr = %s , port = %d\n", connect_addr ,out_addr.sin_port);
        len = sizeof(struct sockaddr);
        if(connect(out_fd, (struct sockaddr *)&out_addr, len) != -1)
        {
                printf("Talk2Gals : Connection established\nWe're good to talk\n");
                fd_talk = out_fd;
        }
        gdk_input_add(fd_talk, GDK_INPUT_READ, recv_msg, NULL);
}
int main(int argc, char *argv[])
{
        gint call_back_vret = 12345678;
        gtk_init(&argc, &argv);
        
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "talk2gals");
        gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);

        button_enter = gtk_button_new_with_label("enter");
        button_client = gtk_button_new_with_label("client");
        button_server = gtk_button_new_with_label("server");

        table = gtk_table_new(300, 400, 0);
        gtk_table_set_col_spacings(GTK_TABLE(table), 1);
        gtk_table_set_row_spacings(GTK_TABLE(table), 1);
        gtk_table_attach_defaults(GTK_TABLE(table), button_client, 10, 30, 10, 20);
        gtk_table_attach_defaults(GTK_TABLE(table), button_server, 40, 60, 10, 20);
        gtk_table_attach_defaults(GTK_TABLE(table), button_enter, 270, 290, 310, 320);

        /* make a scroll windows for message input ,add it to table*/
        messageinput_buffer = gtk_text_buffer_new(NULL);
        messageinput = gtk_text_view_new_with_buffer(messageinput_buffer);
        messageinput_scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messageinput_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(messageinput_scroll), GTK_SHADOW_OUT);
        gtk_container_add(GTK_CONTAINER(messageinput_scroll), messageinput);
        gtk_table_attach_defaults(GTK_TABLE(table), messageinput_scroll, 10, 270, 310, 320);

        /* We will make a scroll windows for messagebox<READ ONLY!> ,add it to table*/
        messagebox_buffer = gtk_text_buffer_new(NULL);
        messagebox = gtk_text_view_new_with_buffer(messagebox_buffer);
        gtk_text_view_set_editable(GTK_TEXT_VIEW(messagebox), FALSE);
        messagebox_scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(messagebox_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(messagebox_scroll), GTK_SHADOW_IN);
        gtk_container_add(GTK_CONTAINER(messagebox_scroll), messagebox);
        gtk_table_attach_defaults(GTK_TABLE(table), messagebox_scroll, 10, 290, 50, 300);



        gtk_container_add(GTK_CONTAINER(window), table);

        g_signal_connect(GTK_OBJECT(window), "delete_event", G_CALLBACK(destory_event), NULL);
        g_signal_connect_swapped(GTK_OBJECT(button_client), "clicked", G_CALLBACK(start_as_client), &call_back_vret);
        g_signal_connect_swapped(GTK_OBJECT(button_server), "clicked", G_CALLBACK(start_as_server), &call_back_vret);
        g_signal_connect_swapped(GTK_OBJECT(button_enter), "clicked", G_CALLBACK(send_msg), &fd_talk);

        gtk_widget_show_all(window);
        gtk_main();
        return 0;
}

void send_msg(int *fd)
{
        size_t send_len;
        GtkTextIter start, end;
        if(gtk_text_buffer_get_char_count(messageinput_buffer) > 0)
        {
                gtk_text_buffer_get_bounds(messageinput_buffer, &start, &end);
                strcpy(buf, gtk_text_buffer_get_text(messageinput_buffer, &start, &end, FALSE));
                send_len = strlen(buf);
                printf("fd = %d buf = %s,send_len = %d\n",*fd, buf,send_len);
        }
        if(send(*fd, buf, send_len, 0) > 0)
                printf("sent %d bytes buf = %s\n",send_len,buf);

        /* copy the message to messagebox */
        sprintf(timee, "%s" "%s", usend, GET_TIME());
        printf("timee = %s\n",timee);
        buf[send_len] = '\n';
        buf[send_len] = '\0';
        gtk_text_buffer_insert_at_cursor(messagebox_buffer, timee, -1);
        gtk_text_buffer_insert_at_cursor(messagebox_buffer, buf, -1);
        /* clear the buffer right after the message is sent */
        gtk_text_buffer_delete(messageinput_buffer, &start, &end);
        return;
}

/* 
 * data and cond is useless in this function,the only reason i did this is to make gcc happy
 * the source is file descriptor
 */
void recv_msg(gpointer data, int source, int cond) 
{
        ssize_t recv_len;
        if((recv_len = recv(source, buf, BUF_SIZE, 0)) > 0)
        {
                printf("recv_len = %d , buf = %s\n",recv_len,buf);
                sprintf(timee, "%s" "%s", urecv, GET_TIME());
                gtk_text_buffer_insert_at_cursor(messagebox_buffer, timee, -1);
                gtk_text_buffer_insert_at_cursor(messagebox_buffer, buf, recv_len);
        }
        return ;
}
