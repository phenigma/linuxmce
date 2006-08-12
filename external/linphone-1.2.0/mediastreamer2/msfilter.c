/***************************************************************************
 *            msfilter.c
 *
 *  Fri Oct 15 16:13:08 2004
 *  Copyright  2004  Simon Morlat 
 *  Email <simon.morlat@linphone.org>
 ****************************************************************************/
/* The mediastreamer library creates and run multimedia streams in a modular
	way.
*/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <errno.h>
#include "msfilter.h"



void ms_filter_init(MSFilter *filter, MSFilterVTable *vtable, gchar *name, guchar finputs, guchar qinputs, guchar foutputs, guchar qoutputs)
{
	filter->vtable=vtable;
	if (vtable->setup_link==NULL && (finputs>0 || foutputs>0)){
		g_error("Filter %s has no setup_link function defined.",name); 
	}
	filter->finputs=0;
	filter->foutputs=0;
	filter->qinputs=0;
	filter->qoutputs=0;
	filter->max_finputs=finputs;
	filter->max_foutputs=foutputs;
	filter->max_qinputs=qinputs;
	filter->max_qoutputs=qoutputs;
	filter->infifos=(finputs>0) ? g_malloc0(finputs*sizeof(MSFifo*)) : NULL;
	filter->outfifos=(foutputs>0) ? g_malloc0(foutputs*sizeof(MSFifo*)) : NULL;
	filter->inqueues=(qinputs>0) ? g_malloc0(qinputs*sizeof(MSQueue*)) : NULL;
	filter->outqueues=(qoutputs>0) ? g_malloc0(qoutputs*sizeof(MSQueue*)) : NULL;
	filter->lock=g_mutex_new();
	filter->min_fifo_size=0x7fff;
	filter->notify_event=NULL;
	filter->userdata=NULL;
	filter->exec_id=0;
	filter->last_tick=0;
	filter->name=name;
	filter->running=FALSE;
}

void ms_filter_uninit(MSFilter *filter)
{
	g_mutex_free(filter->lock);
}


/* find output queue */
gint find_oq(MSFilter *m1,MSQueue *oq)
{
	gint i;
	
	for (i=0;i<m1->max_qoutputs;i++){
			if (m1->outqueues[i]==oq) return i;
	}
	
	return -1;
}

/* find input queue */
gint find_iq(MSFilter *m1,MSQueue *iq)
{
	gint i;
	for (i=0;i<m1->max_qinputs;i++){
		if (m1->inqueues[i]==iq) return i;
	}
	return -1;
}

/* find output fifo */
gint find_of(MSFilter *m1,MSFifo *of)
{
	gint i;
	for (i=0;i<m1->max_foutputs;i++){
		if (m1->outfifos[i]==of) return i;
	}
	
	return -1;
}

/* find input fifo */
gint find_if(MSFilter *m1,MSFifo *inf)
{
	gint i;
	
	for (i=0;i<m1->max_finputs;i++){
		if (m1->infifos[i]==inf) return i;
	}
	
	return -1;
}

#define find_free_iq(_m1)	find_iq(_m1,NULL)
#define find_free_oq(_m1)	find_oq(_m1,NULL)
#define find_free_if(_m1)	find_if(_m1,NULL)
#define find_free_of(_m1)	find_of(_m1,NULL)

