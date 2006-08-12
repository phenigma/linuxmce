#include "SIP_Thread.h"

#include <linphonecore.h>

static LinphoneCore linphoneCore;
static LinphoneCoreVTable linphoneCoreVTable;

/* Callbacks for linphone core */
static void stub () {}
static void call_received(LinphoneCore *linphoneCore, const char *from);
static void prompt_for_auth(LinphoneCore *linphoneCore, const char *realm, const char *username);
static void display_something (LinphoneCore * linphoneCore, char *something);
static void display_url (LinphoneCore * linphoneCore, char *something, char *url);
static void display_warning (LinphoneCore * linphoneCore, char *something);
static void bye_received(LinphoneCore *linphoneCore, const char *from);
static void text_received(LinphoneCore *linphoneCore, LinphoneChatRoom *cr, const char *from, const char *msg);
static void display_status (LinphoneCore * linphoneCore, char *something);

/* SIP Thread */
void * SIP_Thread(void * arg)
{
	return NULL;
}
