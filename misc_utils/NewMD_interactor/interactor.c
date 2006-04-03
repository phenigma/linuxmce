#include "common.h"

// ./interactor <Gateway> <IP> <MAC>
int main(int argc, char * argv[])
{
	int s, s2;
	struct sockaddr_in saddr_client, saddr_server;
	int do_reboot = 0;
	char buffer[1024], cmd[1024];
	int bytes, tmp;
	const char * Gateway, * myIP, * myMAC;

	if (argc != 4)
	{
		printf("(Interactor) Wrong number of arguments: %d\n", argc);
		return 1;
	}
	
	Gateway = argv[1];
	myIP = argv[2];
	myMAC = argv[3];

	saddr_client.sin_family = AF_INET;
	saddr_client.sin_port = htons(INTERACTOR_PORT);
	inet_aton(Gateway, &saddr_client.sin_addr);

	saddr_server.sin_family = AF_INET;
	saddr_server.sin_port = htons(INTERACTOR_PORT);
	saddr_server.sin_addr.s_addr = INADDR_ANY;

	// Connect to the interactor server on the Core
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		perror("socket1");
		return 1;
	}
	
	tmp = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp) == -1))
	{
		perror("setsockopt");
		return 1;
	}

	if (connect(s, (struct sockaddr *) &saddr_client, sizeof(saddr_client)) == -1)
	{
		perror("connect");
		return 1;
	}

	bytes = snprintf(buffer, 1024, "newmd %s %s\n", myIP, myMAC);
	write(s, buffer, bytes < 1024 ? bytes : 1024);
	
	close(s);
	
	// Start interactor server on client for commands
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		perror("socket2");
		return 1;
	}

	if (bind(s, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) == -1)
	{
		perror("bind");
		return 1;
	}
	
	if (listen(s, 5) == -1)
	{
		perror("listen");
		return 1;
	}
	
	while (! do_reboot)
	{
		s2 = accept(s, NULL, NULL);
		if (s2 == -1)
		{
			perror("accept");
			return 1;
		}

		while (! do_reboot && (bytes = read(s2, buffer, 1023)) > 0)
		{
			buffer[bytes] = 0;
			memset(cmd, 0, 1024);
			sscanf(buffer, "%s", cmd);
			if (strcmp(cmd, "reboot") == 0)
				do_reboot = 1;
			else if (strcmp(cmd, "msg") == 0)
				printf("%s", buffer+4);
			else if (strcmp(cmd, "shell") == 0)
			{
				printf("%s\n", "Running a shell");
				system("/bin/sh");
				printf("\n%s\n", "Shell exited");
			}
		}
		
		close(s2);
	}
	reboot(RB_AUTOBOOT);
	
	return 0;
}