int ms_filter_add_link(MSFilter *m1, MSFilter *m2)
{
	gint m1_q=-1;
	gint m1_f=-1;
	gint m2_q=-1;
	gint m2_f=-1;
	/* determine the type of link we can add */
	m1_q=find_free_oq(m1);
	m1_f=find_free_of(m1);
	m2_q=find_free_iq(m2);
	m2_f=find_free_if(m2);
	if ((m1_q!=-1) && (m2_q!=-1)){
		/* link with queues */
		ms_trace("m1_q=%i , m2_q=%i",m1_q,m2_q);
		return ms_filter_link(m1,m1_q,m2,m2_q,LINK_QUEUE);
	}
	if ((m1_f!=-1) && (m2_f!=-1)){
		/* link with queues */
		ms_trace("m1_f=%i , m2_f=%i",m1_f,m2_f);
		return ms_filter_link(m1,m1_f,m2,m2_f,LINK_FIFO);
	}
	g_warning("ms_filter_add_link: could not link.");
	return -1;
}
/**
 * ms_filter_link:
 * @m1:  A #MSFilter object.
 * @pin1:  The pin number on @m1.
 * @m2:  A #MSFilter object.
 * @pin2: The pin number on @m2.
 * @linktype: Type of connection, it may be #LINK_QUEUE, #LINK_FIFOS.
 *
 * This function links two MSFilter object between them. It must be used to make chains of filters.
 * All data outgoing from pin1 of m1 will go to the input pin2 of m2.
 * The way to communicate can be fifos or queues, depending of the nature of the filters. Filters can have
 * multiple queue pins and multiple fifo pins, but most of them have only one queue input/output or only one
 * fifo input/output. Fifos are usally used by filters doing audio processing, while queues are used by filters doing
 * video processing.
 *
 * Returns: 0 if successfull, a negative value reprensenting the errno.h error.
 */
int ms_filter_link(MSFilter *m1, gint pin1, MSFilter *m2,gint pin2, MSLinkType linktype)
{
	MSQueue *q;
	MSFifo *fifo;
	
	g_message("ms_filter_add_link: %s,%i -> %s,%i",m1->name,pin1,m2->name,pin2);
	switch(linktype)
	{
	case LINK_QUEUE:
		/* Are filter m1 and m2 able to accept more queues connections ?*/
		g_return_val_if_fail(m1->qoutputs<m1->max_qoutputs,-EMLINK);
		g_return_val_if_fail(m2->qinputs<m2->max_qinputs,-EMLINK);
		/* Are filter m1 and m2 valid with their inputs and outputs ?*/
		g_return_val_if_fail(m1->outqueues!=NULL,-EFAULT);
		g_return_val_if_fail(m2->inqueues!=NULL,-EFAULT);
		/* are the requested pins exists ?*/
		g_return_val_if_fail(pin1<m1->max_qoutputs,-EINVAL);
		g_return_val_if_fail(pin2<m2->max_qinputs,-EINVAL);
		/* are the requested pins free ?*/
		g_return_val_if_fail(m1->outqueues[pin1]==NULL,-EBUSY);
		g_return_val_if_fail(m2->inqueues[pin2]==NULL,-EBUSY);
		
		q=ms_queue_new(m1,pin1,m2,pin2);
		m1->outqueues[pin1]=m2->inqueues[pin2]=q;
		m1->qoutputs++;
		m2->qinputs++;
		break;
	case LINK_FIFO:
		/* Are filter m1 and m2 able to accept more fifo connections ?*/
		g_return_val_if_fail(m1->foutputs<m1->max_foutputs,-EMLINK);
		g_return_val_if_fail(m2->finputs<m2->max_finputs,-EMLINK);
		/* Are filter m1 and m2 valid with their inputs and outputs ?*/
		g_return_val_if_fail(m1->outfifos!=NULL,-EFAULT);
		g_return_val_if_fail(m2->infifos!=NULL,-EFAULT);
		/* are the requested pins exists ?*/
		g_return_val_if_fail(pin1<m1->max_foutputs,-EINVAL);
		g_return_val_if_fail(pin2<m2->max_finputs,-EINVAL);
		/* are the requested pins free ?*/
		g_return_val_if_fail(m1->outfifos[pin1]==NULL,-EBUSY);
		g_return_val_if_fail(m2->infifos[pin2]==NULL,-EBUSY);
		
		fifo=ms_fifo_new(m1,pin1,m2,pin2);
		
		m1->outfifos[pin1]=m2->infifos[pin2]=fifo;						
		m1->foutputs++;
		m2->finputs++;
		break;
	}
	return 0;
}
/**
 * ms_filter_unlink:
 * @m1:  A #MSFilter object.
 * @pin1:  The pin number on @m1.
 * @m2:  A #MSFilter object.
 * @pin2: The pin number on @m2.
 * @linktype: Type of connection, it may be #LINK_QUEUE, #LINK_FIFOS.
 *
 * Unlink @pin1 of filter @m1 from @pin2 of filter @m2. @linktype specifies what type of connection is removed.
 *
 * Returns: 0 if successfull, a negative value reprensenting the errno.h error.
 */
