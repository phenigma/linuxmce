#include "FileBrowser.h"

FileBrowser::FileBrowser(string SearchFolder)
{
	FirstFile = true;
}

FileBrowser::~FileBrowser(void)
{
}

string FileBrowser::NextFile()
{
	string Result;

	FirstFile = !FirstFile;
	if(FirstFile)
		Result = "/IMG_0001.jpg";
	else
		Result = "/IMG_0004.jpg";
	return Result;
}