#ifndef __COMMON_H_ 
#define __COMMON_H_

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib.h>

#include <vte/vte.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "Step1A.h"
#include "Step1B.h"
#include "Step1C.h"
#include "Step1D.h"
#include "Step2E.h"
#include "Step2C.h"
#include "Step2A.h"
#include "Step3.h"
#include "Step4.h"
#include "Step5.h"

#include "network-common.h"

#define STEP1A 1
#define STEP1B 2
#define STEP1C 3
#define STEP1D 4
#define STEP1E 5
#define STEP2A 6
#define STEP2C 7
#define STEP2E 8
#define STEP3  9
#define STEP4  10
#define STEP5  11

GladeXML  *gxml;
GtkWidget *mainWindow;
GtkWidget *mainBox;
GtkWidget *mainButtonBox;
GQueue    *history;

void		cleanupContainer(GtkWidget *widget);
gchar*		detectCoreIpAddress(void);
void		write_config_file(void);
void		on_back_clicked(GObject object, gpointer data);
GtkWidget*	gtk_label_new_for_wizard(const gchar *text);

#define DT_CORE 1
#define DT_HYBRID 2
#define DT_MEDIA_DIRECTOR 3

gchar*	 setting_coreIP;
gint	 setting_deviceType;
gint     setting_netIfaceNo;
gchar*   setting_netExtName;
gchar*   setting_netExtIP;
gchar*   setting_netExtMask;
gchar*   setting_netExtGateway;
gchar*   setting_netExtDNS1;
gchar*   setting_netExtDNS2;
gchar*   setting_netExtMac;
gboolean setting_netExtUseDhcp;
gboolean setting_netExtKeep;

gboolean setting_runDhcpServer;
gchar*   setting_netIntName;
gchar*   setting_netIntIPN;

gchar*   setting_deviceID;

gint     setting_startupType;

#endif
