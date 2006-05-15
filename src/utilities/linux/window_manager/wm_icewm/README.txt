//
// Author : C Remus
//
// Changed by : ...
//

== Icewm notes ==
Almost all user interaction was disabled.

We use only 2 desktops:
    desktop 1 is the normal desktop, and should be always selected
    desktop 2 is the desktop used for hidding windows that should not appear

Icewm supports 7 layers.
We use only 3 layers for windows:
    OnTop, Normal, Below.
If 2 windows are on the same layer, then the last activeted one is over the other.


=== Keys for testing and debugging ===

;Ctrl+Alt+Shift+Pointer_Button1
    hold key and drag to move the window under cursor

;Ctrl+Alt+Shift+Pointer_Button3
    hold key and drag to resize the window under cursor

;Alt+Ctrl+Shift+Del
    press R to restart icewm

    If the icewm configuration is changed, icewm need to be restarted.
    It will keep the windows on the same position and the same desktop.

;Alt+Ctrl+Shift+Esc
    show the window list

    In the window list, arrow keys allow to select a window, Enter to
    activate that window.
    Activating a window will activate it's desktop, too.



=== Debugging and testing ===

If one wants to do some debugging, like showing the taskbar or the
window border, go to the end of ~/.icewm/preferences and uncomment those
lines.
    Press Ctrl-Alt-Shift-Del and R (Restart)
    to load the new configuration in Icewm



=== Icewm Taskbar ===

After the taskbar is activated in ~/.icewm/preferences:

- the lower-right button (>) will hide it

- the lower-right button (<) will show it again

- the taskbar is on the "OnTop" layer, so it may be below other windows

- right-click on a task will bring up a menu, used to control that app

- some items in the task menu are marked with a symbol

-- this mark show the current application status

- some items in the task-menu are deactivated

-- because the application does not support that action

-- or because that action was deactivated in icewm config files

-- or because that action is not available right now (i.e. cannot be maximized twice)



=== Configuration ===

icewm.cfg is the pluto-config-directory for icewm
~/.icewm is the one known by icewm

Copy ( or make a symbolic link to ) it in the user's $HOME directory
i.e.: cp -a icewm.cfg ~/.icewm

Icewm-session will launch the window manager and all programs needed by Icewm.
It should be started after X11, instead of icewm.



EOF
