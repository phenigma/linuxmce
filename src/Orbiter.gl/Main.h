#ifndef __MAIN_H__
#define __MAIN_H__

#include <string>
using namespace std;

struct CommandLineParams
{
  CommandLineParams()
      : sRouter_IP("dcerouter")
      , PK_Device(0)
      , PK_DeviceTemplate(0)
      , sLogger("stdout")
      , Width(800)
      , Height(600)
      , bFullScreen(false)
	  , bUseOpenGL(false)
      , sLocalDirectory("")
      , sNestedDisplay("")
      , bLocalMode(false)
    {
    }
  string sRouter_IP;
  int PK_Device;
  int PK_DeviceTemplate;
  string sLogger;
  int Width;
  int Height;
	bool bFullScreen;
	bool bUseOpenGL;
  string sLocalDirectory;
  string sNestedDisplay;
  bool bLocalMode;
};

bool ParseCommandLineParams(int argc, char* argv[], CommandLineParams &commandlineparams);

#endif
