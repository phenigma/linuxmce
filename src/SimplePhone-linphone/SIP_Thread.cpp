#include "SIP_Thread.h"

#include <linphonecore.h>
#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"
using namespace DCE;

static inline void func_enter(const char * func)
{
	g_pPlutoLogger->Write(LV_STATUS, "%s - called", func);
}

static inline void func_exit(const char * func)
{
	g_pPlutoLogger->Write(LV_STATUS, "%s - exited", func);
}

/* Callbacks for linphone core */
static void stub() {}
static void call_received(LinphoneCore *linphoneCore, const char *from);
static void bye_received(LinphoneCore *linphoneCore, const char *from);
static void auth_requested(LinphoneCore *linphoneCore, const char *realm, const char *username);
static void display_something(LinphoneCore * linphoneCore, char *something);
static void display_url(LinphoneCore * linphoneCore, char *something, char *url);
static void display_warning (LinphoneCore * linphoneCore, char *something);
	
/* C Object, private members */
static LinphoneCore LS_LinphoneCore;
static LinphoneCoreVTable LS_LinphoneCoreVTable;
static LinphoneProxyConfig * LS_pLinphoneProxyConfig = NULL;
static LinphoneAuthInfo * LS_pLinphoneAuthInfo = NULL;

static SimplePhone * LS_pSimplePhone;

static bool LS_bActiveCall = false;
static string LS_Auth_Username;
static string LS_Auth_Realm;
static int LS_Auth_Received = 0;

/* C Object, public members */
bool LS_bQuit = false;

/* C Object, private methods - declaration */
static void LS_RegisterWithAsterisk();
static void LS_Main_Loop();
static void LS_UnregisterWithAsterisk();
static void LS_InitVTable();
static void LS_InitProxy();
static void LS_SetupAuth();
static void LS_SignalHandler(int ExitStatus);

/* C Object, private methods - implementation */

/** Register with Asterisk */
static void LS_RegisterWithAsterisk()
{
	func_enter("LS_RegisterWithAsterisk");
	//linphone_core_enable_logs(stdout);
	linphone_core_disable_logs();
	linphone_core_init(&LS_LinphoneCore, &LS_LinphoneCoreVTable, NULL, NULL);
	linphone_core_set_sip_port(&LS_LinphoneCore, 5061);
	linphone_core_add_proxy_config(&LS_LinphoneCore, LS_pLinphoneProxyConfig);
	linphone_core_set_default_proxy(&LS_LinphoneCore, LS_pLinphoneProxyConfig);
	func_exit("LS_RegisterWithAsterisk");
}

/** Thread main loop */
static void LS_Main_Loop()
{
	func_enter("LS_Main_Loop");
	while (!LS_bQuit)
	{
		linphone_core_iterate(&LS_LinphoneCore);
		Sleep(20);
		if (LS_Auth_Received == 1)
			LS_SetupAuth();
	}
	func_exit("LS_Main_Loop");
}

/** Unregister with Asterisk */
static void LS_UnregisterWithAsterisk()
{
	func_enter("LS_UnregisterWithAsterisk");
	linphone_core_terminate_dialog(&LS_LinphoneCore, NULL);
	linphone_core_uninit(&LS_LinphoneCore);
	func_exit("LS_UnregisterWithAsterisk");
}

/** Initialize LinphoneCoreVTable variable */
static void LS_InitVTable()
{
	LS_LinphoneCoreVTable.show = (ShowInterfaceCb) stub;
	LS_LinphoneCoreVTable.inv_recv = call_received;
	LS_LinphoneCoreVTable.bye_recv = bye_received;
	LS_LinphoneCoreVTable.auth_info_requested = auth_requested;
	LS_LinphoneCoreVTable.display_status = display_something;
	LS_LinphoneCoreVTable.display_message = display_something;
	LS_LinphoneCoreVTable.display_warning = display_warning;
	LS_LinphoneCoreVTable.display_url = display_url;
	LS_LinphoneCoreVTable.display_question = (DisplayQuestionCb) stub;
}

