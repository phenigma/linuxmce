/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "common.h"

#include <string.h>
#include <malloc.h>

// TODO: beautify code

#define BRIGHT "[1m"
#define COLOR "[1;33m"
#define NORMAL "[0m"

#if 0
#define MSG "Please wait while MD filesystem is created. System will reboot when done"
#define FIL "************************************************************************"
#else
#define MSG "We announced ourselves to the router."
#define FIL "*************************************"
#endif

#define MSG2 "There are multiple DeviceTemplates available for this Device."
#define MSG3 "Please go to an existing orbiter and make a selection.       "
#define FIL2 "*************************************************************"

void DisplayPleaseWait()
{
	printf(BRIGHT "**" FIL "**" NORMAL "\n");
	printf(BRIGHT "* " COLOR MSG NORMAL BRIGHT " *" NORMAL "\n");
	printf(BRIGHT "**" FIL "**" NORMAL "\n");
}
void DisplayPleaseChoose()
{
	printf(BRIGHT "**" FIL2 "**" NORMAL "\n");
	printf(BRIGHT "* " COLOR MSG2 NORMAL BRIGHT " *" NORMAL "\n");
	printf(BRIGHT "* " COLOR MSG3 NORMAL BRIGHT " *" NORMAL "\n");
	printf(BRIGHT "**" FIL2 "**" NORMAL "\n");
}

// ./interactor <Gateway> <IP> <MAC>
int main(int argc, char * argv[])
{
	int s, s2, s_srv;
	struct sockaddr_in saddr_client, saddr_server;
	int do_reboot = 0;
	char buffer[1024], cmd[1024];
	int bytes, tmp;
	const char * Gateway, * myIP, * myMAC, * myArchitecture, * myDT_VendorModelID;
	const char * myCommand;

	if (argc == 7)
	{
		myCommand = argv[6];
	}
	else if (argc != 6)
	{
		printf("(Interactor) Wrong number of arguments: %d\n", argc);
		return 1;
	}
	else
		myCommand = "newmd";

	Gateway = argv[1];
	myIP = argv[2];
	myMAC = argv[3];
	myArchitecture = argv[4];
	myDT_VendorModelID = argv[5];

	saddr_client.sin_family = AF_INET;
	saddr_client.sin_port = htons(INTERACTOR_PORT);
	inet_aton(Gateway, &saddr_client.sin_addr);

	saddr_server.sin_family = AF_INET;
	saddr_server.sin_port = htons(INTERACTOR_PORT);
	saddr_server.sin_addr.s_addr = INADDR_ANY;

	// Start interactor server on client for commands
	s_srv = socket(PF_INET, SOCK_STREAM, 0);
	if (s_srv == -1)
	{
		perror("socket2");
		return 1;
	}

	set_close_on_exec(s_srv);

	if (bind(s_srv, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) == -1)
	{
		perror("bind");
		return 1;
	}
	
	if (listen(s_srv, 5) == -1)
	{
		perror("listen");
		return 1;
	}
	
	// Connect to the interactor server on the Core
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		perror("socket1");
		return 1;
	}
	
	tmp = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)) == -1)
	{
		perror("setsockopt");
		return 1;
	}

	if (connect(s, (struct sockaddr *) &saddr_client, sizeof(saddr_client)) == -1)
	{
		perror("connect");
		return 1;
	}

	bytes = snprintf(buffer, 1024, "%s %s %s 9|1|112|%s %s\n", myCommand, myIP, myMAC, myArchitecture, myDT_VendorModelID);
	write(s, buffer, bytes < 1024 ? bytes : 1024);

	printf("%s\n", buffer);

	close(s);

	// Do the actual server stuff
	s = s_srv;

	DisplayPleaseWait();

	if (strcmp(myCommand, "select") == 0)
		DisplayPleaseChoose();

	while (! do_reboot)
	{
		s2 = accept(s, NULL, NULL);
		if (s2 == -1)
		{
			perror("accept");
			return 1;
		}
		set_close_on_exec(s2);

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
			else if (strcmp(cmd, "deviceid") == 0)
			{
				// NOOP
				// Server reported back to us our ID
				// TODO: write deviceid to PK_Device in /etc/pluto.conf for disked based booting
			}
		}
		
		close(s2);
	}
	reboot(RB_AUTOBOOT);
	
	return 0;
}
