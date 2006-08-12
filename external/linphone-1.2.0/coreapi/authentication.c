/***************************************************************************
 *            authentication.c
 *
 *  Fri Jul 16 12:08:34 2004
 *  Copyright  2004  Simon MORLAT
 *  simon.morlat@linphone.org
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
 
#include "linphonecore.h"
#include "private.h"
#include "misc.h"
#include <eXosip.h>
#include <osipparser2/osip_message.h>
#include "eXosip2.h"
#include "lpconfig.h"

extern LinphoneProxyConfig *linphone_core_get_proxy_config_from_rid(LinphoneCore *lc, int rid);
extern void linphone_proxy_config_set_realm(LinphoneProxyConfig *cfg, const char *realm);
extern void linphone_core_retry_proxy_register(LinphoneCore *lc, const gchar *realm);

LinphoneAuthInfo *linphone_auth_info_new(const gchar *username, const gchar *userid,
				   										const gchar *passwd, const gchar *ha1,const gchar *realm)
{
	LinphoneAuthInfo *obj=g_new0(LinphoneAuthInfo,1);
	if (username!=NULL && (strlen(username)>0) ) obj->username=g_strdup(username);
	if (userid!=NULL && (strlen(userid)>0)) obj->userid=g_strdup(userid);
	if (passwd!=NULL && (strlen(passwd)>0)) obj->passwd=g_strdup(passwd);
	if (ha1!=NULL && (strlen(ha1)>0)) obj->ha1=g_strdup(ha1);
	if (realm!=NULL && (strlen(realm)>0)) obj->realm=g_strdup(realm);
	return obj;
}

void linphone_auth_info_set_passwd(LinphoneAuthInfo *info, const gchar *passwd){
	if (info->passwd!=NULL) {
		g_free(info->passwd);
		info->passwd=NULL;
	}
	if (passwd!=NULL && (strlen(passwd)>0)) info->passwd=g_strdup(passwd);
}

void linphone_auth_info_destroy(LinphoneAuthInfo *obj){
	if (obj->username!=NULL) g_free(obj->username);
	if (obj->userid!=NULL) g_free(obj->userid);
	if (obj->passwd!=NULL) g_free(obj->passwd);
	if (obj->ha1!=NULL) g_free(obj->ha1);
	if (obj->realm!=NULL) g_free(obj->realm);
	g_free(obj);
}

void linphone_auth_info_write_config(LpConfig *config, LinphoneAuthInfo *obj, int pos)
{
	gchar key[50];
	sprintf(key,"auth_info_%i",pos);
	lp_config_clean_section(config,key);
	
	if (obj==NULL){
		return;
	}		
	if (obj->username!=NULL){
		lp_config_set_string(config,key,"username",obj->username);
	}
	if (obj->userid!=NULL){
		lp_config_set_string(config,key,"userid",obj->userid);
	}
	if (obj->passwd!=NULL){
		lp_config_set_string(config,key,"passwd",obj->passwd);
	}
	if (obj->ha1!=NULL){
		lp_config_set_string(config,key,"ha1",obj->ha1);
	}
	if (obj->realm!=NULL){
		lp_config_set_string(config,key,"realm",obj->realm);
	}
	lp_config_sync(config);
}

LinphoneAuthInfo *linphone_auth_info_new_from_config_file(LpConfig * config, int pos)
{
	gchar key[50];
	const gchar *username,*userid,*passwd,*ha1,*realm;
	
	sprintf(key,"auth_info_%i",pos);
	if (!lp_config_has_section(config,key)){
		return NULL;
	}
	
	username=lp_config_get_string(config,key,"username",NULL);
	userid=lp_config_get_string(config,key,"userid",NULL);
	passwd=lp_config_get_string(config,key,"passwd",NULL);
	ha1=lp_config_get_string(config,key,"ha1",NULL);
	realm=lp_config_get_string(config,key,"realm",NULL);
	return linphone_auth_info_new(username,userid,passwd,ha1,realm);
}

static gboolean key_match(const gchar *tmp1, const gchar *tmp2){
	if (tmp1==NULL && tmp2==NULL) return TRUE;
	if (tmp1!=NULL && tmp2!=NULL && strcmp(tmp1,tmp2)==0) return TRUE;
	return FALSE;
	
}

static int auth_info_compare(LinphoneAuthInfo *info,LinphoneAuthInfo *ref){
	
	if (key_match(info->realm,ref->realm) && key_match(info->username,ref->username)) return 0;
	return -1;
}

LinphoneAuthInfo *linphone_core_auth_info_find(LinphoneCore *lc, const gchar *realm, const gchar *username)
{
	LinphoneAuthInfo ref;
	GList *elem;
	ref.realm=(gchar*)realm;
	ref.username=(gchar*)username;
	elem=g_list_find_custom(lc->auth_info,(gconstpointer)&ref,(GCompareFunc)auth_info_compare);
	if (elem==NULL) return NULL;
	return (LinphoneAuthInfo*)elem->data;
}

void linphone_core_add_auth_info(LinphoneCore *lc, LinphoneAuthInfo *info)
{
	int n;
	GList *elem;
	gchar *userid;
	if (info->userid==NULL || info->userid[0]=='\0') userid=info->username;
	else userid=info->userid;
	eXosip_lock();
	eXosip_add_authentication_info(info->username,userid,
				info->passwd,info->ha1,info->realm);
	eXosip_unlock();
	/* find if we are attempting to modify an existing auth info */
	elem=g_list_find_custom(lc->auth_info,(gconstpointer)info,(GCompareFunc)auth_info_compare);
	if (elem!=NULL){
		linphone_auth_info_destroy((LinphoneAuthInfo*)elem->data);
		elem->data=(gpointer)info;
		n=g_list_position(lc->auth_info,elem);
	}else {
		lc->auth_info=g_list_append(lc->auth_info,(gpointer)info);
		n=g_list_length(lc->auth_info)-1;
	}
	/* find if we need to restart a register request */
	linphone_core_retry_proxy_register(lc,info->realm);
}

