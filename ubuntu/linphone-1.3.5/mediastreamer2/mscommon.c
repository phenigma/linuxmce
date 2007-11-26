/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mscommon.h"
#include "msfilter.h"

#include "alldescs.h"
#include "mssndcard.h"

#include <sys/types.h>
#include <dirent.h>
#ifdef HAVE_DLOPEN
#include <dlfcn.h>
#endif


MSList *ms_list_new(void *data){
	MSList *new_elem=ms_new(MSList,1);
	new_elem->prev=new_elem->next=NULL;
	new_elem->data=data;
	return new_elem;
}

MSList * ms_list_append(MSList *elem, void * data){
	MSList *new_elem=ms_list_new(data);
	MSList *it=elem;
	if (elem==NULL) return new_elem;
	while (it->next!=NULL) it=ms_list_next(it);
	it->next=new_elem;
	new_elem->prev=it;
	return elem;
}

MSList * ms_list_concat(MSList *first, MSList *second){
	MSList *it=first;
	if (it==NULL) return second;
	while(it->next!=NULL) it=ms_list_next(it);
	it->next=second;
	second->prev=it;
	return first;
}

MSList * ms_list_free(MSList *list){
	MSList *elem = list;
	MSList *tmp;
	if (list==NULL) return NULL;
	while(elem->next!=NULL) {
		tmp = elem;
		elem = elem->next;
		ms_free(tmp);
	}
	ms_free(elem);
	return NULL;
}

MSList * ms_list_remove(MSList *first, void *data){
	MSList *it;
	if (first->data==data){
		MSList *newhead=first->next;
		if (newhead!=NULL){
			newhead->prev=NULL;
		}
		ms_free(first);
		return newhead;
	}
	for(it=first;it!=NULL;){
		MSList *next=it->next;
		if (it->data==data){
			it->prev->next=it->next;
			if (it->next!=NULL) it->next->prev=it->prev;
			ms_free(it);
			break;
		}
		it=next;
	}
	return first;
}

#define PLUGINS_EXT ".so"

typedef void (*init_func_t)(void);

void ms_load_plugins(const char *dir){
#ifdef HAVE_DLOPEN
	DIR *ds;
	struct dirent *de;
	char *fullpath;
	ds=opendir(dir);	
	if (ds==NULL){
		ms_message("Cannot open directory %s: %s",dir,strerror(errno));
		return;
	}
	while( (de=readdir(ds))!=NULL){
		if (de->d_type==DT_REG && strstr(de->d_name,PLUGINS_EXT)!=NULL){
			void *handle;
			fullpath=ms_strdup_printf("%s/%s",dir,de->d_name);
			ms_message("Loading plugin %s...",fullpath);
			
			if ( (handle=dlopen(fullpath,RTLD_NOW))==NULL){
				ms_warning("Fail to load plugin %s : %s",fullpath,dlerror());
			}else {
				char *initroutine_name=ms_malloc0(strlen(de->d_name)+10);
				char *p;
				void *initroutine;
				strcpy(initroutine_name,de->d_name);
				p=strstr(initroutine_name,PLUGINS_EXT);
				strcpy(p,"_init");
				initroutine=dlsym(handle,initroutine_name);
				if (initroutine!=NULL){
					init_func_t func=(init_func_t)initroutine;
					func();
					ms_message("Plugin loaded.");
				}else{
					ms_warning("Could not locate init routine of plugin %s",de->d_name);
				}
				ms_free(initroutine_name);
			}
			ms_free(fullpath);
		}
	}
	closedir(ds);
#else
	ms_warning("no loadable plugin support: plugins cannot be loaded.");
#endif
}


#ifdef __ALSA_ENABLED__
extern MSSndCardDesc alsa_card_desc;
#endif

#ifdef HAVE_SYS_SOUNDCARD_H
extern MSSndCardDesc oss_card_desc;
#endif

#ifdef __ARTS_ENABLED__
extern MSSndCardDesc arts_card_desc;
#endif

static MSSndCardDesc * ms_snd_card_descs[]={
#ifdef __ALSA_ENABLED__
	&alsa_card_desc,
#endif
#ifdef HAVE_SYS_SOUNDCARD_H
	&oss_card_desc,
#endif
#ifdef __ARTS_ENABLED__
	&arts_card_desc,
#endif
	NULL
};

void ms_init(){
	int i;
	MSSndCardManager *cm;
	ortp_set_log_level_mask(ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR|ORTP_FATAL);
	/* register builtin MSFilter's */
	for (i=0;ms_filter_descs[i]!=NULL;i++){
		ms_filter_register(ms_filter_descs[i]);
	}
	/*register SndCardDesc */
	cm=ms_snd_card_manager_get();
	for (i=0;ms_snd_card_descs[i]!=NULL;i++){
		ms_snd_card_manager_register_desc(cm,ms_snd_card_descs[i]);
	}
	ms_load_plugins(PACKAGE_PLUGINS_DIR);
}

void ms_exit(){
}
