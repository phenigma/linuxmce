/*
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"
#include <unistd.h>
#include <sys/types.h>
 
void start_network_wizard(void) {

	// Display wait message
	cleanupContainer(mainBox); 
        cleanupContainer(mainButtonBox); 
			 
	GtkWidget *label = gtk_label_new_for_wizard ("Detecting current network settings, please wait ...");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);

	// Read network configuration
	int fd[2];
	pid_t script_pid;
	int hasInternet = 0;

	pipe(fd);
	script_pid=fork();

	if (script_pid == 0) {
		close(fd[0]);
		close(1);
		dup(fd[1]);
		execlp("./mce-installer-getnetdata.sh","./mce-installer-getnetdata.sh",NULL);

	} else if (script_pid > 0) {
		close(fd[1]);
		gchar readbuffer[1024];
		int size=0;

		memset(readbuffer, 0, 1024);
		size = read(fd[0], readbuffer, 1023);
		readbuffer[size-1] = '\0';
	//	printf("Read: %s --\n",readbuffer);

		if(size==0 || g_ascii_strcasecmp(readbuffer, "0")==0) {
			printf("No network cards detected\n");
			setting_netIfaceNo=0;
		} else {
			gchar **tokens;
			setting_netIfaceNo = g_ascii_strtoll(readbuffer, NULL, 10);

			// Get inerface names
			memset(readbuffer, 0, 1024);
			size = read(fd[0], readbuffer, 1023);
			readbuffer[size-1] = '\0';
	//		printf("Read: %s --\n",readbuffer);

			tokens = g_strsplit(readbuffer, "|", 2);
			setting_netExtName = tokens[0];	
			setting_netIntName = tokens[1];

			// Get external interface settings
			memset(readbuffer, 0, 1024);
			size = read(fd[0], readbuffer, 1023);
			readbuffer[size-1] = '\0';
	//		printf("Read: %s --\n",readbuffer);

			tokens = g_strsplit(readbuffer, "|", 7);
			setting_netExtIP = tokens[0];
			setting_netExtMask = tokens[1];
			setting_netExtGateway = tokens[2];
			setting_netExtDNS1 = tokens[3];
			setting_netExtDNS2 = tokens[4];
			setting_netExtUseDhcp = g_ascii_strtoll(tokens[5], NULL, 10);
			hasInternet = g_ascii_strtoll(tokens[6], NULL, 10);
		}

	}


	if (setting_netIfaceNo == 0) {
	       displayStep2F();
	
	} else {
		if (hasInternet) {
			displayStep2A();
		} else {
			displayStep2C();
		}
	}
}

