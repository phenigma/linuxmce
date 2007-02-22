#include "common.h"


void on_Step0A_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP1A);

	displayStep0B();
}



void on_Step0A_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
	}
}

void displayStep0A(void) {
	printf("Step0A\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Select a mirror to download the iso from :");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, i want to use this system as a Core");
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step0A_radio_toggled), NULL);
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step0A_forward_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
