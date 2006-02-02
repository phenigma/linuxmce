#ifndef WIN32
	#include "inotify.h"
#else
	#define __s32 long
	#define __u32 unsigned long
#endif

#include <string>
#include <list>
using namespace std;

class cpp_inotify_event
{
	public:
		__s32 wd;
		__u32 mask;
		string name;

		string tostring();
};

static const int inotify_buffer_size = 4096;

class inotify
{
	private:
		int inotify_fd;
		list<cpp_inotify_event> event_queue;

		void convert_buffer(char * buffer, int bytes);
		
	public:
		inotify();
		~inotify();
		int watch(string path, __u32 mask);
		void unwatch(int watch_id);
		bool pending_events();
		cpp_inotify_event get_event();
};