/** Initialize LinphoneProxyConfig variable */
static void LS_InitProxy()
{
	func_enter("LS_InitProxy");
	DCEConfig dceconf;
	
	string sProxy = /* dceconf.m_sDBHost.c_str() */ "dcerouter";
	string sExtension = /* LS_pSimplePhone->DATA_Get_PhoneNumber() */ "210";
	string sPassword = sExtension;
	string sIdentity = "sip:" + sExtension + "@" + sProxy;
	sProxy = "sip:" + sProxy;
	
	g_pPlutoLogger->Write(LV_STATUS,
			"LS_InitProxy -- Proxy: %s; Extension: %s; Identity: %s",
			sProxy.c_str(), sExtension.c_str(), sIdentity.c_str());
	if (sExtension.length() == 0)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Extension is empty. Attempting to sync with AMP");
        system("/usr/pluto/bin/LaunchRemoteCmd.sh dcerouter \"/usr/pluto/bin/sync_pluto2amp.pl\"");
		LS_bQuit = true;
		return;
	}
	
	LS_pLinphoneProxyConfig = linphone_proxy_config_new(sProxy.c_str());
	if (! LS_pLinphoneProxyConfig)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "LS_InitProxy: Failed to initialize proxy");
		LS_bQuit = true;
		return;
	}
	
	linphone_proxy_config_set_identity(LS_pLinphoneProxyConfig, sIdentity.c_str());
	linphone_proxy_config_enableregister(LS_pLinphoneProxyConfig, TRUE);
	linphone_proxy_config_expires(LS_pLinphoneProxyConfig, 600);
	func_exit("LS_InitProxy");
}

/** Setup SIP authentication */
static void LS_SetupAuth()
{
	string sPassword = /* LS_pSimplePhone->DATA_Get_PhoneNumber() */ "210";
	linphone_auth_info_set_passwd(LS_pLinphoneAuthInfo, sPassword.c_str());
	linphone_core_add_auth_info(&LS_LinphoneCore, LS_pLinphoneAuthInfo);
	LS_Auth_Received = 0;
}

static void LS_SignalHandler(int ExitStatus)
{
	g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: terminated by signal");
	LS_bQuit = true;
}

/* C Object, public methods - implementation */

/** Linphone SIP Thread */
void * LS_Thread(void * arg)
{
	LS_pSimplePhone = (SimplePhone *) arg;
	
	signal(SIGTERM, LS_SignalHandler);
	signal(SIGINT, LS_SignalHandler);
	
	LS_InitVTable();
	LS_InitProxy();
	
	/* start doing stuff */
	g_pPlutoLogger->Write(LV_STATUS, "Started SIP/Linphone thread");

	LS_RegisterWithAsterisk();
	LS_Main_Loop();
	LS_UnregisterWithAsterisk();

	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);

	exit(0);
	return NULL;
}

/* Returns true of a call is in progress, false otherwise */
bool LS_ActiveCall()
{
	return LS_bActiveCall;
}

/* Send a DTMF tone though and active call */
void LS_SentDTMF(char cDTMF)
{
	linphone_core_send_dtmf(&LS_LinphoneCore, cDTMF);
}

/* Initiate a call */
void LS_InitiateCall(string sNumber)
{
	linphone_core_invite(&LS_LinphoneCore, sNumber.c_str());
	LS_bActiveCall = true;
}

/* Drop the active call */
void LS_DropCall()
{
	func_enter("LS_DropCall");
	linphone_core_terminate_dialog(&LS_LinphoneCore, NULL);
	LS_bActiveCall = false;

	string tmp;
	LS_pSimplePhone->CallDroppedScreen();
	func_exit("LS_DropCall");
}

/* Accept the incoming call */
void LS_AcceptCall()
{
	linphone_core_accept_dialog(&LS_LinphoneCore, NULL);
	LS_bActiveCall = true;
}

/* Linphone callbacks - implementation */
static void call_received(LinphoneCore *linphoneCore, const char *from)
{
	g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: Received call, from '%s'", from);
	if (strcmp(from, "plutosecurity") == 0)
	{
		g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: It's a 'speak in the house' call");
		LS_AcceptCall();
	}
	else
	{
		LS_pSimplePhone->IncomingCallScreen(from);
	}
}

static void bye_received(LinphoneCore *linphoneCore, const char *from)
{
	g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: Received bye, from '%s'", from);
	LS_DropCall();
}

static void auth_requested(LinphoneCore *linphoneCore, const char *realm, const char *username)
{
	func_enter("callback: auth_requested");
	g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: Authorisation requested, realm: '%s', username '%s'", realm, username);

	if (LS_pLinphoneAuthInfo != NULL)
	{
		g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: Authorisation requested one too many times");
		func_exit("callback: auth_requested");
		return;
	}
	
	LS_Auth_Username = username;
	LS_Auth_Realm = realm;
	LS_Auth_Received = 1;
	
	LS_pLinphoneAuthInfo = linphone_auth_info_new(username, NULL, NULL, NULL, realm);
	func_exit("callback: auth_requested");
}

static void display_something(LinphoneCore * linphoneCore, char *something)
{
	g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: lib message: '%s'", something);
}

static void display_url(LinphoneCore * linphoneCore, char *something, char *url)
{
	g_pPlutoLogger->Write(LV_STATUS, "SimplePhone: lib url: '%s' '%s'", something, url);
}

static void display_warning (LinphoneCore * linphoneCore, char *something)
{
	g_pPlutoLogger->Write(LV_WARNING, "SimplePhone: lib warning: '%s'", something);
}
