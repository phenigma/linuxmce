#include "common.h"

void on_Step2E_forward_clicked(GtkWidget *widget, gpointer data) {
	//g_queue_push_head(history, (gpointer)STEP3);
	// Install a MD here 
}

void displayStep2E(void) {
	printf("Step2E\n");
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new ("Manual Network Configuration.");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	GtkTable *tableNetwork = GTK_TABLE(gtk_table_new(6,2,FALSE));
		// Configuration Label and Entry	
		GtkWidget *labelConfiguration = gtk_label_new("Configuration:");
		gtk_misc_set_alignment(GTK_MISC(labelConfiguration), 0, .5);

		GtkWidget *comboConfiguration = gtk_combo_box_new_text();
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboConfiguration), "Automatic Configuration (DHCP)");
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboConfiguration), "Static IP address");
		gtk_combo_box_set_active(GTK_COMBO_BOX(comboConfiguration), 0);

		gtk_table_attach_defaults(tableNetwork, labelConfiguration, 0, 1, 0, 1);
		gtk_table_attach_defaults(tableNetwork, comboConfiguration, 1, 2, 0, 1);

		// IP Label and Entry
		GtkWidget *labelIP = gtk_label_new("IP address:");
		gtk_misc_set_alignment(GTK_MISC(labelIP), 0, .5);
		GtkWidget *entryIP = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryIP), setting_netExtIP);
		gtk_table_attach_defaults(tableNetwork, labelIP, 0, 1, 1, 2);
		gtk_table_attach_defaults(tableNetwork, entryIP, 1, 2, 1, 2);

		// Netmask Label and Entry
		GtkWidget *labelNetmask = gtk_label_new("Subnet mask:");
		gtk_misc_set_alignment(GTK_MISC(labelNetmask), 0, .5);
		GtkWidget *entryNetmask = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryNetmask), setting_netExtMask);
		gtk_table_attach_defaults(tableNetwork, labelNetmask, 0, 1, 2, 3);
		gtk_table_attach_defaults(tableNetwork, entryNetmask, 1, 2, 2, 3);

		// Gateway Label and Entry
		GtkWidget *labelGateway = gtk_label_new("Gateway address:");
		gtk_misc_set_alignment(GTK_MISC(labelGateway), 0, .5);
		GtkWidget *entryGateway = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryGateway), setting_netExtGateway);
		gtk_table_attach_defaults(tableNetwork, labelGateway, 0, 1, 3, 4);
		gtk_table_attach_defaults(tableNetwork, entryGateway, 1, 2, 3, 4);

		// DNS1 Label and Entry
		GtkWidget *labelDNS1 = gtk_label_new("Primary DNS Server:");
		gtk_misc_set_alignment(GTK_MISC(labelDNS1), 0, .5);
		GtkWidget *entryDNS1 = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryDNS1), setting_netExtDNS1);
		gtk_table_attach_defaults(tableNetwork, labelDNS1, 0, 1, 4, 5);
		gtk_table_attach_defaults(tableNetwork, entryDNS1, 1, 2, 4, 5);
		
		// DNS2 Label and Entry
		GtkWidget *labelDNS2 = gtk_label_new("Secondary DNS Server:");
		gtk_misc_set_alignment(GTK_MISC(labelDNS2), 0, .5);
		GtkWidget *entryDNS2 = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entryDNS2), setting_netExtDNS2);
		gtk_table_attach_defaults(tableNetwork, labelDNS2, 0, 1, 5, 6);
		gtk_table_attach_defaults(tableNetwork, entryDNS2, 1, 2, 5, 6);
	gtk_box_pack_start(GTK_BOX(mainBox), GTK_WIDGET(tableNetwork), TRUE, TRUE, 0);



	// Button Back
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);
	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step2E_forward_clicked), NULL);
		
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}