int ms_filter_unlink(MSFilter *m1, gint pin1, MSFilter *m2,gint pin2,MSLinkType linktype)
{
	switch(linktype)
	{
	case LINK_QUEUE:
		/* Are filter m1 and m2 valid with their inputs and outputs ?*/
		g_return_val_if_fail(m1->outqueues!=NULL,-EFAULT);
		g_return_val_if_fail(m2->inqueues!=NULL,-EFAULT);
		/* are the requested pins exists ?*/
		g_return_val_if_fail(pin1<m1->max_qoutputs,-EINVAL);
		g_return_val_if_fail(pin2<m2->max_qinputs,-EINVAL);
		/* are the requested pins busy ?*/
		g_return_val_if_fail(m1->outqueues[pin1]!=NULL,-ENOENT);
		g_return_val_if_fail(m2->inqueues[pin2]!=NULL,-ENOENT);
		/* are the two pins connected together ?*/
		g_return_val_if_fail(m1->outqueues[pin1]==m2->inqueues[pin2],-EINVAL);
		
		ms_queue_destroy(m1->outqueues[pin1]);
		m1->outqueues[pin1]=m2->inqueues[pin2]=NULL;
		m1->qoutputs--;
		m2->qinputs--;
		
		break;
	case LINK_FIFO:
		/* Are filter m1 and m2 valid with their inputs and outputs ?*/
		g_return_val_if_fail(m1->outfifos!=NULL,-EFAULT);
		g_return_val_if_fail(m2->infifos!=NULL,-EFAULT);
		/* are the requested pins exists ?*/
		g_return_val_if_fail(pin1<m1->max_foutputs,-EINVAL);
		g_return_val_if_fail(pin2<m2->max_finputs,-EINVAL);
		/* are the requested pins busy ?*/
		g_return_val_if_fail(m1->outfifos[pin1]!=NULL,-ENOENT);
		g_return_val_if_fail(m2->infifos[pin2]!=NULL,-ENOENT);
		/* are the two pins connected together ?*/
		g_return_val_if_fail(m1->outfifos[pin1]==m2->infifos[pin2],-EINVAL);
		ms_fifo_destroy(m1->outfifos[pin1]);
		m1->outfifos[pin1]=m2->infifos[pin2]=NULL;			
		m1->foutputs--;
		m2->finputs--;								
		break;
	}
	return 0;
}

/**
 *ms_filter_remove_links:
 *@m1: a filter
 *@m2: another filter.
 *
 * Removes all links between m1 and m2.
 *
 *Returns: 0 if at least one link has been removed, -1 if not.
**/
gint ms_filter_remove_links(MSFilter *m1, MSFilter *m2)
{
	int i,j;
	int removed=-1;
	MSLink *link;
	/* takes all outputs of m1, and removes the one that goes to m2 */
	if (m1->outqueues!=NULL){
		for (i=0;i<m1->max_qoutputs;i++)
		{
			link=(MSLink*)m1->outqueues[i];
			if (link!=NULL){
				MSFilter *rmf;
				/* test if the queue connects to m2 */
				rmf=link->next;
				if (rmf==m2){
					j=find_iq(rmf,(MSQueue*)link);
					if (j==-1) g_error("Could not find input queue: impossible case.");
					ms_filter_unlink(m1,i,m2,j,LINK_QUEUE);
					removed=0;
				}
			}
		}
	}
	if (m1->outfifos!=NULL){
		for (i=0;i<m1->max_foutputs;i++)
		{
			link=(MSLink*)m1->outfifos[i];
			if (link!=NULL){
				MSFilter *rmf;
				/* test if the queue connects to m2 */
				rmf=(MSFilter*)link->next;
				if (rmf==m2){
					j=find_if(rmf,(MSFifo*)link);
					if (j==-1) g_error("Could not find input fifo: impossible case.");
					ms_filter_unlink(m1,i,m2,j,LINK_FIFO);
					removed=0;
				}
			}
		}
	}
	return removed;
}

