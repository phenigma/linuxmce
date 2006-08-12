/*
linphonec
Copyright (C) 2002  Florian Winterstein (flox@gmx.net)
Copyright (C) 2000  Simon MORLAT (simon.morlat@free.fr)

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

#include <linphonecore.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

LinphoneCore linphonec;
FILE *mylogfile;
FILE *configfile;
#define LINE_MAX_LEN 256

extern gboolean linphonec_parse_command_line(LinphoneCore *lc,const gchar *cl);

//auto answer (-a) option
static gboolean auto_answer=FALSE;
//auto terminate on process termination (-t) option
static gboolean terminate_on_close = FALSE;
//auto terminate control flag
static gboolean terminate=FALSE;

/*
gpointer tmalloc(int size)
{
	gpointer p;
	struct timeval before,after;
	gettimeofday(&before,NULL);
	p=malloc(size);
	gettimeofday(&after,NULL);
	if (getelapsed(&before,&after)>500){
		g_error("Spending too much time in malloc() of size %i",size);
	}
	return p;
}

GMemVTable dbgtable={
	tmalloc,
	realloc,
	free,
	calloc,
	tmalloc,
	realloc
};
*/
void
stub ()
{
}

//Process termination handling
void term(int sig)
{
   terminate = TRUE;
   char input[LINE_MAX_LEN];
   snprintf (input,sizeof(LINE_MAX_LEN),"%s", "terminate");
   linphonec_parse_command_line(&linphonec,input);
   snprintf (input,sizeof(LINE_MAX_LEN), "%s", "quit");
   linphonec_parse_command_line(&linphonec,input);
}


//Auto answer handling
void
linphonec_display_something (LinphoneCore * lc, char *something)
{
	fprintf (stdout, "%s\n", something);
}

void
linphonec_display_warning (LinphoneCore * lc, char *something)
{
	fprintf (stdout, "Warning: %s\n", something);
}

void
linphonec_display_url (LinphoneCore * lc, char *something, char *url)
{
	fprintf (stdout, "%s : %s\n", something, url);
}



static LinphoneAuthInfo *pending_auth=NULL;

void linphonec_prompt_for_auth(LinphoneCore *lc, const char *realm, const char *username)
{
	pending_auth=linphone_auth_info_new(username,NULL,NULL,NULL,realm);
	
}

void linphonec_prompt_for_auth_final(LinphoneCore *lc){
	char input[LINE_MAX_LEN];
	char passwd[LINE_MAX_LEN];
	printf("Authentication requested for username '%s' on realm %s.\n. Please enter your password:",
		pending_auth->username,pending_auth->realm);
	fgets (input, LINE_MAX_LEN-1, stdin);
	sscanf(input,"%s",passwd);
	linphone_auth_info_set_passwd(pending_auth,passwd);
	linphone_core_add_auth_info(lc,pending_auth);
	pending_auth=NULL;
}

void linphonec_call_received(LinphoneCore *lc, const char *from){
	if ( auto_answer)  {
		fprintf (stdout, "-------auto answering to call-------" );
		linphone_core_accept_dialog(lc,NULL);
	}
}

LinphoneCoreVTable linphonec_vtable = {
	show:(ShowInterfaceCb) stub,
	inv_recv: linphonec_call_received,
	bye_recv:(ByeReceivedCb) stub,
	auth_info_requested: linphonec_prompt_for_auth,
	display_status:linphonec_display_something,
	display_message:linphonec_display_something,
	display_warning:linphonec_display_warning,
	display_url:linphonec_display_url,
	display_question:stub
};

void
print_usage ()
{
	fprintf (stdout, "\n\
usage: linphonec [-c file] [-s sipaddr] [-a] [-d level -l logfile]\n\
       linphonec -v\n\
\n\
  -c  file             specify path of configuration file.\n\
  -d  level            be verbose. 0 is no output. 6 is all output\n\
  -l  logfile          specify the log file for your SIP phone\n\
  -s  sipaddress       specify the sip call to do at startup\n\
  -a		       enable auto answering for incoming calls\n\
  -t		       enable auto terminate on process end\n\
  -v or --version      display version and exits.\n");
}



