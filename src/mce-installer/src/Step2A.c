#include "common.h"


void on_Step2A_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP2A);
	if (setting_runDhcpServer) {
		displayStep3();
	} else {
		displayStep2E();
	}
}	

void on_Step2A_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		gboolean value = (gboolean) data;
		setting_runDhcpServer = value;
	}
}

void displayStep2A(void) {
	printf("Step2A\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new("Your network appears to be setup correctly.  You have 2 network cards, and the card: [name it] is connected to your internet connection.  The other card: [name it] should be connected to the switch or hub for your home’s LAN, into which all the other devices in your home are connected.\n\nShall I run a DHCP server on this, so I can provide media and services to all the devices in your home?");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, run a DHCP server for my home network.");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step2A_radio_toggled), (gpointer)TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioYes));
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, I’ll set my network options manually.");
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_Step2A_radio_toggled), (gpointer)FALSE);
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, FALSE, 0);

	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step2A_forward_clicked), NULL);

	if (setting_runDhcpServer) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	} else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioNo), TRUE);
	}

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
