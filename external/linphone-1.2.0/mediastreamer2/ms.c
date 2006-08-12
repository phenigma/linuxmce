/*
  The mediastreamer library aims at providing modular media processing and I/O
	for linphone, but also for any telephony application.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org
  										
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ms.h"
#include "sndcard.h"
#include "mscodec.h"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * ms_init:
 *
 *
 * Initialize the mediastreamer. This must be the first function called in a program
 * using the mediastreamer library.
 *
 *
 */
void ms_init()
{
	if (!g_thread_supported()) g_thread_init (NULL);
#ifdef HAVE_GLIB
	if (!g_module_supported()){
		g_error("GModule is not supported.");
	}
#endif
	/* initialize the oss subsystem */
	snd_card_manager_init(snd_card_manager);
	/* register the statically linked codecs */
	ms_codec_register_all();
}






gint ms_load_plugin(gchar *path)
{
#ifdef HAVE_GLIB
	g_module_open(path,0);
#endif
	return 0;
}

gchar * ms_proc_get_param(gchar *parameter)
{
	gchar *file;
	int fd;
	int err,len;
	gchar *p,*begin,*end;
	gchar *ret;
	fd=open("/proc/cpuinfo",O_RDONLY);
	if (fd<0){
		g_warning("Could not open /proc/cpuinfo.");
		return NULL;
	}
	file=g_malloc(1024);
	err=read(fd,file,1024);
	file[err-1]='\0';
	/* find the parameter */
	p=strstr(file,parameter);
	if (p==NULL){
		/* parameter not found */
		g_free(file);
		return NULL;		
	}
	/* find the following ':' */
	p=strchr(p,':');
	if (p==NULL){
		g_free(file);
		return NULL;
	}
	/* find the value*/
	begin=p+2;
	end=strchr(begin,'\n');
	if (end==NULL) end=strchr(begin,'\0');
	len=end-begin+1;
	ret=g_malloc(len+1);
	snprintf(ret,len,"%s",begin);
	//printf("%s=%s\n",parameter,ret);
	g_free(file);
	return ret;
}

gint ms_proc_get_type()
{
	static int proc_type=0;
	gchar *value;
	if (proc_type==0){
		value=ms_proc_get_param("cpu family");
		if (value!=NULL) {
			proc_type=atoi(value);
			g_free(value);
		}else return -1;
	}
	return proc_type;
}

gint ms_proc_get_speed()
{
	char *value;
	static int proc_speed=0;
	if (proc_speed==0){
		value=ms_proc_get_param("cpu MHz");
		if (value!=NULL){
			proc_speed=atoi(value);
			g_free(value);
		}else return -1;
	}
	//printf("proc_speed=%i\n",proc_speed);
	return proc_speed;
}
