#include "common.h"


void on_Step0A_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP0A);

	displayStep0B();
}



void on_Step0A_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
	}
}

void displayStep0A(void) {
	displayStep3();
	return;

	printf("Step0A\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("What method do you want to use to install Linux MCE ?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radioOne = gtk_radio_button_new_with_label(group, "Download the needed packages form the Internet.");
	g_signal_connect(G_OBJECT(radioOne), "toggled", G_CALLBACK(on_Step0A_radio_toggled), NULL);
	gtk_box_pack_start(GTK_BOX(mainBox), radioOne, TRUE, FALSE, 0);
	
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioOne));
	GtkWidget *radioTwo = gtk_radio_button_new_with_label(group, "Download and use an ISO from the one of LinuxMCE mirrors.");
	g_signal_connect(G_OBJECT(radioTwo), "toggled", G_CALLBACK(on_Step0A_radio_toggled), NULL);
	gtk_box_pack_start(GTK_BOX(mainBox), radioTwo, TRUE, FALSE, 0);

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioOne));
	GtkWidget *radioThree = gtk_radio_button_new_with_label(group, "Use the Linux MCE CDROM that i allready have in my drive.");
	g_signal_connect(G_OBJECT(radioThree), "toggled", G_CALLBACK(on_Step0A_radio_toggled), NULL);
	gtk_box_pack_start(GTK_BOX(mainBox), radioThree, TRUE, FALSE, 0);

	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step0A_forward_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
