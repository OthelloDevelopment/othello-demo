/* 八皇后问题控件 Othello.c */
#include <gtk/gtk.h>
#include "othello_server.h"

//定义控件的信号的枚举编号
enum {
    OTHELLO_SIGNAL, //清除信号
    LAST_SIGNAL //最后一个信号
};
//定义数组保存摆放在棋盘中的皇后的位置，0表示无皇后，1表示有皇后
static int area[8][8] = {{2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2}};
static int all = 0; // the sum of chess
static int turn = 0; // 1 is black, 0 is white
int nx, ny; // record x, y
// int recvmsg1[8][8];
extern char recvmsg1[256];
char sendmsg[256];
static void dialog_show(Othello *othello); // show the status and clear the board

//以下三个函数为初始化控件成员函数指针，初始化控件成员变量和当状态按钮状态改变时的回调

static void othello_class_init(OthelloClass *klass);

static void othello_init(Othello *othello);

static void othello_toggle(GtkWidget *widget, Othello *othello);

static gint othello_signals[LAST_SIGNAL] = {0}; // save the signal
//取得控件类型
GType othello_get_type(void) {
    static GType othello_type = 0;
    if (!othello_type) { //控件的类型信息结构
        static const GTypeInfo othello_info = {
                sizeof(OthelloClass),
                NULL, NULL,
                (GClassInitFunc) othello_class_init,
                NULL, NULL,
                sizeof(Othello), 0,
                (GInstanceInitFunc) othello_init,
        };
        othello_type = g_type_register_static(GTK_TYPE_VBOX, "Othello", &othello_info, 0);//注册控件类型
    }
    return othello_type;
}

//初始化成员函数指针
static void othello_class_init(OthelloClass *class) {
    GtkObjectClass *object_class;
    object_class = (GtkObjectClass *) class;
    //定义信号,命名为othello
    othello_signals[OTHELLO_SIGNAL] = g_signal_new(
            "Othello",
            G_TYPE_FROM_CLASS(object_class),
            G_SIGNAL_RUN_FIRST,
            0, NULL, NULL, g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE, 0, NULL
    );
    class->othello = NULL;
}

//初始化控件变量成员
static void othello_init(Othello *othello) {
    GtkWidget *table;
    GdkColor col;
    gint i, j;
    //定义颜色
    col.red = 0;
    col.blue = 0;
    col.green = 0;
    //创建格状容器
    table = gtk_table_new(8, 8, TRUE);
    gtk_container_add(GTK_CONTAINER(othello), table);
    gtk_widget_show(table);
//创建状态按钮并添加到格状容器中
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            othello->buttons[i][j] = gtk_toggle_button_new();
            gtk_widget_modify_bg(
                            GTK_WIDGET(othello->buttons[i][j]),
                            GTK_STATE_NORMAL, &col
                            );
            gtk_table_attach_defaults(
                    GTK_TABLE(table),
                    othello->buttons[i][j],
                    i, i + 1, j, j + 1
            );//放按钮
            g_signal_connect(
                    G_OBJECT(othello->buttons[i][j]),
                    "toggled",
                    G_CALLBACK(othello_toggle),
                    othello
            );//加回调函数

            gtk_widget_set_size_request(othello->buttons[i][j], 44, 44);
        }//设置尺寸
    }
    gtk_widget_show_all(othello);
}

//创建八皇后控件的函数
GtkWidget *othello_new(void) {
    return GTK_WIDGET(g_object_new(othello_get_type(), NULL));
}

//清除棋盘上棋子的函数
void othello_clear(Othello *othello) {
    int i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            g_signal_handlers_block_by_func(
                    G_OBJECT(othello->buttons[i][j]), NULL, othello);
            gtk_toggle_button_set_active(
                    GTK_TOGGLE_BUTTON(othello->buttons[i][j]), FALSE);
            g_signal_handlers_unblock_by_func(
                    G_OBJECT(othello->buttons[i][j]), NULL, othello);
            area[i][j] = 0;
            all = 0;
        }
    }
}

void setbutton(int turn, int x, int y, Othello *othello) {
    gpointer iref;
    GtkWidget *image;
    GtkWidget *child;
    if(turn == 1) {
        image = gtk_image_new_from_file("black.png"); // create the black image
    }
    else {
        image = gtk_image_new_from_file("white.png"); // create the white image
    }
    iref = g_object_ref(image);
    gtk_button_set_label(GTK_BUTTON(othello->buttons[x][y]), " ");
    child = gtk_bin_get_child(GTK_BIN(othello->buttons[x][y]));
    if (child != NULL) { //如子控件不为空，则移除状态按钮的子控件
        gtk_container_remove(GTK_CONTAINER(othello->buttons[x][y]), child);
    }
    //添加图像
    gtk_container_add(GTK_CONTAINER(othello->buttons[x][y]), iref);
    //显示图像
    gtk_widget_show(iref);
}


char *encodefunc() {
    int i, j;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            switch(area[i][j]) {
                case 0: sendmsg[i * 8 + j] = "0"; break;
                case 1: sendmsg[i * 8 + j] = "1"; break;
                case 2: sendmsg[i * 8 + j] = "2"; break;
            }
        }
    }
}


