/***************************************************************************
 *            commands.c
 *
 *  Sun Oct 10 17:27:57 2004
 *  Copyright  2004  Simon MORLAT
 *  Email simon.morlat@linphone.org
 ****************************************************************************/

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
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <linphonecore.h>

#define LINE_MAX_LEN 100

void linphonec_help(const gchar *arg){
	if (strlen(arg)==0){
		printf("Commands are:\n call answer refer terminate proxy call-logs ipv6\nType 'help <command>' for more details.\n");
		return;
	}
	if (strcmp(arg,"call")==0){
		printf("'call <sip-url>' or 'c <sip-url>' : initiate a call to the specified destination.\n");
	}else if (strcmp(arg,"answer")==0){
		printf("'answer' or 'a' : answer to (accept) an incoming call.\n");
	}else if (strcmp(arg,"terminate")==0){
		printf("'terminate' or 't' :  terminate the current call.\n");
    }else if (strcmp(arg,"refer")==0){
        printf("'refer <sip-url>' or 'r <sip-url>' : refer the current call to the specified destination.\n");
	}else if (strcmp(arg,"quit")==0){
		printf("'quit' or 'q' : exit linphonec.\n");
	}else if (strcmp(arg,"proxy")==0){
		printf("'proxy list' : list all proxy setups.\n");
		printf("'proxy add' : add a new proxy setup.\n");
		printf("'proxy remove <index>' : remove proxy setup with number index.\n");
		printf("'proxy use <index>' : use proxy with number index as default proxy.\n");
		printf("'proxy unuse' : don't use a default proxy.\n");
	}else if (strcmp(arg,"call-logs")==0){
		printf("'call-logs' : show an history of calls.\n");
	}else if (strcmp(arg,"ipv6")==0){
		printf("'ipv6 status' : show ipv6 usage status.\n");
		printf("'ipv6 enable' : enable the use of the ipv6 network.\n");
		printf("'ipv6 disable' : do not use ipv6 network.\n");
	}
	else {
		printf("No such command.\n");
	}
}

void linphonec_proxy_add(LinphoneCore *lc){
	gchar tmp[LINE_MAX_LEN],arg[LINE_MAX_LEN];
	LinphoneProxyConfig *cfg;
	tmp[0]=0;
	printf("Adding new proxy setup:\n");
	printf("enter proxy sip address: \n");
	fgets(tmp,LINE_MAX_LEN,stdin);
	sscanf(tmp,"%s",arg);
	cfg=linphone_proxy_config_new(arg);
	if (cfg==NULL) {
		printf("Invalid sip address.");
		return;
	}
	printf("enter your identity sip address for this proxy: \n");
	fgets(tmp,LINE_MAX_LEN,stdin);
	sscanf(tmp,"%s",arg);
	linphone_proxy_config_set_identity(cfg,arg);
	printf("Do you want to register on this proxy (yes/no): \n");
	fgets(tmp,LINE_MAX_LEN,stdin);
	if (strstr(tmp,"yes")!=NULL){
		int expires=0;
		linphone_proxy_config_enableregister(cfg,TRUE);
		printf("Specify register expiration time in seconds (default is 600): \n");
		fgets(tmp,LINE_MAX_LEN,stdin);
		if (strlen(tmp)>1) {
			sscanf(tmp,"%i",&expires);
			linphone_proxy_config_expires(cfg,expires);
		}
	}
	printf("Specify route if needed: \n");
	fgets(tmp,LINE_MAX_LEN,stdin);
	if (strlen(tmp)>1){
		sscanf(tmp,"%s",arg);
		linphone_proxy_config_set_route(cfg,arg);
	}
	linphone_core_add_proxy_config(lc,cfg);
	/* automatically set the last entered proxy as the default one */
	linphone_core_set_default_proxy(lc,cfg);
	printf("Proxy added.\n");
}

void linphonec_proxy_display(LinphoneProxyConfig *cfg){
	printf("sip address: %s\nroute: %s\nidentity: %s\nregister: %s\nexpires: %i\n",
			cfg->reg_proxy,
			(cfg->reg_route!=NULL)?cfg->reg_route:"",
			(cfg->reg_identity!=NULL)?cfg->reg_identity:"",
			(cfg->reg_sendregister)?"yes":"no",
			cfg->expires);
}

void linphonec_proxy_list(LinphoneCore *lc){
	GList *proxies;
	int n;
	int def=linphone_core_get_default_proxy(lc,NULL);
	
	proxies=linphone_core_get_proxy_config_list(lc);
	for(n=0;proxies!=NULL;proxies=g_list_next(proxies),n++){
		if (n==def)
			printf("****** Proxy %i - this is the default one - *******\n",n);
		else 
			printf("****** Proxy %i *******\n",n);
		linphonec_proxy_display((LinphoneProxyConfig*)proxies->data);
	}
}

