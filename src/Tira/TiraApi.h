#ifndef TiraApi_h
#define TiraApi_h

// Derived from TiraHost.cpp

#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>


static int lastKeyRead = 0;
static char ReadLine[2];
int kbhit() { 
	
	int res = read (0, ReadLine, 1);
	if (res)		
		return 1; 
	else
		return 0;
};
int getche() { 
	return ReadLine[0]; 
}
#define STR_W(S) S
#define TIRA_LIBRARY	"obj/TiraAPI.so"
typedef	char SCHAR;
#define STRCPY   strcpy
#define __stdcall	
typedef void* HMODULE;

int GetLastError() {
	return errno;	
};   

void* LoadLibrary(const char* LibName) {
	void* res = dlopen(LibName, RTLD_NOW);
	if ( res == 0 )
		printf(dlerror());
	return res;
};
bool FreeLibrary(void* handle) {
	return dlclose(handle) == 0;	
};
void* GetProcAddress(void* handle, const char* Name){
	return dlsym(handle,Name);	
};
void Sleep(int n) {
	usleep(1000 * n);   
};


	static SCHAR LibName[200];
//---------------------------------------------------------------------------

int Error() {

   printf("Last Error returned : %d\n", GetLastError() );
   return 1;
};

int EmptyFunc() {
	printf("API Library not loaded\n");
	return 1;
};
typedef int (__stdcall * tira_six_byte_cb) (const char * eventstring);

typedef int (__stdcall * t_tira_init) ();
typedef int (__stdcall * t_tira_cleanup) ();
typedef int (__stdcall * t_tira_set_handler) (tira_six_byte_cb cb);
typedef int (__stdcall * t_tira_start) ( int PortID);
typedef int (__stdcall * t_tira_stop) ();
typedef int (__stdcall * t_tira_start_capture) ();
typedef int (__stdcall * t_tira_cancel_capture) ();
typedef int (__stdcall * t_tira_get_captured_data)
                   (const unsigned char** data, int* size );;
typedef int (__stdcall * t_tira_transmit)
       (int repeat, int frequency, const unsigned char* data, const int dataSize );
typedef int (__stdcall * t_tira_delete)(const unsigned char* ptr);

typedef int (__stdcall * t_tira_set_dword)(unsigned int addr, unsigned int val);

t_tira_init               p_tira_init = (t_tira_init)EmptyFunc;
t_tira_cleanup            p_tira_cleanup = (t_tira_cleanup)EmptyFunc;;
t_tira_set_handler        p_tira_set_handler = (t_tira_set_handler)EmptyFunc;;
t_tira_start              p_tira_start = (t_tira_start)EmptyFunc;;
t_tira_stop               p_tira_stop = (t_tira_stop)EmptyFunc;;
t_tira_start_capture      p_tira_start_capture = (t_tira_start_capture)EmptyFunc;;
t_tira_cancel_capture     p_tira_cancel_capture = (t_tira_cancel_capture)EmptyFunc;;
t_tira_get_captured_data  p_tira_get_captured_data = (t_tira_get_captured_data)EmptyFunc;;
t_tira_transmit           p_tira_transmit = (t_tira_transmit)EmptyFunc;;
t_tira_delete             p_tira_delete = (t_tira_delete)EmptyFunc;;
t_tira_set_dword          p_tira_set_dword = (t_tira_set_dword) EmptyFunc;

int __stdcall OurCalback(const char * eventstring) {
   printf("IR Data %s\n", eventstring);
   return 0;
};

static HMODULE handle = 0;
int LoadTiraDLL();
int UnloadTiraDLL();

#endif
