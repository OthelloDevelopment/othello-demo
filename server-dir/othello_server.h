#ifndef __OTHELLO_H__
#define __OTHELLO_H__

#include <gdk/gdk.h>
#include <gtk/gtkvbox.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OTHELLO(obj) GTK_CHECK_CAST (obj, othello_get_type (), Othello)
#define OTHELLO_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, othello_get_type(), OthelloClass)
#define IS_OTHELLO(obj) GTK_CHECK_TYPE (obj, othello_get_type ())

typedef struct _Othello Othello;
typedef struct _OthelloClass OthelloClass;
struct _Othello {
    GtkVBox vbox;
    GtkWidget *msg;
    // the map
    GtkWidget *buttons[8][8];
    // send button
    GtkWidget *sendbutton;
    // receive button
    GtkWidget *recvbutton;
};

struct _OthelloClass {
    GtkVBoxClass parent_class;
    void (*othello)(Othello *othello);
};

GtkType othello_get_type(void);
GtkWidget *othello_new(void);
void othello_clear(Othello *othello);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __OTHELLO_H__ */
