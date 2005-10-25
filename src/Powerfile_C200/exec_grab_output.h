#ifndef EXEC_GRAB_OUTPUT
#define EXEC_GRAB_OUTPUT

#include <string>
using namespace std;

// TODO: put this in PlutoUtils as a ExecWithOutput function or something
bool exec_output(const char * path, char * args[], string & sOutput);
#endif
