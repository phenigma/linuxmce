#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SCRIPT_NAME "./HelperScript.sh"
#define SCRIPT_IN 100
#define SCRIPT_OUT 101

int main(void) {
	int manager2script[2];
	int script2manager[2];

	// Init write pipe
	if (pipe(manager2script) == -1) {
		perror("pipe write");
		return(1);
	}

	// Init read pipe
	if (pipe(script2manager) == -1) {
		perror("pipe read");
		return(1);
	}

	// Fork
	int script_pid = fork();
	if (script_pid < 0 ) {
		perror("fork");
		return(1);
	}

	// Run child script
	if (script_pid > 0) {
		close(manager2script[1]);  // Close input pipe fd
		dup2(manager2script[0],SCRIPT_IN); // Make stdin of this process be the same thing as the output of this pipe
		close(manager2script[0]);  // Close output pipe fd

		close(script2manager[0]);  // Close ouput pipe fd
		dup2(script2manager[1],SCRIPT_OUT); // Make stdout of the process be the same thing as the input of the pipe
		close(script2manager[1]);  // Close input pipe fd

		execl(SCRIPT_NAME, SCRIPT_NAME, NULL);

		_exit(0);
	}

	// Run Application
	close(manager2script[0]);
	close(script2manager[1]);
	int sin, sout;
	sin = script2manager[0];
	sout = manager2script[1];

	char message[255];
	char cmd[255];

	strcpy(cmd, "DOWNLOAD 1 http://mirror.home-dn.net/debian-multimedia/pool/main/3/3gpwiz/3gpwiz_1.3-0.3_all.deb 98035251c140ef4168252004bf37dfa0\n");
	write(sout, cmd, strlen(cmd));
	memset(message, 0, 255);
	read(sin, &message, 255);
	printf("GOT: %s", message);


	strcpy(cmd, "APPLY 1 http://mirror.home-dn.net/debian-multimedia/pool/main/3/3gpwiz/3gpwiz_1.3-0.3_all.deb deb\n");
	write(sout, cmd, strlen(cmd));
	memset(message, 0, 255);
	read(sin, &message, 255);
	printf("GOT: %s", message);

}
