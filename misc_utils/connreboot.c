#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/reboot.h>
#include <string.h>
#include <stdio.h>

#define PORT 100

int main()
{
	int s, s2;
	struct sockaddr_in saddr;
	int do_reboot = 0;
	char buffer[1024], cmd[1024];
	int bytes;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = INADDR_ANY;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		perror("socket");
		return 1;
	}

	if (bind(s, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
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
				printf("%s", cmd+4);
		}
		
		close(s2);
	}
	reboot(RB_AUTOBOOT);
	
	return 0;
}
