/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

GHashTable *mirrors;

void on_StepAptMirror_forward_clicked(GtkWidget *widget, gpointer data)  {
	g_queue_push_head(history, (gpointer)STEPAPTMIRROR);
        displayStep5();
}

void on_StepAptMirror_combo_changed(GtkWidget *widget, gpointer data) {
	//GtkWidget *tableMirror = GTK_WIDGET(data);
	setting_installMirror = g_hash_table_lookup(mirrors, gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	printf("Am schimbat in >> %s << \n", setting_installMirror);
}

void displayStepAptMirror(void) {
	printf("StepAptMirror\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Please choose a mirror");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	GtkTable *tableMirror = GTK_TABLE(gtk_table_new(2,2,TRUE));
	//Mirror label and combo box
	GtkWidget *labelMirror = gtk_label_new("Mirror:");
	gtk_misc_set_alignment(GTK_MISC(labelMirror), 0, .5);

	//read the list of mirrors
	FILE *fmirror = fopen("./mirrors", "r");
	char line[1024];
	gchar **tokens;
	mirrors = g_hash_table_new(g_str_hash, g_str_equal);
	GtkWidget *comboMirror = gtk_combo_box_new_text();
	while (1) {
		fgets(line,1024,fmirror);
		if (feof(fmirror)) break;
		//printf("Read %s--\n",line);
		line[strlen(line)-1] = '\0';
		tokens = g_strsplit(line,"|",0);
		g_hash_table_insert(mirrors,tokens[0],tokens[1]);
		//printf("name: %s -- mirror: %s\n",tokens[0], tokens[1]);
		gtk_combo_box_append_text(GTK_COMBO_BOX(comboMirror),tokens[0]);
	}

	//GtkWidget *comboMirror = gtk_combo_box_new_text();
	//gtk_combo_box_append_text(GTK_COMBO_BOX(comboMirror),"Mirror 1");
	//gtk_combo_box_append_text(GTK_COMBO_BOX(comboMirror),"Mirror 2");

	gtk_combo_box_set_active(GTK_COMBO_BOX(comboMirror),0);
	setting_installMirror = g_hash_table_lookup(mirrors, gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboMirror)));
	printf("Initial %s\n", setting_installMirror);

	g_signal_connect(G_OBJECT(comboMirror), "changed", G_CALLBACK(on_StepAptMirror_combo_changed), (gpointer)tableMirror);

	gtk_table_attach_defaults(tableMirror, labelMirror, 0, 1, 0, 1);
	gtk_table_attach_defaults(tableMirror, comboMirror, 1, 2, 0, 1);

	gtk_box_pack_start(GTK_BOX(mainBox), GTK_WIDGET(tableMirror), TRUE, TRUE, 0);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepAptMirror_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
