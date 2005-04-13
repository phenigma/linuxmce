#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>

int main()
{
	const struct timeval tv_val = { .tv_sec = 5, .tv_usec = 500000 };
	struct timeval tv = tv_val;
	fd_set fd;
	char buf[1024];
	int selret, old_school = -1, timeouts = 0;

	FD_ZERO(&fd);
	FD_SET(0, &fd);
	
	printf("This is the Pluto Remote Assistance shell\n");
	printf("It displays a dot every ~5.5 seconds and keeps the connection alive\n");
	while(timeouts < 3)
	{
		printf(".");
		fflush(stdout);

		if (old_school < 1)
		{
			selret = select(1, &fd, 0, 0, &tv);
			if (selret < 0)
			{
				break;
			}
			else if (selret > 0)
			{
				if (read(0, buf, 1024) <= 0)
				{
					break;
				}
				old_school = 0;
				timeouts = 0;
				select(0, 0, 0, 0, &tv);
			}
			else if (old_school == 0)
			{
				timeouts ++;
			}
			else
			{
				old_school = 1;
				printf("\n-- Old style Remote Assistance (no client keep-alive)\n");
			}
			FD_ZERO(&fd);
			FD_SET(0, &fd);
		}
		else
		{
			select(0, 0, 0, 0, &tv);
		}
		tv = tv_val;
	}

	if (! old_school)
		printf("\nThe other end failed to transmit the keep-alive character. Exiting\n");
	return 0;
}
