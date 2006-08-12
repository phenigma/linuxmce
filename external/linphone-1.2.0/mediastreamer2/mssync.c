/***************************************************************************
 *            mssync.c
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


#include "mssync.h"
#include "mslink.h"
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

void run_graph(MSFilter *f, int tick, void (*func)(MSFilter *));

void ms_sync_run(MSSync *s);

void ms_sync_start(MSSync *s){
	s->run=TRUE;
	s->thread=g_thread_create((GThreadFunc)ms_sync_run,(gpointer)s,TRUE,NULL);
}

void ms_sync_stop(MSSync *s){
	g_mutex_lock(s->lock);
	s->run=FALSE;
	g_cond_wait(s->cond,s->lock);
	g_mutex_unlock(s->lock);
}

void ms_sync_init(MSSync *sync)
{	
	sync->lock=g_mutex_new();
	sync->cond=g_cond_new();
	sync->execution_list=NULL;
	sync->ticks=1;
	sync->time=0;
	sync->interval=10;
	sync->thread=NULL;
	sync->run=FALSE;
	sync->exec_id=0;
	ms_sync_start(sync);
}

void ms_sync_uninit(MSSync *sync)
{
	ms_sync_stop(sync);
	if (sync->execution_list!=NULL) g_list_free(sync->execution_list);
	g_mutex_free(sync->lock);
	g_cond_free(sync->cond);
}

struct _SearchContext{
	GList *sources;
	int tag;
};
typedef struct _SearchContext SearchContext;

void search_context_init(SearchContext *s, int tag){
	s->sources=0;
	s->tag=tag;
}

void find_sources(SearchContext *ctx, MSFilter *f){
	int i;
	MSLink *link;
	if (f==NULL) g_error("Bad graph.");
	if (f->exec_id==ctx->tag){
		return;
	}
	f->exec_id=ctx->tag;
	f->last_tick=0;
	if (f->max_qinputs==0 && f->max_finputs==0){
		/* this is a source */
		ctx->sources=g_list_append(ctx->sources,(gpointer)f);
	}
	/* go upstream */
	for(i=0;i<f->max_finputs,link=(MSLink*)f->infifos[i];i++){
		if (link!=NULL) find_sources(ctx,link->prev);
	}
	for(i=0;i<f->max_qinputs,link=(MSLink*)f->inqueues[i];i++){
		if (link!=NULL) find_sources(ctx,link->prev);
	}
	/* go downstream */
	for(i=0;i<f->max_foutputs,link=(MSLink*)f->outfifos[i];i++){
		if (link!=NULL) find_sources(ctx,link->next);
	}
	for(i=0;i<f->max_qoutputs,link=(MSLink*)f->outqueues[i];i++){
		if (link!=NULL) find_sources(ctx,link->next);
	}
}

void run_graph_setup(MSFilter *f, int tick, MSSync *s){
	void _setup(MSFilter *f){
		ms_filter_setup_links(f,s);
	}
	run_graph(f, tick, _setup);
}

/**
 * ms_sync_attach:
 * @sync:  A #MSSync object.
 * @f:  A #MSFilter object.
 *
 * Attach a chain of filters to a synchronisation source @sync. Filter @f must be the first filter of the processing chain.
 *  In order to be run, each chain of filter must be attached to a synchronisation source, that will be responsible for scheduling
 *  the processing. Multiple chains can be attached to a single synchronisation.
 *
 * Returns: 0 if successfull, a negative value reprensenting the errno.h error.
 */
int ms_sync_attach(MSSync *sync,MSFilter *f)
{
	SearchContext ctx;
	sync->exec_id++;
	search_context_init(&ctx, sync->exec_id);
	find_sources(&ctx,f);
	if (ctx.sources==NULL){
		g_warning("No sources found.");
		return -EINVAL;
	}
	g_list_foreach(ctx.sources,(GFunc)run_graph_setup,GINT_TO_POINTER(1));
	g_mutex_lock(sync->lock);
	sync->execution_list=g_list_concat(sync->execution_list,ctx.sources);
	g_mutex_unlock(sync->lock);
}

/*note:
at the beginning: f->last_tick=0;
at run_graph_setup: f->last_tick=1;
at run_graph_process: f->last_tick=sync->tick_count
*/

/**
 * ms_sync_detach:
 * @sync:  A #MSSync object.
 * @f:  A #MSFilter object.
 *
 * Detach a chain of filters to a synchronisation source. Filter @f must be the first filter of the processing chain.
 * The processing chain will no more be executed.
 *
 * Returns: 0 if successfull, a negative value reprensenting the errno.h error.
 */
int ms_sync_detach(MSSync *sync,MSFilter *f)
{	
	GList *elem;
	GList *to_remove=NULL;
	MSFilter *tmp=NULL;
	int tag=f->exec_id;
	g_mutex_lock(sync->lock);
	for(elem=sync->execution_list;elem!=NULL;elem=g_list_next(elem)){
		tmp=(MSFilter*)elem->data;
		if (to_remove!=NULL) {
			sync->execution_list=g_list_delete_link(sync->execution_list,to_remove);
			ms_filter_unsetup_links(tmp);
			to_remove=NULL;
		}
		if (tmp->exec_id==tag) {
			to_remove=elem;
			g_message("Removing source %s:%x from scheduler list.",tmp->name,tmp);
		}
	}
	if (to_remove!=NULL) {
		sync->execution_list=g_list_delete_link(sync->execution_list,to_remove);
		ms_filter_unsetup_links(tmp);
		to_remove=NULL;
	}
	g_mutex_unlock(sync->lock);
	return 0;
}

