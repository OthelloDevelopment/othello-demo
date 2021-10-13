#ifndef __OTHELLO_H__
#define __OTHELLO_H__

#include <gdk/gdk.h>
#include <gtk/gtkvbox.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
//三个转换宏
#define OTHELLO(obj) GTK_CHECK_CAST (obj, othello_get_type (), Othello)
#define OTHELLO_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, othello_get_type(), OthelloClass)
#define IS_OTHELLO(obj) GTK_CHECK_TYPE (obj, othello_get_type ())
//两个与控件相关的结构
typedef struct _Othello Othello;
typedef struct _OthelloClass OthelloClass;
struct _Othello {
    GtkVBox vbox;
    GtkWidget *msg;
    GtkWidget *buttons[8][8];//定义8x8按钮数组来表示棋盘
};

struct _OthelloClass {
    GtkVBoxClass parent_class;
    void (*othello)(Othello *othello);
};

//三个与控件相关的函数
GtkType othello_get_type(void); //取得此控件的类型
GtkWidget *othello_new(void); //创建八皇后控件
void othello_clear(Othello *othello); //清除棋盘上皇后
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __OTHELLO_H__ */
