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


/*

aaronbaalbergen: can I ask you a question?  Where can we read about what those 'desktop on' and 'desktop off' commands are supposed to be doing and how to use them?  For some reason, without anybody making changes as far as I can tell, when we launch myth it's no longer selected.  It is launched, and it is running.  It's just the window isn't selected.  I ran the ratpoison command "select mythfrontend" from a ssh shell, and it does not select it there either.  But just through experimenting I found that if I do 'desktop off', then do 'select', then 'desktop on', then it does select myth.  But the app never gets keyboard or mouse.  And I can't find any docs.  Can you tell me where we need to go to see how this is supposed to work?

mtoader : desktop on = make the current selected window be treated as the desktop  window (aka make it will screen and removed from the internal list of windows managed by ratopison ) 

mtoader : desktop off = undo the desktop on (aka if there is a window set as desktop then unset it and add to the internal list of managed windows and at the same time resize to the actual desktop size as ratpoison sees it ).

aaronbaalbergen: so that's why sometimes SDL_App shows up when you do a ratpoison windows, and sometimes it doesn't.  When it's not showing up, it's because it's the desktop, right?  But then why when I see I do:
./ratpoison -d :0 -c windows
and I *do* see myth in the window list, why doesn't select X work, unless I first do a desktop off?

mtoader : when SDL_App shows in the list of managed windows it means it isn't a desktop window

aaronbaalbergen: I assume I should never do a desktop off, just leaving Orbiter (SDL_App) as the desktop.  But for some reason, select does not work without first doing a desktop off, and then a desktop on.  which I guess means by doing that I'm making Myth the desktop.

mtoader : if SDL_App would be teh desktop window than it shouldn't be in the list of managed windows..

mtoader : yes

mtoader : i don;t know why select doesn't work 

mtoader : select is the internal command in ratpoison 

mtoader : i didn't played with ti .. 

aaronbaalbergen: OK, so bottom line is i should never touch 'desktop off/on'.  Only do a select.

mtoader : 

mtoader : yes

aaronbaalbergen: And somehow ratpoison is broken that select doesn't work.

aaronbaalbergen: and your keyboard/mouse sharing code, is that also in desktop.c?

aaronbaalbergen: Or that's someplace else?

mtoader : Orbiter is doing the desktop on when it starts up 

mtoader : that's inside the orbiter 

aaronbaalbergen: OK, it's a bit clearer then.  Thx for the help.  

mtoader : becasue  the orbiter is the observer of the events in this case

mtoader : np-

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
