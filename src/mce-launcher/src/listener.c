#include "listener.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

listener_args *data;

void listener_create_pipe(void) {

}

void* listener_thread(void *args) {
	int err;
	gchar* buff;

	GIOChannel* iochan;

	/* fetch passed arguments */
	data = (listener_args *)args;

	/* create a pipe file */
	err = mkfifo(LISTENER, 0666);
	if (err == -1 && err != EEXIST) {
		printf("ERROR: Cannot create communication pipe.\n");
	}

	iochan = g_io_channel_new_file(LISTENER, "r", NULL);
	while ( 1 ) {

		err = g_io_channel_read_line(iochan, &buff, NULL, NULL, NULL);

		if (err != G_IO_STATUS_NORMAL || buff == NULL) {
			continue;
		}

		printf("Got : %s\n", buff);
		g_free(buff);

	}
	printf("Nononono\n");

	gdk_threads_enter();
	gtk_label_set_text(GTK_LABEL(data->labelMain), "Here i am");
	gdk_threads_leave();

	return NULL;
}
