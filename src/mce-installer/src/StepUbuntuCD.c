/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_StepUbuntuCD_forward_clicked(GtkWidget *widget, gpointer data)  {
	g_queue_push_head(history, (gpointer)STEPAPTMIRROR);	
        displayStep5();
}

void on_StepUbuntuCD_file_set(GtkWidget *widget, gpointer data) {
	GtkWidget *radioUseISO = GTK_WIDGET(data);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioUseISO), TRUE);
	setting_ubuntuCdIsoPath = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
	printf("setting_ubuntuCdIsoPath = %s\n", setting_ubuntuCdIsoPath);
}

void on_StepUbuntuCD_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		setting_ubuntuCdFrom = (gint)data;
		printf("setting_ubuntuCdFrom = %d\n", (gint)data);
	}
}


void displayStepUbuntuCD(void) {
	printf("StepUbuntuCD\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("If you're going to add extra media stations, I'll need to create new Kubuntu images for them.  Rather than downloading Kubuntu again from the internet, I recommend you put the original Kubuntu Feisty CD back in the drive so I can make a local copy of it.");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);


	// Radio button install from Internet
	GSList *group = NULL;
	GtkWidget *radioUseNet = gtk_radio_button_new_with_label(group, "I don't have it. Download packages from the Internet.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseNet, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseNet), "toggled", G_CALLBACK(on_StepUbuntuCD_radio_toggled), (gpointer)FROM_NET);

	// Radio button install using CD
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioUseNet));
	GtkWidget *radioUseCD = gtk_radio_button_new_with_label(group, "It's in the CD drive.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseCD, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseCD), "toggled", G_CALLBACK(on_StepUbuntuCD_radio_toggled), (gpointer)FROM_CD);

	// Radio button install using ISO
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioUseNet));
	GtkWidget *radioUseISO = gtk_radio_button_new_with_label(group, "I have an iso image on my harddrive.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseISO, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseISO), "toggled", G_CALLBACK(on_StepUbuntuCD_radio_toggled), (gpointer)FROM_ISO);

	// File chooser button
	GtkWidget *fileChooser = gtk_file_chooser_button_new("Choose 'Kubuntu Install CD ISO' Location", GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_box_pack_start(GTK_BOX(mainBox), fileChooser, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(fileChooser), "selection-changed", G_CALLBACK(on_StepUbuntuCD_file_set), radioUseISO);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepUbuntuCD_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
