#include "common.h"

void on_Step3_forward_clicked(GtkWidget *widget, gpointer data) {
	//g_queue_push_head(history, (gpointer)STEP3);
	// Install a MD here 
}

void displayStep3(void) {
	printf("Step3\n");
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new ("What is the base IP address I should use for the devices in your home?  Normally the default is fine. You will also need a DHCP server");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);
	
	//GtkWidget *label2 = gtk_label_new ("It’s highly recommended that this Core act as your home’s DHCP server.  That means the computer which gives IP addresses to all the devices in your home.  This allows me to serve media and provide services throughout the home.  There can only be 1 DHCP server in the home, though, and often times a cable or dsl or other internet also wants to provide that service.  One solution is to add a 2nd network card to this system.  Then you will plug your internet connection into one of the network ports, and the other network port will go into your home’s network where all the other devices are attached.  The other possibility is to configure your cable or dsl modem, or an access point, so it gives an IP address only for this computer, or uses a static ip.");
	//gtk_label_set_line_wrap(GTK_LABEL(label2), TRUE);
	//gtk_box_pack_start(GTK_BOX(mainBox), label2, TRUE, TRUE, 0);
	GtkTable *tableNetwork = GTK_TABLE(gtk_table_new(2,2,TRUE));
		// Configuration Label and Entry
		GtkWidget *labelConfiguration = gtk_label_new("Run a DHCP Server:");
		gtk_misc_set_alignment(GTK_MISC(labelConfiguration), 0, .5);

		GtkWidget *comboConfiguration = gtk_combo_box_new_text();
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboConfiguration), "Yes");
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboConfiguration), "No");
		gtk_combo_box_set_active(GTK_COMBO_BOX(comboConfiguration), 0);

		gtk_table_attach_defaults(tableNetwork, labelConfiguration, 0, 1, 0, 1);
		gtk_table_attach_defaults(tableNetwork, comboConfiguration, 1, 2, 0, 1);

		// IP Input Label
		GtkWidget *labelIP = gtk_label_new("Internal Network : ");
		gtk_misc_set_alignment(GTK_MISC(labelIP), 0, .5);
		GtkWidget *entryIP = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryIP), "192.168.80");
		gtk_table_attach_defaults(tableNetwork, labelIP, 0, 1, 1, 2);
		gtk_table_attach_defaults(tableNetwork, entryIP, 1, 2, 1, 2);

	gtk_box_pack_start(GTK_BOX(mainBox), GTK_WIDGET(tableNetwork), TRUE, TRUE, 0);

	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step3_forward_clicked), NULL);
		
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}

