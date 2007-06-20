/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_StepCheckSystem_forward_clicked(GtkWidget *widget, gpointer data) {
//	g_queue_push_head(history, (gpointer)STEPCHECKSYSTEM);
//	displayStep3();
}


void on_StepCheckSystem_details_callback( GtkWidget *widget, gpointer data) {
    g_print ("Hello again - %s was pressed\n", (char *) data);
}

void displayStepCheckSystem(void) {
	printf("StepCheckSystem\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);


	GtkWidget *label = gtk_label_new_for_wizard ("Test Results:");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);


	GtkWidget* runWindow = run_shell_command("./mce-installer-CheckSystem.sh", "Checking System", "Not a fresh Kubuntu system");
	gtk_widget_show_all(runWindow);
	gboolean packages_test=gtk_dialog_run(GTK_DIALOG(runWindow));

	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);
	
	label = gtk_label_new_for_wizard ("Test Results:");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);
	GtkWidget *table, *label1, *button;
	table = gtk_table_new (1, 3, TRUE);
	
	if (packages_test) { 
		//label = gtk_label_new_for_wizard ("Installed Packages: FAILED");
		label = gtk_label_new_for_wizard ("Installed Packages");
		label1 = gtk_label_new_for_wizard ("FAILED");
		button = gtk_button_new_with_label ("Details");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (on_StepCheckSystem_details_callback), (gpointer) "button 1");
		gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE(table), label1, 1, 2, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE(table), button, 2, 3, 0, 1);
	} else {
		label = gtk_label_new_for_wizard ("Installed Packages");
		label1 = gtk_label_new_for_wizard ("PASS");
		button = gtk_button_new_with_label ("Details");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (on_StepCheckSystem_details_callback), (gpointer) "button 1");
		gtk_table_attach_defaults (GTK_TABLE(table), label, 0, 1, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE(table), label1, 1, 2, 0, 1);
		gtk_table_attach_defaults (GTK_TABLE(table), button, 2, 3, 0, 1);
		//label = gtk_label_new_for_wizard ("installed Pacakage: PASS");
	}
	gtk_box_pack_start(GTK_BOX(mainBox), table, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);
	
	// Button Forward
	buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepCheckSystem_forward_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
