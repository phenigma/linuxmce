#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>

int main()
{
	const struct timeval tv_val = { .tv_sec = 10, .tv_usec = 0 };
	struct timeval tv = tv_val;
	fd_set fd;
	char buf[1024];

	FD_ZERO(&fd);
	FD_SET(0, &fd);
	while (select(1, &fd, 0, 0, &tv) > 0)
	{
		read(0, buf, 1024);
		FD_ZERO(&fd);
		FD_SET(0, &fd);
		tv = tv_val;
	}

	return 0;
}
