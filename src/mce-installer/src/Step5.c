/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"


GtkWidget *buttonFinish;

void on_Step5_forward_clicked(GtkWidget *widget, gpointer data) {
}


void on_terminal_child_exited(VteTerminal *terminal, gpointer data) {
	gtk_widget_set_sensitive (GTK_WIDGET(buttonFinish), TRUE);
	displayStep6();
}


void displayStep5(void) {
	printf("Step5\n");

	int script_pid = -1;

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Installing base Linux MCE software");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	// Progress Bar
	GtkWidget *progressBar = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(mainBox), progressBar, TRUE, TRUE, 0);

	// Terminal widget
	GtkWidget *terminal = vte_terminal_new();
	gtk_box_pack_start(GTK_BOX(mainBox), terminal, TRUE, TRUE, 0);
	write_config_file();
	printf("Device Type : %d\n", setting_deviceType);
	if (setting_deviceType != DT_MEDIA_DIRECTOR) {	
		script_pid = vte_terminal_fork_command(VTE_TERMINAL(terminal),"./mce-installer-core.sh",NULL, NULL, "", FALSE, FALSE, FALSE);
	} else {
		script_pid = vte_terminal_fork_command(VTE_TERMINAL(terminal),"./mce-installer-diskedmd.sh",NULL,NULL, "", FALSE, FALSE, FALSE);
	}
	g_signal_connect(G_OBJECT(terminal),"child-exited", G_CALLBACK(on_terminal_child_exited), NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
