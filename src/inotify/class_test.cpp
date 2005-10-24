#include "inotify_class.h"

#include <unistd.h>

#include <iostream>
using namespace std;

int main()
{
	inotify obj;

	int wd1 = obj.watch("/tmp", IN_ALL_EVENTS);
	int wd2 = obj.watch("/", IN_ALL_EVENTS);
	obj.unwatch(wd2);
	while (! obj.pending_events()) sleep(1);
	cout << "Pending events" << endl;
	while (1)
	{
		cpp_inotify_event event = obj.get_event();

		cout << "Event: " << event.tostring() << endl;
	}
}
