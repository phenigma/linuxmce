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

#include "mssndcard.h"

static MSSndCardManager *scm=NULL;

static MSSndCardManager * create_manager(){
	MSSndCardManager *obj=ms_new(MSSndCardManager,1);
	obj->cards=NULL;
	return obj;
}

MSSndCardManager * ms_snd_card_manager_get(void){
	if (scm==NULL) scm=create_manager();
	return scm;
}

MSSndCard * ms_snd_card_manager_get_card(MSSndCardManager *m, int index){
	int i;
	MSList *elem;
	for (i=0,elem=m->cards;elem!=NULL;i++,elem=elem->next){
		if (i==index) return (MSSndCard*)elem->data;
	}
	ms_warning("no card at index %i",index);
	return NULL;
}

MSList * ms_snd_card_manager_get_list(MSSndCardManager *m){
	return m->cards;
}

void ms_snd_card_manager_add_card(MSSndCardManager *m, MSSndCard *c){
	ms_message("Card %s/%s added",ms_snd_card_get_driver_type(c),ms_snd_card_get_name(c));
	m->cards=ms_list_append(m->cards,c);
}

void ms_snd_card_manager_register_desc(MSSndCardManager *m, MSSndCardDesc *desc){
	if (desc->detect!=NULL)
		desc->detect(m);
}


MSSndCard * ms_snd_card_new(MSSndCardDesc *desc){
	MSSndCard *obj=ms_new(MSSndCard,1);
	obj->desc=desc;
	obj->name=NULL;
	obj->data=NULL;
	if (desc->init!=NULL)
		desc->init(obj);
	return obj;
}

const char *ms_snd_card_get_driver_type(MSSndCard *obj){
	return obj->desc->driver_type;
}

const char *ms_snd_card_get_name(MSSndCard *obj){
	return obj->name;
}

void ms_snd_card_set_level(MSSndCard *obj, MSSndCardMixerElem e, int percent){
	if (obj->desc->set_level!=NULL)
		obj->desc->set_level(obj,e,percent);
	else ms_warning("unimplemented by %s wrapper",obj->desc->driver_type);
}

int ms_snd_card_get_level(MSSndCard *obj, MSSndCardMixerElem e){
	if (obj->desc->get_level!=NULL)
		return obj->desc->get_level(obj,e);
	else {
		ms_warning("unimplemented by %s wrapper",obj->desc->driver_type);
		return -1;
	}
}

void ms_snd_card_set_capture(MSSndCard *obj, MSSndCardCapture c){
	if (obj->desc->set_capture!=NULL)
		obj->desc->set_capture(obj,c);
	else ms_warning("unimplemented by %s wrapper",obj->desc->driver_type);
}

struct _MSFilter * ms_snd_card_create_reader(MSSndCard *obj){
	if (obj->desc->create_reader!=NULL)
		return obj->desc->create_reader(obj);
	else ms_warning("unimplemented by %s wrapper",obj->desc->driver_type);
	return NULL;
}

struct _MSFilter * ms_snd_card_create_writer(MSSndCard *obj){
	if (obj->desc->create_writer!=NULL)
		return obj->desc->create_writer(obj);
	else ms_warning("unimplemented by %s wrapper",obj->desc->driver_type);
	return NULL;
}
