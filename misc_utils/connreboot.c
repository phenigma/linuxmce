#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/reboot.h>

#define PORT 100

int main()
{
	int s;
	struct sockaddr_in saddr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = INADDR_ANY;

	s = socket(PF_INET, SOCK_STREAM, 0);
	bind(s, (struct sockaddr *) &saddr, sizeof(saddr));
	listen(s, 5);
	accept(s, NULL, NULL);
	reboot(RB_AUTOBOOT);
	
	return 0;
}
