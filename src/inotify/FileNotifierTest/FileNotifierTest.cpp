#include "DCE/Logger.h"
#include "inotify/FileNotifier.h"

namespace DCE
{
    Logger *g_pPlutoLogger;
}
using namespace DCE;

void OnCreateFiles(list<string> &listFiles)
{
	for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
	{
		string sItem = *it;
		g_pPlutoLogger->Write(LV_STATUS, "File/folder created: %s", sItem.c_str());	
	}
}

void OnDeleteFiles(list<string> &listFiles)
{
    for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
    {
        string sItem = *it;
        g_pPlutoLogger->Write(LV_STATUS, "File/folder deleted: %s", sItem.c_str());
    }
}

int main(int argc, char* argv[]) 
{
  g_pPlutoLogger = new FileLogger(stdout);
  g_pPlutoLogger->Write(LV_WARNING, "Starting...");

  FileNotifier fileNotifier;
  fileNotifier.RegisterCallbacks(OnCreateFiles, OnDeleteFiles);
  fileNotifier.Watch("/home/Chris/Test");
  fileNotifier.Run();

//  while(1) //wait forever here. it's just a test.
//  {
//      Sleep(1000);
//  }

  delete g_pPlutoLogger;

  return 0;
}

