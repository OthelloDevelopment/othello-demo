/* 八皇后问题测试程序 othello_test.c*/
#include <stdlib.h>
#include <gtk/gtk.h>
#include "othello_server.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>							// 包含套接字函数库
#include <stdio.h>
#include <netinet/in.h>							// 包含AF_INET相关结构
#include <arpa/inet.h>							// 包含AF_INET相关操作的函数
#include <unistd.h>
#define PORT 3339


// char recvmsg1[256];
// int c_fd;
//八皇后控件的“Othello”信号的回调函数
static void win(GtkWidget *othello, gpointer data) {
    othello_clear(OTHELLO(othello));//清除棋盘中的皇后
}

int main(int argc, char *argv[]) {
    
    // int s_fd;	// 服务器和客户套接字标识符
    // int s_len, c_len;			// 服务器和客户消息长度
    // struct sockaddr_in s_addr;	// 服务器套接字地址
    // struct sockaddr_in c_addr;	// 客户套接字地址
    // s_fd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    // s_addr.sin_family = AF_INET;	// 定义服务器套接字地址中的地址域为IPv4
    // s_addr.sin_addr.s_addr=htonl(INADDR_ANY); // 定义套接字地址
    // s_addr.sin_port = htons(PORT); // 定义服务器套接字端口
    // s_len = sizeof(s_addr);
    // bind(s_fd, (struct sockaddr *) &s_addr, s_len);	// 绑定套接字与设置的端口号
    // listen(s_fd, 1000); //监听，最大连接请求设为10
    // printf("请稍候，等待客户端发送数据\n");
    // c_len = sizeof(c_addr); //接收客户端连接请求
    // c_fd = accept(s_fd,(struct sockaddr *) &c_addr,(socklen_t *__restrict) &c_len);
    

    GtkWidget *window, *vbox, *hbox;
    GtkWidget *label, *image, *statusbar;
    GtkWidget *othello;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "黑白棋");

    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);

    image = gtk_image_new_from_stock(GTK_STOCK_HELP, GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 3);

    label = gtk_label_new("这是黑白棋的服务端程序");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 3);

    othello = othello_new();
    gtk_box_pack_start(GTK_BOX(vbox), othello, FALSE, FALSE, 3);

    //向自定义控件连接信号"Othello"
    g_signal_connect(G_OBJECT(othello), "Othello", G_CALLBACK(win), NULL);

    statusbar = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 3);
    gtk_statusbar_push(statusbar, 1, "");
    gtk_widget_show_all(window);
    gtk_main();

    close(c_fd);							// 关闭连接
    return 0;
}