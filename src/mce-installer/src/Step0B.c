#include "common.h"

#include <sys/types.h>
#include <unistd.h>
pthread_t progress_tid;
pid_t wget_pid;

GtkWidget *progress;
GtkWidget *labelSpeed;
GtkWidget *labelTotal;

void *progress_thread(void *arg) {
	gchar* buff;
	gchar** buffv;
	GIOChannel* iochan = NULL;
	int err;

	while (!iochan) {
		printf(".");
		iochan = g_io_channel_new_file("/tmp/mce-wget-progress", "r", NULL);
	}

	while (1) {
		err = g_io_channel_read_line(iochan, &buff, NULL, NULL, NULL);
                if (err != G_IO_STATUS_NORMAL || buff == NULL) {
			continue;
		printf("*");
		}
	
		buffv = g_strsplit(buff, "|", 3);
		if (buffv == NULL || buffv[0] == NULL || buffv[1] == NULL) { continue; }
	
		gdk_threads_enter();
		int done = atoi(buffv[0]);
		gtk_progress_bar_update(GTK_PROGRESS_BAR(progress), (gfloat)done/100);

		gchar *progress_bar_text=g_strdup_printf("Downloaded %s %%",buffv[0]);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), progress_bar_text);
		g_free(progress_bar_text);
		

		progress_bar_text=g_strdup_printf("Done : %s",buffv[1]);
		gtk_label_set_text(GTK_LABEL(labelTotal), progress_bar_text);
		g_free(progress_bar_text);

		progress_bar_text=g_strdup_printf("Speed : %s",buffv[2]);
		gtk_label_set_text(GTK_LABEL(labelSpeed), progress_bar_text);
		g_free(progress_bar_text);

		gdk_threads_leave();

		g_free(buff);
                g_strfreev(buffv);
	}
}

void start_wget(void) {
	wget_pid = fork();
	if (wget_pid > 0) {
		execl("./wget-wrapper.sh","wget-wrapper.sh",NULL);
	} else if (wget_pid < 0) {
		printf("ERROR: Cannot fork wget process\n");
	}
}

void on_Step0B_forward_clicked(GtkWidget *widget, gpointer data) {
}

void displayStep0B(void) {
	printf("Step0B\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Downloading Linux MCE CDROM");
	gtk_box_pack_start(GTK_BOX(mainBox), label, FALSE, FALSE, 0);
	
	// Progress Bar
	progress = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(mainBox), progress, FALSE, FALSE, 0);

	// Wizard text
	labelTotal = gtk_label_new_for_wizard ("Done: not available");
	gtk_box_pack_start(GTK_BOX(mainBox), labelTotal, FALSE, FALSE, 0);
	
	// Wizard text
	labelSpeed = gtk_label_new_for_wizard ("Speed: not available");
	gtk_box_pack_start(GTK_BOX(mainBox), labelSpeed, FALSE, FALSE, 0);

	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_Step0B_forward_clicked), NULL);

	start_wget();

	// Start Progressbar thread
	pthread_create(&progress_tid, NULL, progress_thread, NULL);

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);	
}
