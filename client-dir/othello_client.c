#include <gtk/gtk.h>
#include "othello_client.h"

char sendmsg2[256]; // the message to send
char recvmsg2[256];

enum {
    OTHELLO_SIGNAL,
    LAST_SIGNAL
};
// the map, 2 is null, 0 is white, 1 is black
static int area[8][8] = {{2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 1, 0, 2, 2, 2},
                         {2, 2, 2, 0, 1, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2},
                         {2, 2, 2, 2, 2, 2, 2, 2}};

// the sum of chess
static int all = 0;

// 1 is black, 0 is white
static int turn = 1;

// record x, y
int nx, ny;

// count the map, and compute who will win
int cnt = 0, black = 0, white = 0;

// record is reverse
int isreverse;

// record isdown
int isdown = 0;

// show the status and clear the board
static void dialog_show(Othello *othello);

static void othello_class_init(OthelloClass *klass);
static void othello_init(Othello *othello);
static void othello_toggle(GtkWidget *widget, Othello *othello);
static void send_toggle(GtkWidget *widget, Othello *othello);
static void recv_toggle(GtkWidget *widget, Othello *othello);

static gint othello_signals[LAST_SIGNAL] = {0}; // save the signal

GType othello_get_type(void) {
    static GType othello_type = 0;
    if (!othello_type) {
        static const GTypeInfo othello_info = {
                sizeof(OthelloClass),
                NULL, NULL,
                (GClassInitFunc) othello_class_init,
                NULL, NULL,
                sizeof(Othello), 0,
                (GInstanceInitFunc) othello_init,
        };
        othello_type = g_type_register_static(GTK_TYPE_VBOX, "Othello", &othello_info, 0);
    }
    return othello_type;
}

static void othello_class_init(OthelloClass *class) {
    GtkObjectClass *object_class;
    object_class = (GtkObjectClass *) class;
    // create the signal and named othello
    othello_signals[OTHELLO_SIGNAL] = g_signal_new(
            "Othello",
            G_TYPE_FROM_CLASS(object_class),
            G_SIGNAL_RUN_FIRST,
            0, NULL, NULL, g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE, 0, NULL
    );
    class->othello = NULL;
}

// init func
static void othello_init(Othello *othello) {
    GtkWidget *table;
    GdkColor col;
    gint i, j;
    // set the color
    col.red = 0;
    col.blue = 0;
    col.green = 0;
    // create the table container
    table = gtk_table_new(8, 9, TRUE);
    gtk_container_add(GTK_CONTAINER(othello), table);
    gtk_widget_show(table);
    // create the button
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            // generate the button
            othello->buttons[i][j] = gtk_toggle_button_new();
            // binding component
            gtk_widget_modify_bg(
                    GTK_WIDGET(othello->buttons[i][j]),
                    GTK_STATE_NORMAL, &col
            );
            // set the button
            gtk_table_attach_defaults(
                    GTK_TABLE(table),
                    othello->buttons[i][j],
                    i, i + 1, j, j + 1
            );
            // add the hook function
            g_signal_connect(
                    G_OBJECT(othello->buttons[i][j]),
                    "toggled",
                    G_CALLBACK(othello_toggle),
                    othello
            );
            // set the size
            gtk_widget_set_size_request(othello->buttons[i][j], 44, 44);
        }
    }

//     add send button and set the label
    othello->sendbutton = gtk_toggle_button_new();
    gtk_button_set_label(othello->sendbutton, "jmp");

    gtk_widget_modify_bg(
            GTK_WIDGET(othello->sendbutton),
            GTK_STATE_NORMAL, &col
    );
    // set the button
    gtk_table_attach_defaults(
            GTK_TABLE(table),
            othello->sendbutton,
            8, 8 + 1, 0, 0 + 1
    );
    // add the hook function
    g_signal_connect(
            G_OBJECT(othello->sendbutton),
            "toggled",
            G_CALLBACK(send_toggle),
            othello
    );

    othello->recvbutton = gtk_toggle_button_new();
    gtk_button_set_label(othello->recvbutton, "recv");
    gtk_widget_modify_bg(
            GTK_WIDGET(othello->recvbutton),
            GTK_STATE_NORMAL, &col
    );
    gtk_table_attach_defaults(
            GTK_TABLE(table),
            othello->recvbutton,
            8, 8 + 1, 1, 1 + 1
    );
    g_signal_connect(
            G_OBJECT(othello->recvbutton),
            "toggled",
            G_CALLBACK(recv_toggle),
            othello
    );

    gtk_widget_show_all(othello);
    rendermap(othello);
}

// send message
static void send_toggle(GtkWidget *widget, Othello *othello) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(othello->sendbutton), FALSE);
    if(isdown == 0) {
        printf("send\n");
        isdown = 1;
        // encode the map
        encodefunc();
        // use socket to send the message
        extern int sockfd;
        send(sockfd,sendmsg2,sizeof(sendmsg2),MSG_DONTWAIT);
    }
}

// receive message
static void recv_toggle(GtkWidget *widget, Othello *othello) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(othello->recvbutton), FALSE);
    printf("recv\n");
    // use socket to receive the message
    extern int sockfd;
    int flag = recv(sockfd,recvmsg2,256,MSG_DONTWAIT);
    // decode the string to a two-dimensional matrix
