icewm.cfg is the pluto-config-directory for icewm
~/.icewm is the one known by icewm

Copy ( or make a symbolic link to ) it in the user's $HOME directory
i.e.: cp -a icewm.cfg ~/.icewm

Icewm-session will launch the window manager and all programs needed by
Icewm. It should be started after X11, instead of icewm.

If one wants to do some debugging, like showing the taskbar or the
window border, go to the end of ~/.icewm/preferences and uncomment those
lines. Press Ctrl-Alt-Shift-Del and R (Restart)
