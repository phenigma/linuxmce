/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


gint run_shell_command_progress_pulse(gpointer data) {
	GtkProgressBar *pbar = GTK_PROGRESS_BAR(data);
	gtk_progress_bar_pulse(pbar);
	return 1;
}

void run_shell_child_exited(VteTerminal *terminal, gpointer data) {
	g_source_remove((gint)data);
}

void run_shell_command(const gchar* application, const gchar* windowTitle) {
	GtkWidget *runWindow;

	runWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(runWindow), windowTitle);
	gtk_window_set_modal(GTK_WINDOW(runWindow), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(runWindow), GTK_WINDOW(mainWindow)); 
	gtk_window_set_position(GTK_WINDOW(runWindow), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_container_set_border_width(GTK_CONTAINER(runWindow), 10);

	GtkWidget* runHBox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(runWindow), runHBox);

	GtkWidget* runLabelWait = gtk_label_new("Please Wait ...");
	gtk_box_pack_start(GTK_BOX(runHBox), runLabelWait, TRUE, TRUE, 5);

	GtkWidget* runProgress = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(runHBox), runProgress, TRUE, TRUE, 5);
	gint runTagProgessBar = g_timeout_add(200, run_shell_command_progress_pulse, runProgress);

	GtkWidget* runTerminal = vte_terminal_new();
	vte_terminal_fork_command(VTE_TERMINAL(runTerminal),"ls",NULL, NULL, "", FALSE, FALSE, FALSE);
	g_signal_connect(G_OBJECT(runTerminal), "child-exited", G_CALLBACK(run_shell_child_exited), (gpointer)runTagProgessBar);

	gtk_widget_show_all(runWindow);
}



void on_StepUbuntuExtraCD_forward_clicked(GtkWidget *widget, gpointer data)  {
	g_queue_push_head(history, (gpointer)STEPAPTMIRROR);	
	run_shell_command("ls", "This is a title");
//      displayStep5();
}

void on_StepUbuntuExtraCD_file_set(GtkWidget *widget, gpointer data) {
	GtkWidget *radioUseISO = GTK_WIDGET(data);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioUseISO), TRUE);
	setting_ubuntuExtraCdIsoPath = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
	printf("setting_ubuntuExtraCdIsoPath = %s\n", setting_ubuntuExtraCdIsoPath);
}

void on_StepUbuntuExtraCD_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		setting_ubuntuExtraCdFrom = (gint)data;
		printf("setting_ubuntuExtraCdFrom = %d\n", (gint)data);
	}
}

void displayStepUbuntuExtraCD(void) {
	printf("StepUbuntuExtraCD\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("To eliminate the need to download a lot of packages from the internet, which can lead to installation errors if the internet connection goes down, it is recommended you first get the 'Kubuntu extra packages CD'.  If you have it, please insert it now, or locate the .iso image on your hard drive.");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);


	// Radio button install from Internet
	GSList *group = NULL;
	GtkWidget *radioUseNet = gtk_radio_button_new_with_label(group, "I don't have it. Download packages from the Internet.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseNet, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseNet), "toggled", G_CALLBACK(on_StepUbuntuExtraCD_radio_toggled), (gpointer)FROM_NET);

	// Radio button install using CD
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioUseNet));
	GtkWidget *radioUseCD = gtk_radio_button_new_with_label(group, "It's in the CD drive.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseCD, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseCD), "toggled", G_CALLBACK(on_StepUbuntuExtraCD_radio_toggled), (gpointer)FROM_CD);

	// Radio button install using ISO
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radioUseNet));
	GtkWidget *radioUseISO = gtk_radio_button_new_with_label(group, "I have an iso image on my harddrive.");
	gtk_box_pack_start(GTK_BOX(mainBox), radioUseISO, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(radioUseISO), "toggled", G_CALLBACK(on_StepUbuntuExtraCD_radio_toggled), (gpointer)FROM_ISO);

	// File chooser button
	GtkWidget *fileChooser = gtk_file_chooser_button_new("Choose 'Kubuntu Extra CD ISO' Location", GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_box_pack_start(GTK_BOX(mainBox), fileChooser, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(fileChooser), "selection-changed", G_CALLBACK(on_StepUbuntuExtraCD_file_set), radioUseISO);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepUbuntuExtraCD_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