void linphonec_proxy_remove(LinphoneCore *lc, int index){
	GList *proxies;
	LinphoneProxyConfig *cfg;
	proxies=linphone_core_get_proxy_config_list(lc);
	cfg=(LinphoneProxyConfig*)g_list_nth_data(proxies,index);
	if (cfg==NULL){
		printf("No such proxy.");
		return;
	}
	linphone_core_remove_proxy_config(lc,cfg);
}

void linphonec_proxy_use(LinphoneCore *lc, int index){
	GList *proxies;
	LinphoneProxyConfig *cfg;
	proxies=linphone_core_get_proxy_config_list(lc);
	cfg=(LinphoneProxyConfig*)g_list_nth_data(proxies,index);
	if (cfg==NULL){
		printf("No such proxy.");
		return;
	}
	linphone_core_set_default_proxy(lc,cfg);
}

void linphonec_proxy_unuse(LinphoneCore *lc){
	linphone_core_set_default_proxy(lc,NULL);
}

void linphonec_show_logs(LinphoneCore *lc){
	GList *elem=linphone_core_get_call_logs(lc);
	for (;elem!=NULL;elem=g_list_next(elem)){
		LinphoneCallLog *cl=(LinphoneCallLog*)elem->data;
		gchar *str=linphone_call_log_to_str(cl);
		printf("%s\n",str);
		g_free(str);
	}
}

void linphonec_ipv6(LinphoneCore *lc, const char *arg1){
	if (strcmp(arg1,"status")==0){
		printf("ipv6 use enabled: %s\n",linphone_core_ipv6_enabled(lc) ? "true":"false");
	}else if (strcmp(arg1,"enable")==0){
		linphone_core_enable_ipv6(lc,TRUE);
		printf("ipv6 use enabled.\n");
	}else if (strcmp(arg1,"disable")==0){
		linphone_core_enable_ipv6(lc,FALSE);
		printf("ipv6 use disabled.\n");
	}
}

gboolean linphonec_parse_command_line(LinphoneCore *lc,const gchar *cl){
	int cllen=strlen(cl);
	gchar *command=g_malloc0(cllen);
	gchar *arg1=g_malloc0(cllen);
	gchar *arg2=g_malloc0(cllen);
	gboolean cont=TRUE;
	sscanf(cl,"%s %s %s",command,arg1,arg2);	 
	if (strcmp(command,"call")==0 || strcmp(command,"c")==0){
		linphone_core_invite(lc,arg1);
    }else if (strcmp(command,"refer")==0 || strcmp(command,"r")==0){
        linphone_core_refer(lc,arg1);
	}else if (strcmp(command,"terminate")==0 || strcmp(command,"t")==0){
		linphone_core_terminate_dialog(lc,NULL);
	}else if (strcmp(command,"answer")==0 || strcmp(command,"a")==0){
		linphone_core_accept_dialog(lc,NULL);
	}else if (strcmp(command,"quit")==0 || strcmp(command,"q")==0){
		cont=FALSE;		
	}else if (strcmp(command,"help")==0 ){
		linphonec_help(arg1);	
	}else if (strcmp(command,"proxy")==0 ){
		if (strcmp(arg1,"add")==0){
			linphonec_proxy_add(lc);
		}else if (strcmp(arg1,"list")==0){
			linphonec_proxy_list(lc);
		}else if (strcmp(arg1,"remove")==0){
			linphonec_proxy_remove(lc,atoi(arg2));
		}
		else if (strcmp(arg1,"use")==0){
			linphonec_proxy_use(lc,atoi(arg2));
		}
		else if (strcmp(arg1,"unuse")==0){
			linphonec_proxy_unuse(lc);
		}
	}else if (strcmp(command,"call-logs")==0){
		linphonec_show_logs(lc);	
	}else if (strcmp(command,"ipv6")==0){
		linphonec_ipv6(lc,arg1);
	}else if (strlen(command)==1){
		switch(command[0]){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '#':
			case '*':
				linphone_core_send_dtmf(lc, command[0]);
				break;
		}
	}else if (strlen(command)==0){
	}else {
		gchar *tmp=g_strdup(cl);
		gchar *eol;
		if ((eol=strchr(tmp,'\n'))!=NULL){
			eol[0]='\0';
		}
		printf("'%s' : Cannot understand this.\n",tmp);
		g_free(tmp);
	}
	g_free(command);
	g_free(arg1);
	g_free(arg2);
	return cont;
}
