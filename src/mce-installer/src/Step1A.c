#include "common.h"


void on_Step1A_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEP1A);

	if (setting_deviceType != 3) {
		displayStep1D();
	} else {
		displayStep1B();
	}
}



void on_Step1A_radio_toggled(GtkWidget *widget, gpointer data) {
	if(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) {
		gint value = (gint) data;
		printf("Setting deviceType %d\n", value);
		setting_deviceType = value;
	}
}

void displayStep1A(void) {
	printf("Step1A\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new ("You need to have one system in the home that will be designated as the ‘Master’, or ‘Core’.  This system should be left on all the time because it provides vital services to the home, such as phone service, and because any of the media players in the various rooms in your home need to talk to the Core to coordinate their activities.\n\nI could not find any Core on your network, so I assume you want this system to be your Core?  Is that correct?");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radioYes = gtk_radio_button_new_with_label(group, "Yes, i want to use this system as a Core");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioYes), TRUE);
	g_signal_connect(G_OBJECT(radioYes), "toggled", G_CALLBACK(on_Step1A_radio_toggled), (gpointer)DT_CORE);
	gtk_box_pack_start(GTK_BOX(mainBox), radioYes, TRUE, FALSE, 0);

	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioYes));
	GtkWidget *radioNo = gtk_radio_button_new_with_label(group, "No, i want a Media Director");
	g_signal_connect(G_OBJECT(radioNo), "toggled", G_CALLBACK(on_Step1A_radio_toggled), (gpointer)DT_MEDIA_DIRECTOR);
	gtk_box_pack_start(GTK_BOX(mainBox), radioNo, TRUE, FALSE, 0);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step1A_forward_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
