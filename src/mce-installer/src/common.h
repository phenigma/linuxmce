#ifndef __COMMON_H_ 
#define __COMMON_H_

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "Step1A.h"
#include "Step1B.h"
#include "Step1C.h"
#include "Step1D.h"

#define STEP1A 1
#define STEP1B 2
#define STEP1C 3
#define STEP1D 4
#define STEP1E 5

GladeXML  *gxml;
GtkWidget *mainWindow;
GtkWidget *mainBox;
GtkWidget *mainButtonBox;
GQueue    *history;

void		cleanupContainer(GtkWidget *widget);
gchar*		detectCoreIpAddress(void);
void		on_back_clicked(GObject object, gpointer data);

#define DT_CORE 1
#define DT_HYBRID 2
#define DT_MEDIA_DIRECTOR 3

gchar*	setting_coreIP;
gint	setting_deviceType; // 1-Core, 2-Hybrid, 3-MediaDirector

#endif
