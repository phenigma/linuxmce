#include "common.h"

static void remove_gtk_widget(GtkWidget *widget, gpointer data) {
	GtkWidget *container = (GtkWidget *) data;
	
	if (GTK_IS_CONTAINER(widget)) {
		gtk_container_foreach(GTK_CONTAINER(widget), remove_gtk_widget, widget);
	} 
	
	gtk_container_remove(GTK_CONTAINER(container), widget);	
}

void cleanupContainer (GtkWidget *container) {
	if (GTK_IS_CONTAINER(container)) {
		gtk_container_foreach(GTK_CONTAINER(container), remove_gtk_widget, container);
	}	
}

void on_back_clicked(GObject object, gpointer data) {
	if (!g_queue_is_empty(history) != 0) {
		gint prevScreen = (gint) g_queue_pop_head(history);
		switch (prevScreen){
			case STEP1A:
				displayStep1A();
				break;
			case STEP1B:
				displayStep1B();
				break;
			case STEP1C:
				displayStep1C();
				break;
			case STEP1D:
				displayStep1D();
				break;
			case STEP2A:
				displayStep2A();
				break;
			case STEP2C:
				displayStep2C();
				break;
			case STEP2E:
				displayStep2E();
				break;
			case STEP3:
				displayStep1A();
				break;
			default:
				printf("Unknown screen in history [%d]", prevScreen);
		}

		/*
		if (!g_ascii_strcasecmp(prevScreen,"Step1A")) {
			displayStep1A();
		} else if (!g_ascii_strcasecmp(prevScreen,"Setp1B")) {
			displayStep1B();
		} else if (!g_ascii_strcasecmp(prevScreen,"Step1C")) {
			displayStep1C();
		} else if (!g_ascii_strcasecmp(prevScreen,"Step1D")) {
			displayStep1D();
		} else {
			printf("Unknown screen in history [%s]\n", prevScreen);
		}
		*/
	} else {
		printf("No screen in history\n");
	}
}

gchar* detectCoreIpAddress(void) {
	printf("Searching for dcerouter's ip address...");
	struct sockaddr_in si_other;
	int s, slen=sizeof(si_other);
	int on=1;
	char buf[3]="xxx";
	gchar *ret = "";

	s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	fcntl(s, F_SETFL, O_NONBLOCK);
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	memset((char *) &si_other, sizeof(si_other), 0);
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(33333);

	inet_aton("255.255.255.255", &si_other.sin_addr);
	sendto(s, buf, 3, 0, &si_other, slen);

	time_t start_time = time(0);
	while ((time(0) - start_time) < 2) {
		recvfrom(s, buf, 3, 0, &si_other, &slen);
	}
	close(s);


	ret = inet_ntoa(si_other.sin_addr);
	if (g_ascii_strncasecmp(ret, "255.255.255.255",16)) {
		printf("   %s\n",ret);
		return ret;
	} else {
		printf("   not found\n");
		return NULL;
	}
}
  
