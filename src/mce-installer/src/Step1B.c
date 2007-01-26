
#include "common.h"

void on_Step1B_forward_clicked(GtkWidget *widget, gpointer data) {
		g_queue_push_head(history, (gpointer)STEP1B);
	        displayStep4();
}

void displayStep1B(void) {
	printf("Step1B\n");
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new ("I could not detect the IP address associated with your Core computer that's why you need to supply the IP addres manualy.");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	GtkWidget *hboxIP = gtk_hbox_new(FALSE, 0);
		// IP Input Label
		GtkWidget *labelIP = gtk_label_new("Core's IP Address : ");
		gtk_box_pack_start(GTK_BOX(hboxIP), labelIP, FALSE, FALSE, 0);

		//Ip Input Entry
		GtkWidget *entryIP = gtk_entry_new();
		if (setting_coreIP) {
			gtk_entry_set_text(GTK_ENTRY(entryIP), setting_coreIP);
		}
		gtk_box_pack_start(GTK_BOX(hboxIP), entryIP, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(mainBox), hboxIP, TRUE, TRUE, 0);

	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step1B_forward_clicked), NULL);
		
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}