//    decodefunc();
//    int i,j;
//    for(i=0;i<8;i++) {
//        for(j=0;j<8;j++) {
//            printf("%d ", area[i][j]);
//        }
//        printf("\n");
//    }
//    printf("%s\n",recvmsg2);
    // render the map
    printf("flag = %d\n",flag);
    // render the map
    if(flag>0) {
        decodefunc();
        rendermap(othello);
        isdown = 0;
    }
//    rendermap(othello);
}

GtkWidget *othello_new(void) {
    return GTK_WIDGET(g_object_new(othello_get_type(), NULL));
}

// clear the image on the button
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
            area[i][j] = 2;
            all = 0;
        }
    }
}

// set button image, color the buttons
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
    if (child != NULL) {
        gtk_container_remove(GTK_CONTAINER(othello->buttons[x][y]), child);
    }
    // add the image
    gtk_container_add(GTK_CONTAINER(othello->buttons[x][y]), iref);
    // show the image
    gtk_widget_show(iref);
}

// encode func which make a two-dimensional matrix to a string
void encodefunc() {
    int i, j;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            switch(area[i][j]) {
                case 0: sendmsg2[i * 8 + j] = '0'; break;
                case 1: sendmsg2[i * 8 + j] = '1'; break;
                case 2: sendmsg2[i * 8 + j] = '2'; break;
            }
        }
    }
    sendmsg2[64]='\0';
}

// decode func which make a string to a two-dimensional matrix
void decodefunc() {
    int i, j;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            if(recvmsg2[i * 8 + j] == '0') {
                area[i][j] = 0;
            }
            else if(recvmsg2[i * 8 + j] == '1') {
                area[i][j] = 1;
            }
            else if(recvmsg2[i * 8 + j] == '2') {
                area[i][j] = 2;
            }
        }
    }
}

// render the map from area
void rendermap(Othello *othello) {
    int i, j;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            if(area[i][j] == 0 || area[i][j] == 1) {
                setbutton(area[i][j], i, j, othello);
            }
        }
    }
}

// the hook function that responds to the button click event
static void othello_toggle(GtkWidget *widget, Othello *othello) {
    int i, j;
    gboolean istrue = TRUE;
    GtkWidget *child;
    GtkWidget *dialog;

    // find which button is pushed
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if(GTK_TOGGLE_BUTTON(othello->buttons[i][j])->active == TRUE) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(othello->buttons[i][j]), FALSE);
                if (area[i][j] == 2 && isdown == 0) {

                    isreverse = 0;
                    //area[i][j] = turn;
                    all++;

                    // up direction
                    nx = i;
                    ny = j;
                    if(nx - 1 >=0 && area[nx - 1][ny] != turn && area[nx - 1][ny] != 2) {
                        for(;;) {
                            nx--;
                            if(nx >= 0 && area[nx][ny] == 2) {
                                break;
                            }
                            if(area[nx][ny] == turn) {
                                isreverse = 1;
                                int k;
                                for(k=i; k>nx; k--) {
                                    area[k][ny] = turn;
                                }
                                break;
                            }
                        }
                    }

                    // down direction
                    nx = i;
                    ny = j;
                    if(nx + 1 <= 7 && area[nx + 1][ny] != turn && area[nx + 1][ny] != 2) {
                        for(;;) {
                            nx++;
                            if(nx<=7 && area[nx][ny] == 2) {
                                break;
                            }
                            if(area[nx][ny] == turn) {
                                isreverse = 1;
                                int k;
                                for(k=i; k<nx; k++) {
                                    area[k][ny] = turn;
                                }
                                break;
                            }
                        }
                    }

                    // left direction
                    nx = i;
                    ny = j;
                    if(ny - 1 >= 0 && area[nx][ny - 1] != turn && area[nx][ny - 1] != 2) {
                        for(;;) {
                            ny--;
                            if(ny >= 0 && area[nx][ny] == 2) {
                                break;
                            }
                            if(area[nx][ny] == turn) {
                                isreverse = 1;
                                int k;
                                for(k=j; k>ny; k--) {
                                    area[nx][k] = turn;
                                }
                                break;
                            }
                        }
                    }

                    // right direction
                    nx = i;
                    ny = j;
                    if(ny + 1 <= 7 && area[nx][ny + 1] != turn && area[nx][ny + 1] != 2) {
                        for(;;) {
                            ny++;
                            if(ny <=7 && area[nx][ny] == 2) {
                                break;
                            }
                            if(area[nx][ny] == turn) {
                                isreverse = 1;
                                int k;
                                for(k=j; k<ny; k++) {
                                    area[nx][k] = turn;
                                }
                                break;
                            }
                        }
                    }


                    if(isreverse == 1) {
                        area[i][j] = turn;
                        isdown = 1;
                        encodefunc();
                        // use socket to send the message
                        extern int sockfd;
                        send(sockfd,sendmsg2,sizeof(sendmsg2),MSG_DONTWAIT);
                    }
                }
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

    white = 0;
    black = 0;
    cnt = 0;
    for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            if(area[i][j] == 2) {
                cnt++;
            }
            else if(area[i][j] == 0) {
                white++;
            }
            else {
                black++;
            }
        }
    }
    if(cnt == 0) {
        if(black>white) {
            dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        "恭喜！\n你赢了!!!!!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
        else if(black == white) {
            dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        "你们两是平局");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
        else {
            dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        "真遗憾\n下次再努努力吧~");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
        g_signal_emit(G_OBJECT(othello), othello_signals[OTHELLO_SIGNAL], 0);
    }
}
