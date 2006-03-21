/* communications.h -- Send commands to a running copy of ratpoison.
 * Copyright (C) 2000, 2001, 2002, 2003 Shawn Betts
 *
 * This file is part of ratpoison.
 *
 * ratpoison is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * ratpoison is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307 USA
 */

#ifndef _RATPOISON_COMMUNICATIONS_H
#define _RATPOISON_COMMUNICATIONS_H 1

int send_command (unsigned char interactive, unsigned char *cmd, int screen_num);

/* open the log-file */
FILE * log_file_open();

/* close the log-file */
void log_file_close();

/* log string with time-stamp */
void log_string(const char *str);

/* log message and command with time-stamp */
void log_message_command(const char *sMessage, const char *cmd/*, unsigned char interactive, int screen_num*/);

void log_message_command_args(const char *sMessage, const char *cmd, const char *args);
void log_message_command_result(const char *sMessage, const char *cmd, const char *args, const char *result);

#endif /* ! _RATPOISON_COMMUNICATIONS_H */
