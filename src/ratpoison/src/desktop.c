/*
*  C Implementation: %{$MODULE}
*
* Description:
*
*
* Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <string.h>
#include "ratpoison.h"

rp_window *desktop_window = NULL;
int isKeyboardDisabled = 0; // this one is needed since ratpoison has other paths for reenabling the keyboard.

static int isDesktopHasFocus = 0;

char *
cmd_desktop(int interactive, char *data)
{
    if ( data && strncmp(data, "off", 3) == 0 )
    {
        clear_destkop_application();
    }
    else
    {
        make_desktop_application(current_window());
        cmd_next(interactive, NULL);
    }

    return NULL;
}

char *
cmd_keybindings(int interactive, char *data)
{
    if ( data && strncmp(data, "off", 3) == 0)
    {
        ungrab_keys_all_wins ();
        isKeyboardDisabled = 1;
    }
    else
    {
        isKeyboardDisabled = 0;
        grab_keys_all_wins ();
    }

    return NULL;
}

char *
cmd_keystodesktop(int interactive, char *data)
{
    PRINT_DEBUG(("****************   Handling keys to destop.\n"));
    if ( data && strncmp(data, "off", 3) == 0)
    {
        PRINT_DEBUG(("****************   Removing keys from desktop.\n"));
        isDesktopHasFocus = 0;
        if ( current_window() != NULL )
        {
            set_rp_window_focus(current_window());
            grab_top_level_keys(current_window()->w);
        }
    }
    else
    {
        if ( desktop_window == NULL )
            return "There is no application set as desktop currently!";

        PRINT_DEBUG(("***************** setting focus to the destkop window.\n"));
        set_rp_window_focus(desktop_window);
        PRINT_DEBUG(("********** Grabbing top level keys.\n"));
        grab_top_level_keys(desktop_window->w);
        isDesktopHasFocus = 1;
    }

    return NULL;
}

void clear_destkop_application()
{
    rp_frame *frame;
    rp_screen *screen;
    rp_window *new_window;

    PRINT_DEBUG (("Clearing the desktop application!\n"));

    if ( desktop_window == NULL )
        return;

    frame = win_get_frame (desktop_window);
    screen = frames_screen(frame);

    new_window = add_to_window_list (screen, desktop_window->w);
    update_window_information (new_window);
    map_window(new_window);

    free_window(desktop_window);
    desktop_window = NULL;
}

void make_desktop_application(rp_window *window)
{
    if ( window == NULL )
    {
        PRINT_DEBUG(( "Can't set a null window as the desktop window"));
        return;
    }

    PRINT_DEBUG (( "Setting window %d (%s) as the current desktop application!\n", window->number, window->wm_name));
    clear_destkop_application();

    desktop_window = window;

    numset_release (rp_window_numset, window->number);

    list_del (&desktop_window->node);
    groups_unmap_window(desktop_window);
    groups_del_window (desktop_window);

    set_state(desktop_window, NormalState);
    move_window_as_desktop(desktop_window);

    XMoveResizeWindow (dpy,
                                desktop_window->w,
                                desktop_window->x,
                                desktop_window->y,
                                desktop_window->width,
                                desktop_window->height);

    XSetWindowBorderWidth (dpy, desktop_window->w, desktop_window->border);

    XSelectInput(dpy, desktop_window->w, WIN_EVENTS );
    unhide_window (desktop_window);
    XLowerWindow(dpy, desktop_window->w);
    XSync(dpy, False);
}

/**
        TODO: Move this in the proper place
    if ( win->wm_name && strcmp(win->wm_name, desktop_window_name) == 0 )
        {
            desktop_window = win;
            unmanage(desktop_window);
            XLowerWindow(dpy, desktop_window->w);
            XSelectInput(dpy, desktop_window->w, WIN_EVENTS );
            set_active_window(current_window());
        }
*/



/**
      PRINT_DEBUG ( ("New name %s, desktop name: %s\n", win->wm_name, desktop_window_name) );

      if ( strcmp(win->wm_name, desktop_window_name) == 0)
      {
        PRINT_DEBUG( ("This window will become the desktop window\n") );
        desktop_window = win;

        set_window_as_desktop(desktop_window);
        set_active_window(current_window());
        PRINT_DEBUG( ("It should be unamanged here!\n"));
      }
*/

/**
    else
    {
        // the window is a new window
        // look if it is the desktop window
        if ( desktop_window && ev->xproperty.window == desktop_window->w && ev->xproperty.atom == XA_WM_NAME )
        {
            update_window_name (desktop_window);
            update_window_names (desktop_window->scr);

            if ( strcmp(desktop_window->wm_name, desktop_window_name) == 0)
            {
                PRINT_DEBUG( ("The title for the desktop window has changed! We should manage it again!\n"));

                update_window_gravity (desktop_window);
                get_mouse_root_position (desktop_window, &desktop_window->mouse_x, &desktop_window->mouse_y);

                XSelectInput (dpy, desktop_window->w, WIN_EVENTS);

                list_add_tail (&desktop_window->node, &rp_unmapped_window);

                // Add the window to the current group.
                group_add_window (rp_current_group, desktop_window);

                // win = add_to_window_list (s, win->w);
                update_window_information (desktop_window);

                desktop_window = NULL;
            }
        }
    }
*/

int
move_window_as_desktop(rp_window *win)
{
    rp_frame *frame;
    rp_screen *screen;

    if (win->frame_number == EMPTY)
    {
        PRINT_DEBUG(("Empty frame!! ?? \n"));
        return 0;
    }

    frame = win_get_frame (win);
    screen = frames_screen(frame);

    win->x = win->y = 0;
    win->width = screen->width;
    win->height = screen->height;

	return 1;
}

int
desktop_has_focus()
{
    return isDesktopHasFocus;
}
