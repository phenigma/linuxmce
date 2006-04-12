#ifndef _WMCTRL_H_
#define _WMCTRL_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include <glib.h>

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

#define MAX_PROPERTY_VALUE_LEN 4096
#define SELECT_WINDOW_MAGIC ":SELECT:"
#define ACTIVE_WINDOW_MAGIC ":ACTIVE:"

#define p_verbose(...) fprintf(stderr, __VA_ARGS__);

gboolean wm_supports (Display *disp, const gchar *prop);
Window *get_client_list (Display *disp, unsigned long *size);
int client_msg(Display *disp, Window win, char *msg,
        unsigned long data0, unsigned long data1,
        unsigned long data2, unsigned long data3,
        unsigned long data4);
int list_windows (Display *disp, bool show_pid, bool show_geometry, bool show_class);
int list_desktops (Display *disp,  bool wa_desktop_titles_invalid_utf8);
int showing_desktop (Display *disp, char *param);
int change_viewport (Display *disp, char *param);
int change_geometry (Display *disp, char *param);
int change_number_of_desktops (Display *disp, char *param);
int switch_desktop (Display *disp, char *param);
int wm_info (Display *disp);
gchar *get_output_str (gchar *str, gboolean is_utf8);
int action_window (Display *disp, Window win, char mode, char *param);
int action_window_pid (Display *disp, char mode, char *param_window, char *param);
int action_window_str (Display *disp, char mode, char *param_window, char *param, bool show_class, bool full_window_title_match);
int activate_window (Display *disp, Window win,
        gboolean switch_desktop);
int close_window (Display *disp, Window win);
int longest_str (gchar **strv);
int window_to_desktop (Display *disp, Window win, int desktop);
void window_set_title (Display *disp, Window win, char *str, char mode);
gchar *get_window_title (Display *disp, Window win);
gchar *get_window_class (Display *disp, Window win);
gchar *get_property (Display *disp, Window win,
        Atom xa_prop_type, gchar *prop_name, unsigned long *size);
void init_charset(bool force_utf8);
int window_move_resize (Display *disp, Window win, char *arg);
int window_state (Display *disp, Window win, char *arg);
Window Select_Window(Display *dpy);
Window get_active_window(Display *dpy);

#endif

