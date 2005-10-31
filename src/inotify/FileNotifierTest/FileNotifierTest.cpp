#include "DCE/Logger.h"
#include "FileNotifier.h"

namespace DCE
{
    Logger *g_pPlutoLogger;
}
using namespace DCE;

void OnCreateFiles(const vector<string> &sFiles)
{
}

void OnDeleteFiles(const vector<string> &sFiles)
{
}

int main(int argc, char* argv[]) 
{
  g_pPlutoLogger = new FileLogger("FileNotifierTest.log");

  FileNotifier fileNotifier;
  fileNotifier.RegisterCallbacks((FileNotifierCallback *)&OnCreateFiles, (FileNotifierCallback *)&OnDeleteFiles);
  fileNotifier.Watch("/home/Chris/Test");

  while(1) //wait forever here. it's just a test.
  {
      Sleep(1000);
  }

  delete g_pPlutoLogger;

  return 0;
}