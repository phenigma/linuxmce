/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"
#include "StepCreateDiskedDevice.h"
#include <string.h>

GtkWidget *progress_CreateDevice;

void on_StepCreateDiskedDevice_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEPCREATEMD);
	displayStep3();
}

void on_StepCreateDiskedDevice_message(const gchar* message) {
	gdk_threads_enter();

	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_CreateDevice), message);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_CreateDevice), gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progress_CreateDevice)) + 0.2);

	gdk_threads_leave();
}

void *create_thread(void *arg) {
	GtkWidget *buttonForward = GTK_WIDGET(arg);

	int server_socket, client_socket;
	struct sockaddr_in saddr_client, saddr_server;

	char buffer[1024];
	int bytes;
	
	saddr_client.sin_family = AF_INET;
	saddr_client.sin_port = htons(7238);
	inet_aton(setting_coreIP, &saddr_client.sin_addr);

	saddr_server.sin_family = AF_INET;
	saddr_server.sin_port = htons(7238);
	saddr_server.sin_addr.s_addr = INADDR_ANY;

	// Start interactor server on client for commands
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		printf("ERROR: server_socket cannot be created\n");
		return NULL;
	}
	//set_close_on_exec(server_socket);
	if (bind(server_socket, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) == -1) {
		printf("ERROR: cannot bind on server_socket\n");
		return NULL;
	}
	if (listen(server_socket, 5) == -1) {
		printf("ERROR: cannot listen on server_socket\n");
		return NULL;
	}

	client_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("ERROR: client_socket cannot be created\n");
		return NULL;
	}
	int tmp = 1;
	if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)) == -1) {
		printf("ERROR: cannot set socket options on client_socket\n");
		return NULL;
	}
	if (connect(client_socket, (struct sockaddr *) &saddr_client, sizeof(saddr_client)) == -1) {
		printf("ERROR: cannot connect on client_socket\n");
		return NULL;
	}

	bytes = snprintf(buffer, 1024, "newmd 10.0.0.66 00:11:22:33:44:55 9|0\n");
	write(client_socket, buffer, bytes < 1024 ? bytes : 1024);

	close (client_socket);

	gboolean stop = FALSE;
	while (!stop) {
		int recv_socket = accept(server_socket, NULL, NULL);
		if (recv_socket == -1) {
			printf("ERROR: recv_socket cannot be created\n");
			return NULL;
		}

		while (!stop && (bytes = read(recv_socket, buffer, 1023)) > 0 ) {
			char cmd[1024];

			buffer[bytes] = 0;
			memset(cmd, 0, 1024);
			printf("Buffer: %s", buffer);
			sscanf(buffer, "%s", cmd);
			if (strcmp(cmd, "reboot") == 0) {
				buffer[bytes-1]=0;
				stop = TRUE;
			} else if (strcmp(cmd, "msg") == 0) { 
				buffer[bytes-1]=0;
				if (strcmp(buffer+4, "Rebooting") == 0) {
					on_StepCreateDiskedDevice_message("Finished Creating Device");
				} else {
					on_StepCreateDiskedDevice_message(buffer+4);
				}

			} else if (strcmp(cmd, "deviceid") == 0) {
				setting_deviceID = g_strdup(buffer+9);
			}
		}
		close(recv_socket);
	}
	close(server_socket);

	gtk_widget_set_sensitive(GTK_WIDGET(buttonForward), TRUE);
	return NULL;
}

void displayStepCreateDiskedDevice(void) {
	printf("StepCreateDiskedDevice\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Creating Media Directory device on the router.");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

	// Progress bar
	progress_CreateDevice = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(mainBox), progress_CreateDevice, FALSE, FALSE, 0);	

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepCreateDiskedDevice_forward_clicked), NULL);

	// Start the thread that takes care of creating the device
	if (setting_deviceID == 0) {
		pthread_t create_tid;
		pthread_create(&create_tid, NULL, create_thread, (void*)buttonForward);
		gtk_widget_set_sensitive(GTK_WIDGET(buttonForward), FALSE);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_CreateDevice), "Use an existing orbiter to continue");
	} else {
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_CreateDevice), "Finished Creating Device");
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_CreateDevice), 1);
	}

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
