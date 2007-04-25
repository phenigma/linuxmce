/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


void on_StepUI_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEPUI);
        displayStepDvdCss();
}

void on_StepUI_finish_clicked(GtkWidget *widget, gpointer data) {

}

void on_StepUI_radio_toggled(GtkWidget *widget, gpointer data) {
	setting_UI = (gint)data;
	printf("UI: %d\n", setting_UI);
}

void displayStepUI(void) {
	printf("StepUI\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("What interface do you want to use ?");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Questions
	GSList *group = NULL;
	GtkWidget *radio[3];

	radio[0] = gtk_radio_button_new_with_label(group, "Basic UI (UI1)");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
	radio[1] = gtk_radio_button_new_with_label(group, "UI2 with with medium settings (masking)");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
	radio[2] = gtk_radio_button_new_with_label(group, "UI2 with alpha blending/transparency");

	int i;
	for (i=0; i<3; i++) {
		g_signal_connect(G_OBJECT(radio[i]), "toggled", G_CALLBACK(on_StepUI_radio_toggled), (gpointer)i);
		gtk_box_pack_start(GTK_BOX(mainBox), radio[i], TRUE, FALSE, i);
		if (setting_UI == i) { 
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio[i]), TRUE);
		}
	}

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepUI_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