int
linphonec_main_loop (LinphoneCore * opm, char * sipAddr)
{
	if (terminate_on_close) {
           signal(SIGTERM, term); 
        }
	//auto call handling
	gboolean firstTimeInLoop=TRUE;
	char input[LINE_MAX_LEN];
	int err;
	fd_set fdset;
	struct timeval timeout;
	gboolean run=TRUE;
	printf("linphonec> ");fflush(stdout);
	while (run && !terminate)
	{
		FD_ZERO(&fdset);
		FD_SET(0,&fdset);
		timeout.tv_sec=0;
		timeout.tv_usec=20000;
		err=select(1,&fdset,NULL,NULL,&timeout);
		if (err==0){
			linphone_core_iterate(opm);
			if (pending_auth!=NULL) linphonec_prompt_for_auth_final(opm);
			//auto call handling
			if (sipAddr  != NULL && firstTimeInLoop) {
		 		firstTimeInLoop=FALSE;
				snprintf (input, sizeof(input),"call %s", sipAddr);
				run=linphonec_parse_command_line(&linphonec,input);
			}
			continue;
		}
		else if (err<0 && !terminate){
			g_warning("Error in select(): %s",strerror(errno));
			continue;
		}
		fgets (input, LINE_MAX_LEN-1, stdin);
		run=linphonec_parse_command_line(&linphonec,input);
		printf("linphonec> ");fflush(stdout);
	}
	printf("\n");
	return 0;
}

int
main (int argc, char *argv[])
{
	char *logfile_name = NULL;
	char *configfile_name = NULL;
	char *sipAddr = NULL;
	char *p;
	int trace_level = 0;
	int arg_num;

	//g_mem_set_vtable(&dbgtable);

	configfile = NULL;
	mylogfile = NULL;
	arg_num = 1;

#ifdef ENABLE_NLS
	p = bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	if (p == NULL)
		perror ("bindtextdomain failed");
#ifndef __ARM__
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
	textdomain (GETTEXT_PACKAGE);
#else
	printf ("NLS disabled.\n");
#endif

	while (arg_num < argc)
	{
		int old_arg_num = arg_num;
		if (strncmp ("-d", argv[arg_num], 2) == 0)
		{
			arg_num++;
			if (arg_num < argc)
				trace_level = atoi (argv[arg_num]);
			else
				trace_level = 1;
		}
		else if (strncmp ("-l", argv[arg_num], 2) == 0)
		{
			arg_num++;
			if (arg_num < argc)
				logfile_name = argv[arg_num];
		}
		else if (strncmp ("-c", argv[arg_num], 2) == 0)
		{
			arg_num++;
			if (arg_num < argc)
				configfile_name = argv[arg_num];
		}
		else if (strncmp ("-s", argv[arg_num], 2) == 0)
		{
			arg_num++;
			if (arg_num < argc)
				sipAddr = argv[arg_num];
		}
                else if (strncmp ("-a", argv[arg_num], 2) == 0)
                {
                        auto_answer = TRUE;
                }
                else if (strncmp ("-t", argv[arg_num], 2) == 0)
                {
                        terminate_on_close = TRUE;
                }
		else if ((strncmp ("-v", argv[arg_num], 2) == 0)
			 ||
			 (strncmp
			  ("--version", argv[arg_num],
			   strlen ("--version")) == 0))
		{
			printf ("version: " LINPHONE_VERSION "\n");
			exit (0);
		}
		else if (old_arg_num == arg_num)
		{
			fprintf (stderr, "ERROR: bad arguments\n");
			print_usage ();
			exit (0);
		}
		arg_num++;
	}

	if (configfile_name == NULL)
	{
		configfile_name =
			g_strdup_printf ("%s/.linphonec", getenv ("HOME"));
	}
	else
	{
		if (!g_file_test (configfile_name, G_FILE_TEST_EXISTS))
		{
			fprintf (stderr, "Cannot open config file %s.\n",
				 configfile_name);
			exit (1);
		}
	}
	
	if (trace_level > 0)
	{
		if (logfile_name != NULL)
			mylogfile = fopen (logfile_name, "w+");

		if (mylogfile == NULL)
		{
			mylogfile = stdout;
			fprintf (stderr,
				 "INFO: no logfile, logging to stdout\n");
		}
		linphone_core_enable_logs(mylogfile);
	}
	else
	{
		linphone_core_disable_logs();
	}
	/* init */
	linphone_core_init (&linphonec, &linphonec_vtable, configfile_name,
			    NULL);
	linphonec_main_loop (&linphonec, sipAddr);

	linphone_core_uninit (&linphonec);

	if (mylogfile != NULL && mylogfile != stdout)
		fclose (mylogfile);

	exit (0);
	return 0;
}