/**
 * ms_filter_fifos_have_data:
 * @f: a #MSFilter object.
 *
 * Tells if the filter has enough data in its input fifos in order to be executed succesfully.
 *
 * Returns: 1 if it can be executed, 0 else.
 */
gint ms_filter_fifos_have_data(MSFilter *f)
{
	gint i,j;
	gint val=0;
	gint max_inputs=f->max_finputs;
	gint con_inputs=f->finputs;
	MSFifo *fifo;
	/* test queues */
	for(i=0,j=0; (i<max_inputs) && (j<con_inputs);i++)
	{
		fifo=f->infifos[i];
		if (fifo!=NULL)
		{
			j++;
			val=1;
    	if (fifo->readsize==0) return 0;
			if (fifo->readsize<f->r_mingran) return 0;
		}
	}
	//printf("filter has data!\n");
	return 1;  /*all fifos have something to read that is enough for that filter*/
}

/**
 * ms_filter_queues_have_data:
 * @f: a #MSFilter object.
 *
 * Tells if the filter has enough data in its input queues in order to be executed succesfully.
 *
 * Returns: 1 if it can be executed, 0 else.
 */
gint ms_filter_queues_have_data(MSFilter *f)
{
	gint i,j,val=0;
	gint max_inputs=f->max_qinputs;
	gint con_inputs=f->qinputs;
	MSQueue *q;
	/* test queues */
	for(i=0,j=0; (i<max_inputs) && (j<con_inputs);i++)
	{
		q=f->inqueues[i];
		if (q!=NULL)
		{
			j++;
			val=1;
    	if (!ms_queue_can_get(q)) return 0;
		}
	}
	return 1;  /*all queues have something to read that is enough for that filter*/
}



void ms_filter_destroy(MSFilter *f)
{
	/* first check if the filter is disconnected from any others */
	g_return_if_fail(f->finputs==0);
	g_return_if_fail(f->foutputs==0);
	g_return_if_fail(f->qinputs==0);
	g_return_if_fail(f->qoutputs==0);
	f->vtable->destroy(f);
	g_mutex_free(f->lock);
	if (f->infifos!=NULL) g_free(f->infifos);
	if (f->inqueues!=NULL) g_free(f->inqueues);
	if (f->outfifos!=NULL) g_free(f->outfifos);
	if (f->outqueues!=NULL) g_free(f->outqueues);
	g_free(f);
}

static int max_bpt(MSFilter *f){
	int i;
	int maxbpt=0;
	for(i=0;i<f->max_finputs;i++){
		MSFifo *fifo=f->infifos[i];
		if (fifo!=NULL) maxbpt=MAX(maxbpt,fifo->bpt);
	}
	return maxbpt;
}


static void setup_fifo(MSFifo *fifo){
	MSLinkParams params;
	int bpt;
	memset(&params,0,sizeof(MSLinkParams));
	params.dir=MS_LINK_OUTPUT;
	params.dir=MS_LINK_TYPE_FIFO;
	params.pin=fifo->link.prevpin;
	fifo->link.prev->vtable->setup_link(fifo->link.prev,&params);
	if (params.maxwritegran<=0 || params.maxwriteoff<0 ) g_error("Bad fifo parameters !");
	params.dir=MS_LINK_INPUT;
	params.pin=fifo->link.nextpin;
	fifo->link.next->vtable->setup_link(fifo->link.next,&params);
	if (params.maxreadgran<=0 || params.maxreadoff<0 ) g_error("Bad fifo parameters !");
	if (ms_filter_is_source(fifo->link.prev))
		bpt=params.maxwritegran;
	else{
		bpt=max_bpt(fifo->link.prev);
	}
	ms_fifo_parametrize(fifo,params.maxreadgran,params.maxwritegran,params.maxreadoff,params.maxwriteoff,bpt);
}
static void setup_queue(MSQueue *q){};

