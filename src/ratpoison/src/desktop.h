//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef DESKTOP_H
#define DESKTOP_H

extern rp_window *desktop_window;
extern int isKeyboardDisabled;

void clear_destkop_application();
void make_desktop_application(rp_window *);

int desktop_has_focus();

char *cmd_keystodesktop(int interactive, char *data);

#endif
