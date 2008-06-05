#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <resolv.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TIMEOUT 1

int   child_pid;
int   exit_status = -1;
int   port;
char* hostname;

void sig_alrm(int sig)
{
	fprintf(stderr, "Timeout error\n");
	kill(child_pid, 15);
	exit_status = 1;
}

void sig_chld(int signo) 
{
    int status;

    /* Wait for any child without blocking */
    if (waitpid(child_pid, &status, WNOHANG) < 0) 
    {
        fprintf(stderr, "waitpid failed\n");
        return;
    }

    if (WIFEXITED(status))
    {
        exit_status = WEXITSTATUS(status);
    }
}

int test_connection() {
	int sockfd;
	struct sockaddr_in dest;
	struct hostent *host;

	host = gethostbyname(hostname);
	if(host == NULL) {
		fprintf(stderr, "Gethostbyname error\n");
		return h_errno;
	}

	/* Open the socket */
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "Socket error\n");
		return errno;
	}

	/* Init host/port struct */
	bzero(&dest, sizeof(dest));
	memcpy((char *)&dest.sin_addr, (char *)host->h_addr, host->h_length);
	printf("%s\n", inet_ntoa(dest.sin_addr));
	dest.sin_family = AF_INET;
	dest.sin_port   = htons(port);

	/* Connect to server */
	if ( connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) != 0 ) {
		fprintf(stderr, "Connect error\n");
		return errno;
	}

	/* Close socket */
	fprintf(stderr, "Success\n");
	close(sockfd);
	return 0;
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
		return 1;
	}

	hostname = argv[1];
	port     = atoi(argv[2]);


	child_pid = fork();
	if (child_pid == 0 ) {
		return test_connection();
	} else if (child_pid < 0 ) {
		return 1;
	}

	signal(SIGALRM, (void *)sig_alrm);
	signal(SIGCHLD, (void *)sig_chld);
	alarm(TIMEOUT);
	
	while (exit_status == -1) {
		usleep(20000);
	}
	return exit_status;
}
