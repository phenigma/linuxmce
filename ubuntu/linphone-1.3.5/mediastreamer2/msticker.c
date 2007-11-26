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



#include "msticker.h"


void * ms_ticker_run(void *s);

void ms_ticker_start(MSTicker *s){
	s->run=TRUE;
	ms_thread_create(&s->thread,NULL,ms_ticker_run,s);
}


void ms_ticker_init(MSTicker *ticker)
{
	ms_mutex_init(&ticker->lock,NULL);
	ms_cond_init(&ticker->cond,NULL);
	ticker->execution_list=NULL;
	ticker->ticks=1;
	ticker->time=0;
	ticker->interval=10;
	ticker->run=FALSE;
	ticker->exec_id=0;
	ms_ticker_start(ticker);
}

MSTicker *ms_ticker_new(){
	MSTicker *obj=ms_new(MSTicker,1);
	ms_ticker_init(obj);
	return obj;
}

void ms_ticker_stop(MSTicker *s){
	ms_mutex_lock(&s->lock);
	s->run=FALSE;
	ms_cond_wait(&s->cond,&s->lock);
	ms_mutex_unlock(&s->lock);
}


void ms_ticker_uninit(MSTicker *ticker)
{
	ms_ticker_stop(ticker);
	ms_mutex_destroy(&ticker->lock);
	ms_cond_destroy(&ticker->cond);
}

void ms_ticker_destroy(MSTicker *ticker){
	ms_ticker_uninit(ticker);
	ms_free(ticker);
}

void find_sources(MSList **sources, MSFilter *f, void (*func)(MSFilter*) ){
	int i;
	MSQueue *link;
	if (f==NULL) ms_fatal("Bad graph.");
	/*ms_message("seeing %s, seen=%i",f->desc->name,f->seen);*/
	if (f->seen){
		return;
	}
	f->seen=TRUE;
	func(f);	/*execute supplied func on each seen filter */
	f->last_tick=0;
	if (f->desc->ninputs==0){
		/* this is a source */
		*sources=ms_list_append(*sources,f);
	}
	/* go upstream */
	for(i=0;i<f->desc->ninputs;i++){
		link=f->inputs[i];
		if (link!=NULL) find_sources(sources,link->prev.filter,func);
	}
	/* go downstream */
	for(i=0;i<f->desc->noutputs;i++){
		link=f->outputs[i];
		if (link!=NULL) find_sources(sources,link->next.filter,func);
	}
}


int ms_ticker_attach(MSTicker *ticker,MSFilter *f)
{
	MSList *sources=NULL;
	find_sources(&sources,f,ms_filter_preprocess);
	if (sources==NULL){
		ms_error("No sources found.");
		return -1;
	}
	ms_mutex_lock(&ticker->lock);
	ticker->execution_list=ms_list_concat(ticker->execution_list,sources);
	ms_mutex_unlock(&ticker->lock);
	return 0;
}



int ms_ticker_detach(MSTicker *ticker,MSFilter *f)
{	
	MSList *sources=NULL;
	MSList *it;
	/* we must lock around find_sources because seen is set to FALSE every tick */
	ms_mutex_lock(&ticker->lock);
	find_sources(&sources,f,ms_filter_postprocess);
	if (sources==NULL){
		ms_mutex_unlock(&ticker->lock);
		ms_error("No sources found.");
		return -1;
	}
	
	for(it=sources;it!=NULL;it=ms_list_next(it)){
		ticker->execution_list=ms_list_remove(ticker->execution_list,it->data);
	}
	ms_mutex_unlock(&ticker->lock);
	ms_list_free(sources);
	return 0;
}


bool_t filter_can_process(MSFilter *f, int tick){
	/* look if filters before this one have run */
	int i;
	MSQueue *l;
	for(i=0;i<f->desc->ninputs;i++){
		l=f->inputs[i];
		if (l!=NULL){
			if (l->prev.filter->last_tick!=tick) return FALSE;
		}
	}
	return TRUE;
}

void call_process(MSFilter *f){
	if (f->desc->ninputs==0 || ms_filter_inputs_have_data(f)) {
			ms_filter_process(f);
	}
}

void run_graph(MSFilter *f, MSTicker *s){
	int i;
	MSQueue *l;
	if (f->last_tick!=s->ticks && filter_can_process(f,s->ticks)){
		/* this is a candidate */
		f->last_tick=s->ticks;
		f->ticker=s; /*redundant*/
		f->seen=FALSE; /*redundant, also*/
		call_process(f);	
		/* now recurse to next filters */		
		for(i=0;i<f->desc->noutputs;i++){
			l=f->outputs[i];
			if (l!=NULL){
				run_graph(l->next.filter,s);
			}
		}
	}
}

void run_graphs(MSTicker *s){
	MSList *it;
	for(it=s->execution_list;it!=NULL;it=it->next){
		run_graph((MSFilter*)it->data,s);
	}
}

static uint64_t get_cur_time(){
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME,&ts)<0){
		ms_fatal("clock_gettime() doesn't work: %s",strerror(errno));
	}
	return (ts.tv_sec*1000LL) + (ts.tv_nsec/1000000LL);
}

static void sleepMs(int ms){
	struct timespec ts;
	ts.tv_sec=0;
	ts.tv_nsec=ms*1000000LL;
	nanosleep(&ts,NULL);
}

void * ms_ticker_run(void *arg)
{
	MSTicker *s=(MSTicker*)arg;
	uint64_t realtime;
	uint64_t orig=get_cur_time();
	int lastlate=0;
	s->ticks=1;
	ms_mutex_lock(&s->lock);
	while(s->run){
		s->ticks++;
		run_graphs(s);
		ms_mutex_unlock(&s->lock);
		s->time+=s->interval;
		while(1){
			int64_t diff;
			realtime=get_cur_time()-orig;
			diff=s->time-realtime;
			if (diff>0){
				/* sleep until next tick */
				sleepMs(diff);
			}else{
				int late=-diff;
				if (late>s->interval*5 && late>lastlate){
					ms_warning("We are late of %d miliseconds.",late);
				}
				lastlate=late;
				break; /*exit the while loop */
			}
		}
		ms_mutex_lock(&s->lock);
	}
	ms_cond_signal(&s->cond);
	ms_mutex_unlock(&s->lock);
	return NULL;
}