static void dummy_handler(int signum)
{
	//posix_timer_time+=POSIXTIMER_INTERVAL/1000;
	//alarm_received++;
	//printf("tick !!!\n");
}

struct _PosixTimer{
	struct timeval orig;
	int time;
	int milisec;
	gboolean late_ticks;
};
typedef struct _PosixTimer PosixTimer;

static void posix_timer_init(PosixTimer *posix_timer, int milisec)
{
	struct itimerval timer;
	sigset_t set;
	/* block the SIGALRM signal 
	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);*/
	
	timer.it_value.tv_sec=0;
	timer.it_value.tv_usec=milisec*1000;
	timer.it_interval.tv_sec=0;
	timer.it_interval.tv_usec=milisec*1000;
	
	signal(SIGALRM,dummy_handler);
	setitimer(ITIMER_REAL,&timer,NULL);
	gettimeofday(&posix_timer->orig,NULL);
	posix_timer->late_ticks=FALSE;
	posix_timer->time=0;
	posix_timer->milisec=milisec;
}

static int posix_timer_elapsed_real_time(PosixTimer *posix_timer){
	struct timeval cur;
	gettimeofday(&cur,NULL);
	return ((cur.tv_usec-posix_timer->orig.tv_usec)/1000 ) + ((cur.tv_sec-posix_timer->orig.tv_sec)*1000 );
}

static void posix_timer_wait(PosixTimer *posix_timer)
{
	sigset_t set;
	guint32 time;
	gint32 diff;
	
	
	posix_timer->time+=posix_timer->milisec;
	
	while ( (diff=posix_timer_elapsed_real_time(posix_timer)-posix_timer->time)<0 ){
		sigfillset(&set);
		sigdelset(&set,SIGALRM);
		sigsuspend(&set);
		/* here we receive the signal */
	}
	posix_timer->time+=posix_timer->milisec;
	
	diff=time-posix_timer->time;
	
	if (diff> posix_timer->milisec*5){
		posix_timer->late_ticks=diff/posix_timer->milisec;
		if (!posix_timer->late_ticks) g_warning("MSTimer: must catchup %i ticks.",posix_timer->late_ticks);
		posix_timer->late_ticks=TRUE;
	}else posix_timer->late_ticks=FALSE;
}

static void posix_timer_uninit(PosixTimer *posix_timer)
{
	struct itimerval timer;
	/* unset the timer */
	memset(&timer,0,sizeof(struct itimerval));
	setitimer(ITIMER_REAL,&timer,NULL);
}


gboolean filter_can_process(MSFilter *f, int tick){
	/* look if filters before this one have run */
	int i;
	MSLink *l;
	for(i=0;i<f->max_finputs;i++){
		l=(MSLink*)f->infifos[i];
		if (l!=NULL){
			if (l->prev->last_tick!=tick) return FALSE;
		}
	}
	for(i=0;i<f->max_qinputs;i++){
		l=(MSLink*)f->inqueues[i];
		if (l!=NULL){
			if (l->prev->last_tick!=tick) return FALSE;
		}
	}
	return TRUE;
}

void call_process(MSFilter *f){
	if (ms_filter_is_source(f) || ms_filter_queues_have_data(f) || ms_filter_fifos_have_data(f)){
			ms_filter_process(f);
	}
}

void run_graph(MSFilter *f, int tick, void (*func)(MSFilter *)){
	int i;
	MSLink *l;
	if (f->last_tick!=tick && filter_can_process(f,tick)){
		/* this is a candidate */
		f->last_tick=tick;
		func(f);	
		/* now recurse to next filters */		
		for(i=0;i<f->max_foutputs;i++){
			l=(MSLink*)f->outfifos[i];
			if (l!=NULL){
				run_graph(l->next,tick,func);
			}
		}
		for(i=0;i<f->max_qoutputs;i++){
			l=(MSLink*)f->outqueues[i];
			if (l!=NULL){
				run_graph(l->next,tick,func);
			}
		}
	}
}

void run_graph_process(MSFilter *f, int tick){
	run_graph(f,tick,call_process);
}

void ms_sync_run(MSSync *s)
{
	PosixTimer timer;
	posix_timer_init(&timer,s->interval);
	s->ticks=1;
	g_mutex_lock(s->lock);
	while(s->run){
		s->ticks++;
		g_list_foreach(s->execution_list,(GFunc)run_graph_process,(gpointer)GINT_TO_POINTER(s->ticks));		
		g_mutex_unlock(s->lock);
		/* sleep until next interrupt */
		posix_timer_wait(&timer);
		s->time=timer.time;
		g_mutex_lock(s->lock);
	}
	posix_timer_uninit(&timer);
	g_cond_signal(s->cond);
	g_mutex_unlock(s->lock);
}
