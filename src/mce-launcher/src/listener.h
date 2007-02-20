#include <gtk/gtk.h>

#define LISTENER  "/tmp/mce-launcher-pipe"

typedef struct {
	GtkWidget *labelMain;
	GtkWidget *progressMain;
} listener_args;

void* listener_thread (void *args);