void ms_filter_setup_links(MSFilter *f, struct _MSSync *s){
	int i;
	for(i=0;i<f->max_foutputs;i++){
		if (f->outfifos[i]!=0) setup_fifo(f->outfifos[i]);
	}
	for(i=0;i<f->max_qoutputs;i++){
		if (f->outqueues[i]!=0) setup_queue(f->outqueues[i]);
	}
	f->sync=s;
	if (f->vtable->notify_start!=NULL)
		f->vtable->notify_start(f,s);
}

void ms_filter_unsetup_links(MSFilter *f){
	if (f->vtable->notify_stop!=NULL)
		f->vtable->notify_stop(f);
	f->sync=NULL;
}

GList *filter_list=NULL;

void ms_filter_register(MSFilterInfo *info)
{
	gpointer tmp;
	tmp=g_list_find(filter_list,info);
	if (tmp==NULL) filter_list=g_list_append(filter_list,(gpointer)info);
}

void ms_filter_unregister(MSFilterInfo *info)
{
	filter_list=g_list_remove(filter_list,(gpointer)info);
}

static gint compare_names(gpointer info, gpointer name)
{
	MSFilterInfo *i=(MSFilterInfo*) info;
	return (strcmp(i->name,name));
}

MSFilterInfo * ms_filter_get_by_name(gchar *name)
{
	return((MSFilterInfo*) g_list_find_custom(filter_list,
						(gpointer)name,(GCompareFunc)compare_names));
}



MSFilter * ms_filter_new_with_name(gchar *name)
{
	MSFilterInfo *info=ms_filter_get_by_name(name);
	if (info!=NULL) return info->constructor();
	g_warning("ms_filter_new_with_name: no filter named %s found.",name);
	return NULL;
}


/* find the first codec in the left part of the stream */
/*
MSFilter * ms_filter_search_upstream_by_type(MSFilter *f,MSFilterType type)
{
	MSFilter *tmp=f;
	MSFilterInfo *info;
	
	if ((tmp->infifos!=NULL) && (tmp->infifos[0]!=NULL)){
		tmp=(MSFilter*) tmp->infifos[0]->link.prev;
		while(1){
			info=MS_FILTER_GET_CLASS(tmp)->info;
			if (info!=NULL){
				if ( (info->type==type) ){
					return tmp;
				}
			}
			if ((tmp->infifos!=NULL) && (tmp->infifos[0]!=NULL))
				tmp=(MSFilter*) tmp->infifos[0]->link.prev;
			else break;
		}
	}
	tmp=f;
	if ((tmp->inqueues!=NULL) && (tmp->inqueues[0]!=NULL)){
		tmp=(MSFilter*) tmp->inqueues[0]->link.prev;
		while(1){
		
			info=MS_FILTER_GET_CLASS(tmp)->info;
			if (info!=NULL){
				if ( (info->type==type)){
					return tmp;
				}
			}else g_warning("ms_filter_search_upstream_by_type: filter %s has no info."
							,MS_FILTER_GET_CLASS(tmp)->name);
			if ((tmp->inqueues!=NULL) && (tmp->inqueues[0]!=NULL))
				tmp=(MSFilter*) tmp->inqueues[0]->prev_data;
			else break;
		}	
	}
	return NULL;
}

*/
int ms_filter_set_property(MSFilter *f, MSFilterProperty prop,void *value)
{
	if (f->vtable->set_property!=NULL){
		return f->vtable->set_property(f,prop,value);
	}
	return 0;
}

int ms_filter_get_property(MSFilter *f, MSFilterProperty prop,void *value)
{
	if (f->vtable->get_property!=NULL){
		return f->vtable->get_property(f,prop,value);
	}
	return -1;
}

void ms_filter_set_notify_func(MSFilter* filter,MSFilterNotifyFunc func, gpointer userdata)
{
	filter->notify_event=func;
	filter->userdata=userdata;
}

void ms_filter_notify_event(MSFilter *filter,gint event, gpointer arg)
{
	if (filter->notify_event!=NULL){
		filter->notify_event(filter,event,arg,filter->userdata);
	}
}

void swap_buffer(gchar *buffer, gint len)
{
	int i;
	gchar tmp;
	for (i=0;i<len;i+=2){
		tmp=buffer[i];
		buffer[i]=buffer[i+1];
		buffer[i+1]=tmp;
	}
}
