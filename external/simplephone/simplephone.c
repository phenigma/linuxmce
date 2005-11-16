#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "iaxclient.h"

static int phone_status = 0;

static void doAnswer(int signal) 
{
	if(phone_status>0)
	{
		iaxc_answer_call(0);
		iaxc_select_call(0);
	}
}

static void doHangup(int signal) 
{
	iaxc_dump_call();
	iaxc_millisleep(1000);
}

static void doAbort(int signal) 
{
	iaxc_dump_all_calls();
	iaxc_millisleep(1000);
	exit(0);	
}

static void doProccess(void)
{
	while(1)
	{
		iaxc_process_calls();
		iaxc_millisleep(5);
		if(phone_status>0)
		{
			doAnswer(0);
		}
		else if(phone_status<0)
		{
			doHangup(0);
		}
	}
}

static int iaxCallback(iaxc_event e)
{
	if(e.type == IAXC_EVENT_STATE)
	{
		phone_status=0;
		if(e.ev.call.state == IAXC_CALL_STATE_FREE)
		{
			phone_status=-1;
		}
		else if(e.ev.call.state & IAXC_CALL_STATE_RINGING)
		{
			phone_status=1;
			if((!strcasecmp(e.ev.call.remote,"unknown")) && (!strcasecmp(e.ev.call.remote_name,"unknown")))
			{
				phone_status=2;
			}
/*			fprintf(stderr,"CALL \"%s\" \"%s\" \"%s\" \"%s\"\n",e.ev.call.remote,e.ev.call.remote_name,e.ev.call.local,e.ev.call.local_context);*/
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char tmp[256];
	if(argc<4 || argc>5)
	{
		fprintf(stderr,"\nUSAGE : %s <user> <pass> <host>\n\n",argv[0]);
		return 1;
	}

	if(iaxc_initialize(AUDIO_INTERNAL_PA,1)<0)
	{
		fprintf(stderr,"Could not start (maybe a sound issue?)\n");
		return 2;
	}
	iaxc_set_formats(IAXC_FORMAT_GSM,IAXC_FORMAT_GSM);
	iaxc_set_silence_threshold(-50);
	iaxc_set_event_callback(iaxCallback);
	iaxc_register(argv[1],argv[2],argv[3]);
	
	signal(SIGHUP,doAbort);
	signal(SIGINT,doAbort);
	signal(SIGPIPE,doAbort);
	signal(SIGTERM,doAbort);
	signal(SIGUSR1,doAnswer);
	signal(SIGUSR2,doHangup);

	if(argv[4])
	{
		snprintf(tmp,sizeof(tmp)-1,"%s:%s@%s/%s",argv[1],argv[2],argv[3],argv[4]);
		iaxc_call(tmp);
	}
	doProccess();
	return 0;
}
