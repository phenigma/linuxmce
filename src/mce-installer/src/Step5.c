#include "common.h"


void on_Step5_forward_clicked(GtkWidget *widget, gpointer data) {
}

void displayStep5(void) {
	printf("Step5\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Installing base pluto software");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Progress Bar
	GtkWidget *progressBar = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(mainBox), progressBar, TRUE, TRUE, 0);

	// Terminal widget
	GtkWidget *terminal = vte_terminal_new();
	gtk_box_pack_start(GTK_BOX(mainBox), terminal, TRUE, TRUE, 0);
	write_config_file();	
	vte_terminal_fork_command(terminal,"./mce-installer-core.sh",NULL, NULL, "", FALSE, FALSE, FALSE);

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step5_forward_clicked), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