void linphone_core_remove_auth_info(LinphoneCore *lc, LinphoneAuthInfo *info){
	int len=g_list_length(lc->auth_info);
	int newlen;
	int i;
	GList *elem;
	lc->auth_info=g_list_remove(lc->auth_info,(gconstpointer)info);
	newlen=g_list_length(lc->auth_info);
	/*printf("len=%i newlen=%i\n",len,newlen);*/
	linphone_auth_info_destroy(info);
	for (i=0;i<len;i++){
		linphone_auth_info_write_config(lc->config,NULL,i);
	}
	for (elem=lc->auth_info,i=0;elem!=NULL;elem=g_list_next(elem),i++){
		linphone_auth_info_write_config(lc->config,(LinphoneAuthInfo*)elem->data,i);
	}
	
}

void linphone_core_clear_all_auth_info(LinphoneCore *lc){
	GList *elem;
	int i;
	eXosip_lock();
	eXosip_clear_authentication_info();
	eXosip_unlock();
	for(i=0,elem=lc->auth_info;elem!=NULL;elem=g_list_next(elem),i++){
		LinphoneAuthInfo *info=(LinphoneAuthInfo*)elem->data;
		linphone_auth_info_destroy(info);
		linphone_auth_info_write_config(lc->config,NULL,i);
	}
	g_list_free(lc->auth_info);
	lc->auth_info=NULL;
}

gboolean linphone_core_find_or_ask_for_auth_info(LinphoneCore *lc,const char *username,const char* realm, gboolean force_ask)
{
	if (force_ask || linphone_core_auth_info_find(lc,realm,username)==NULL){
		if (lc->vtable.auth_info_requested!=NULL) {
			lc->vtable.auth_info_requested(lc,realm,username);
		}
		return FALSE;
	}else{
		return TRUE;
	}
}

gboolean linphone_process_authentication(LinphoneCore *lc, osip_message_t *resp, LinphoneProxyConfig *cfg, gboolean force_ask)
{
	char *prx_realm=NULL,*www_realm=NULL;
	osip_proxy_authenticate_t *prx_auth;
	osip_www_authenticate_t *www_auth;
	char *username;
	gboolean have_it=TRUE;
	
	username=osip_uri_get_username(resp->from->url);
	prx_auth=(osip_proxy_authenticate_t*)osip_list_get(resp->proxy_authenticates,0);
	www_auth=(osip_proxy_authenticate_t*)osip_list_get(resp->www_authenticates,0);
	if (prx_auth!=NULL)
		prx_realm=osip_proxy_authenticate_get_realm(prx_auth);
	if (www_auth!=NULL)
		www_realm=osip_www_authenticate_get_realm(www_auth);
	
	if (prx_realm==NULL && www_realm==NULL){
		g_warning("No realm in the server response.");
		return FALSE;
	}
	if (cfg!=NULL) {
		if (prx_realm!=NULL)
			linphone_proxy_config_set_realm(cfg,prx_realm);
		else if (www_realm!=NULL)
			linphone_proxy_config_set_realm(cfg,www_realm);
	}
	/* see if we already have this auth information , not to ask it everytime to the user */
	if (prx_realm!=NULL) 
		have_it=linphone_core_find_or_ask_for_auth_info(lc,username,prx_realm,force_ask);
	if (www_realm!=NULL) 
		have_it=linphone_core_find_or_ask_for_auth_info(lc,username,www_realm, force_ask) && have_it;
	return have_it;
}

void linphone_register_authentication_required(LinphoneCore *lc,eXosip_event_t *ev){
	osip_message_t *resp;
	LinphoneProxyConfig *cfg;
	eXosip_reg_t *reg=eXosip_event_get_reginfo(ev);
	g_return_if_fail(reg!=NULL);
	
	resp=reg->r_last_tr->last_response;
	g_return_if_fail(resp!=NULL);
	cfg=linphone_core_get_proxy_config_from_rid(lc,ev->rid);
	g_return_if_fail(cfg!=NULL);
	if (linphone_process_authentication(lc,resp,cfg,cfg->auth_pending)){
		/* we have the information, so retry the register */
		eXosip_lock();
		eXosip_register(ev->rid,-1);
		eXosip_unlock();
	}
	cfg->auth_pending=TRUE;
}

void linphone_invite_authentication_required(LinphoneCore* lc,LinphoneCall *lcall)
{
	osip_message_t *resp;
	eXosip_call_t *call=NULL;
	eXosip_call_find(lcall->cid,&call);
	g_return_if_fail(call!=NULL);
	resp=call->c_out_tr->last_response;
	g_return_if_fail(resp!=NULL);
	if (linphone_process_authentication(lc,resp,NULL,lcall->auth_pending)){
		eXosip_lock();
		eXosip_retry_call(lcall->cid);
		eXosip_unlock();
	}else{
		linphone_call_destroy(lcall);
		lc->call=NULL;
	}
	
}
