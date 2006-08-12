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


#ifndef MSFILTER_H
#define MSFILTER_H

#include <config.h>

#ifdef HAVE_GLIB
#include <glib.h>
#include <gmodule.h>
#else
#undef VERSION
#undef PACKAGE
#include <uglib.h>
#endif

#include <string.h>
#include "msutils.h"
#include "msfifo.h"
#include "msqueue.h"

struct _MSFilter;
struct _MSSync;
struct _MSLinkParams;
/*this is the abstract object and class for all filter types*/
typedef gint (*MSFilterNotifyFunc)(struct _MSFilter*, gint event, gpointer arg, gpointer userdata);
typedef struct _MSFilter * (*MSFilterNewFunc)(void);
typedef  void (*MSFilterProcessFunc)(struct _MSFilter *);
typedef  void (*MSFilterUninitFunc)(struct _MSFilter *);
typedef  int (*MSFilterPropertyFunc)(struct _MSFilter *,int ,void*);
typedef  void (*MSFilterNotifyStartFunc)(struct _MSFilter *, struct _MSSync *); 
typedef  void (*MSFilterNotifyStopFunc)(struct _MSFilter *);
typedef  void (*MSFilterSetupLinkFunc)(struct _MSFilter *, struct _MSLinkParams *  );

struct _MSFilterVTable{
	MSFilterUninitFunc destroy;
	MSFilterProcessFunc process;
	MSFilterSetupLinkFunc setup_link;
	MSFilterNotifyStartFunc notify_start;
	MSFilterNotifyStopFunc notify_stop;
	MSFilterPropertyFunc set_property;
	MSFilterPropertyFunc get_property;
};

typedef struct _MSFilterVTable MSFilterVTable;

struct _MSFilter
{
	MSFilterVTable *vtable;
	GMutex *lock;
	gchar *name;
	guchar max_finputs;   /* maximum number of fifo inputs*/
	guchar max_foutputs;  /* maximum number of fifo outputs*/
	guchar max_qinputs;   /* maximum number of queue inputs*/
	guchar max_qoutputs;  /* maximum number of queue outputs*/
	guchar finputs;   /* number of connected fifo inputs*/
	guchar foutputs;  /* number of connected fifo outputs*/
	guchar qinputs;   /* number of connected queue inputs*/
	guchar qoutputs;  /* number of connected queue outputs*/
	gint min_fifo_size; /* set when linking*/
	gint r_mingran;				/* read minimum granularity (for fifos).
					It can be zero so that the filter can accept any size of reading data*/
	gint exec_id;
	guint last_tick;
	MSFifo **infifos; /*pointer to a table of pointer to input fifos*/
	MSFifo **outfifos;  /*pointer to a table of pointer to output fifos*/
	MSQueue **inqueues;  /*pointer to a table of pointer to input queues*/
	MSQueue **outqueues;  /*pointer to a table of pointer to output queues*/
	MSFilterNotifyFunc notify_event;
	struct _MSSync *sync;
	gpointer userdata;
	gboolean running;
};


/* public API :*/

typedef struct _MSFilter MSFilter;

typedef enum{
	MS_FILTER_PROPERTY_FREQ,	/* value is int */
	MS_FILTER_PROPERTY_BITRATE, /*value is int */
	MS_FILTER_PROPERTY_CHANNELS,
	MS_FILTER_PROPERTY_FMTP    /* value is string */
}MSFilterProperty;

#define MS_FILTER(obj) ((MSFilter*)obj)
typedef enum _MSLinkType {
	LINK_FIFO,
	LINK_QUEUE
} MSLinkType;
void ms_filter_set_notify_func(MSFilter* filter,MSFilterNotifyFunc func, gpointer userdata);
void ms_filter_notify_event(MSFilter *filter,gint event, gpointer arg);
int ms_filter_set_property(MSFilter *f,MSFilterProperty property, void *value);
int ms_filter_get_property(MSFilter *f,MSFilterProperty property, void *value);
/* low level connect functions */
int ms_filter_link(MSFilter *m1, gint pin1, MSFilter *m2,gint pin2, MSLinkType linktype);
int ms_filter_unlink(MSFilter *m1, gint pin1, MSFilter *m2,gint pin2,MSLinkType linktype);
/* high level connect functions */
int ms_filter_add_link(MSFilter *m1, MSFilter *m2);
int ms_filter_remove_links(MSFilter *m1, MSFilter *m2);
void ms_filter_destroy(MSFilter *f);

#define declare_simple_static_fifo_setup(name,ingran, outgran) \
static void name##_simple_static_fifo_setup(MSFilter *f, MSLinkParams *p) \
{\
	switch(p->dir){\
		case MS_LINK_INPUT:\
			p->maxreadgran=ingran; \
			break;\
		case MS_LINK_OUTPUT:\
			p->maxwritegran=outgran; \
			break;\
	}\
}

#define simple_static_fifo_setup_ref(name)	name##_simple_static_fifo_setup

/* protected */
void ms_filter_init(MSFilter *f, MSFilterVTable *vtable, gchar *name, guchar finputs, guchar qinputs, guchar foutputs, guchar qoutputs);
#define  ms_filter_process(filter) ((filter)->vtable->process((filter)))
#define ms_filter_lock(filter)		g_mutex_lock((filter)->lock)
#define ms_filter_unlock(filter)	g_mutex_unlock((filter)->lock)
#define ms_filter_get_mingran(f) ((f)->r_mingran)
#define ms_filter_set_mingran(f,gran) ((f)->r_mingran=(gran))

/*private*/
gint ms_filter_fifos_have_data(MSFilter *f);
gint ms_filter_queues_have_data(MSFilter *f);
void ms_filter_setup_links(MSFilter *f, struct _MSSync *s);
void ms_filter_unsetup_links(MSFilter *f);
#define ms_filter_is_source(f) ((f)->max_finputs==0  && (f)->max_qinputs==0)
void ms_filter_uninit(MSFilter *obj);

#define MSFILTER_VERSION(a,b,c) (((a)<<2)|((b)<<1)|(c))

enum _MSFilterType
{
	MS_FILTER_DISK_IO,
	MS_FILTER_AUDIO_CODEC,
	MS_FILTER_VIDEO_CODEC,
	MS_FILTER_NET_IO,
	MS_FILTER_VIDEO_IO,
	MS_FILTER_AUDIO_IO,
	MS_FILTER_OTHER
};

typedef enum _MSFilterType MSFilterType;


/* find the first codec in the left part of the stream */
MSFilter * ms_filter_search_upstream_by_type(MSFilter *f,MSFilterType type);

struct _MSFilterInfo
{
	gchar *name;
	gint version;
	MSFilterType type;
	MSFilterNewFunc constructor;
	char *description;  /*some textual information*/
};

typedef struct _MSFilterInfo MSFilterInfo;

void ms_filter_register(MSFilterInfo *finfo);
void ms_filter_unregister(MSFilterInfo *finfo);
MSFilterInfo * ms_filter_get_by_name(gchar *name);

MSFilter * ms_filter_new_with_name(gchar *name);



extern GList *filter_list;
#define MS_FILTER_INFO(obj)	((MSFilterInfo*)obj)

void swap_buffer(gchar *buffer, gint len);


#endif
