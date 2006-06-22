#include "common.h"

#include <string.h>
#include <malloc.h>

// TODO: beautify code

#define BRIGHT "[1m"
#define COLOR "[1;33m"
#define NORMAL "[0m"

#define MSG "Please wait while MD filesystem is created. System will reboot when done"
#define FIL "************************************************************************"

struct Room_t
{
	int PK_Room;
	char Description[1024]; // ugly
};

struct PlutoRooms_t
{
	int count;
	struct Room_t * Rooms;
};

void str_chr_replace(char * str, char oldch, char newch)
{
	int i = 0;

	for (i = 0; str[i] != 0; i++)
	{
		if (str[i] == oldch)
			str[i] = newch;
	}
}

void PopulatePlutoRooms(struct PlutoRooms_t * PlutoRooms, char * stream)
{
	char * next_spc;
	int room_idx = 0;

	sscanf(stream, "%d", &PlutoRooms->count);
	PlutoRooms->Rooms = NULL;
	if (PlutoRooms->count <= 0)
		return;

	PlutoRooms->Rooms = (struct Room_t *) calloc(PlutoRooms->count, sizeof(struct Room_t));

	next_spc = strchr(stream, ' ');
	while (next_spc)
	{
		stream = next_spc + 1;
		next_spc = strchr(stream, ' ');

		// \x01 -> field separator; \x02 -> convert to space
		char * field_separator = strchr(stream, '\x01');
		* field_separator = ' ';
		sscanf(stream, "%d", &PlutoRooms->Rooms[room_idx].PK_Room);
		field_separator++;
		if (* field_separator != ' ')
			sscanf(field_separator, "%s", PlutoRooms->Rooms[room_idx].Description);
		else
			* PlutoRooms->Rooms[room_idx].Description = 0;
		str_chr_replace(PlutoRooms->Rooms[room_idx].Description, '\x02', ' ');
		room_idx++;
	}
}

char * GetRoomName()
{
	int done = 0;
	static char buffer[1024];

	fgetc(stdin); // hack: there's a \n in the buffer
	while (!done)
	{
		printf("Room name: ");
		fflush(stdout);

		fgets(buffer, 1024, stdin);
		if (buffer[strlen(buffer) - 1] == '\n')
			buffer[strlen(buffer) - 1] = 0;
		if (strlen(buffer) != 0)
			done = 1;
	}

	return buffer;
}

void ChooseRoom(struct PlutoRooms_t * PlutoRooms, char ** RoomName, int * PK_Room)
{
	int i;
	int done = 0;
	int choice;

	while (! done)
	{
		printf("\n--> Choose a room:\n");
		for (i = 0; i < PlutoRooms->count; i++)
		{
			printf("%d. %s\n", i + 1, PlutoRooms->Rooms[i].Description);
		}
		printf("%s\n", "0. NEW ROOM");

		printf("Choice: ");
		fflush(stdout);
		if (scanf("%d", &choice) <= 0)
		{
			scanf("%*s"); // read the garbage that the user fed us, into /dev/null
			continue;
		}

		// validate choice
		if (choice == 0)
		{
			* RoomName = strdup(GetRoomName());
			* PK_Room = 0;
			done = 1;
		}
		else if (choice > 0 && choice <= PlutoRooms->count)
		{
			* RoomName = strdup(PlutoRooms->Rooms[choice - 1].Description);
			* PK_Room = PlutoRooms->Rooms[choice - 1].PK_Room;
			done = 1;
		}
	}
}

void DisplayPleaseWait()
{
	printf(BRIGHT "**" FIL "**" NORMAL "\n");
	printf(BRIGHT "* " COLOR MSG NORMAL BRIGHT " *" NORMAL "\n");
	printf(BRIGHT "**" FIL "**" NORMAL "\n");
}

// ./interactor <Gateway> <IP> <MAC>
int main(int argc, char * argv[])
{
	int s, s2;
	struct sockaddr_in saddr_client, saddr_server;
	int do_reboot = 0;
	char buffer[1024], cmd[1024];
	int bytes, tmp;
	const char * Gateway, * myIP, * myMAC;
	char * myRoom;

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

	// Ask for enumeration of rooms
	// <count> <PK_Room>\x01<Description> [<PK_Room>\x01<Description] [...]
	bytes = snprintf(buffer, 1024, "rooms\n");
	write(s, buffer, bytes < 1024 ? bytes : 1024);
	bytes = read(s, buffer, 1023);
	buffer[bytes] = 0;
	
	struct PlutoRooms_t PlutoRooms;
	PopulatePlutoRooms(&PlutoRooms, buffer);

	int PK_Room;
	ChooseRoom(&PlutoRooms, &myRoom, &PK_Room);

	bytes = snprintf(buffer, 1024, "newmd %s %s %d %s\n", myIP, myMAC, PK_Room, myRoom);
	write(s, buffer, bytes < 1024 ? bytes : 1024);
	
	close(s);

	// TODO: fix gap between sockets

	// Start interactor server on client for commands
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		perror("socket2");
		return 1;
	}

	set_close_on_exec(s);

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
	
	DisplayPleaseWait();
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
		}
		
		close(s2);
	}
	reboot(RB_AUTOBOOT);
	
	return 0;
}
