/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_StepDvdCss_forward_clicked(GtkWidget *widget, gpointer data)  {

	gboolean goNext = TRUE;
	gboolean installDvdCss = * (gboolean *)data;

	if (installDvdCss == TRUE) {	
		goNext = FALSE;
		GtkWidget* runWindow = run_shell_command("./mce-installer-DvdCss.sh", "Installing DVD CSS", "Failed to cache the DVD CSS");
		gtk_widget_show_all(runWindow);
		if (gtk_dialog_run(GTK_DIALOG(runWindow)) == 1) {
			goNext = TRUE;
		}
	}

	if (goNext) {
		g_queue_push_head(history, (gpointer)STEPDVDCSS);
		displayStep4();
	}
}

void on_StepDvdCss_radio_no_toggled(GtkWidget *widget, gpointer data) {
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gboolean *) data = FALSE;
	}
}

void on_StepDvdCss_radio_yes_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		* (gboolean *) data = TRUE;
	}
}


void displayStepDvdCss(void) {

	printf("StepDvdCss\n");

	gboolean* installDvdCss;
	installDvdCss = (gboolean *)malloc(sizeof(gboolean));
	*installDvdCss = TRUE;

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("The software to play encrypted, commercial dvd’s (libdvdcss) is not installed.  Is this legal in your area and should it be installed?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);


	// Radio : Yes
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, i want it installed.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_StepDvdCss_radio_yes_toggled), (gpointer)installDvdCss);

	// Radio : No
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioYes));
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, it's ilegal in my contry to use it.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_StepDvdCss_radio_no_toggled), (gpointer)installDvdCss);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepDvdCss_forward_clicked), (gpointer)installDvdCss);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