void decodefunc() {
    int i, j;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            if(recvmsg1[i * 8 + j] == "0") {
                area[i][j] = 0;
            }
            else if(recvmsg1[i * 8 + j] == "1") {
                area[i][j] = 1;
            }
            else if(recvmsg1[i * 8 + j] == "2") {
                area[i][j] = 2;
            }
        }
    }
}


void rendermap(Othello *othello) {
    int i, j;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            setbutton(area[i][j], i, j, othello);
        }
    }
}


//当表示棋盘格的状态按钮的状态改变时执行
static void othello_toggle(GtkWidget *widget, Othello *othello) {
    int i, j;
    extern int c_fd;
    gboolean istrue = TRUE;
    GtkWidget *image;
    GtkWidget *child;
    gpointer iref;
    GtkWidget *dialog;
//    if(turn == 1) {
//        image = gtk_image_new_from_file("black.png"); // create the black image
//    }
//    else {
//        image = gtk_image_new_from_file("white.png"); // create the white image
//    }

    iref = g_object_ref(image);//引用图像控件指针
    //查找按钮
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (GTK_TOGGLE_BUTTON(othello->buttons[i][j])->active == TRUE && area[i][j] == 2) {
//                printf("i:%d j:%d area:%d\n", i, j, area[i][j]);
//                setbutton(turn, i, j, othello);
                area[i][j] = turn;
//                all++;
//                printf("push button x(i):%d y(j):%d\n", i, j);

                // up direction
                nx = i;
                ny = j;
                if(nx - 1 >=0 && area[nx - 1][ny] != turn && area[nx - 1][ny] != 2) {
                    for(;;) {
                        nx--;
                        if(nx >= 0 && area[nx][ny] == 2) {
                            break;
                        }
                        if(nx < 0 || area[nx][ny] == turn) {
                            int k;
                            for(k=i; k>nx; k--) {
//                                setbutton(turn, k, ny, othello);
                                area[k][ny] = turn;
                            }
                            break;
                        }
                    }
                }
//                printf("up\n");

                // down direction
                nx = i;
                ny = j;
                if(nx + 1 <= 7 && area[nx + 1][ny] != turn && area[nx + 1][ny] != 2) {
                    for(;;) {
                        nx++;
                        if(nx<=7 && area[nx][ny] == 2) {
                            break;
                        }
                        if(nx > 7 || area[nx][ny] == turn) {
                            int k;
                            for(k=i; k<nx; k++) {
//                                setbutton(turn, k, ny, othello);
                                area[k][ny] = turn;
                            }
                            break;
                        }
                    }
                }
//                printf("down\n");

                // left direction
                nx = i;
                ny = j;
                if(ny - 1 >= 0 && area[nx][ny - 1] != turn && area[nx][ny - 1] != 2) {
                    for(;;) {
                        ny--;
                        if(ny >= 0 && area[nx][ny] == 2) {
                            break;
                        }
                        if(ny < 0 || area[nx][ny] == turn) {
                            int k;
                            for(k=j; k>ny; k--) {
//                                setbutton(turn, nx, k, othello);
                                area[nx][k] = turn;
                            }
                            break;
                        }
                    }
                }
//                printf("left\n");

                // right direction
                nx = i;
                ny = j;
                if(ny + 1 <= 7 && area[nx][ny + 1] != turn && area[nx][ny + 1] != 2) {
                    for(;;) {
                        ny++;
                        if(ny <=7 && area[nx][ny] == 2) {
                            break;
                        }
                        if(ny > 7 || area[nx][ny] == turn) {
                            int k;
                            for(k=j; k<ny; k++) {
//                                setbutton(turn, nx, k, othello);
                                area[nx][k] = turn;
                            }
                            break;
                        }
                    }
                }
//                printf("right\n");
                turn ^= 1;
            }
        }
    }
    rendermap(othello);

    // for(i=0;i<8;i++) {
    //     for(j=0;j<8;j++) {
    //         printf("%d ", area[i][j]);
    //     }
    //     printf("\n");
    // }
    encodefunc();
    // printf("please input:");
    // scanf("%s", &sendmsg);
    send(c_fd,sendmsg, sizeof(sendmsg),0);//回复消息
    
    
    if(recv(c_fd,recvmsg1,256,0)>0) //接收消息recv(c_fd,buf,256,0)>0
	{
        // recvmsg1[sizeof(recvmsg1)+1]='\0';
        decodefunc();
        printf("收到客户端消息:\n %s\n",recvmsg1);//输出到终端
      
    }
    rendermap(othello);
    // if this is client
    // send map
    // receive refreshed map
    // find difference
    // render the view

    
    if(all == 64) {
        g_signal_emit(G_OBJECT(othello), othello_signals[OTHELLO_SIGNAL], 0);
    }

}

//显示出错对话框
static void dialog_show(Othello *othello) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(
            NULL,
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "非常遗憾！\n您没有完成八皇后问题．继续努力吧！"
    );
    //
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    all = 0;
    //如超过八次则自动清除,发射othello信号,则执行othello_clear函数
    g_signal_emit(G_OBJECT(othello), othello_signals[OTHELLO_SIGNAL], 0);
}
