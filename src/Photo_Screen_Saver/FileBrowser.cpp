#include "FileBrowser.h"

#include "../PlutoUtils/FileUtils.h"

FileBrowser::FileBrowser(string SearchFolder)
{
	FirstFile = true;
#ifdef WIN32
	this->SearchFolder = "/usr/pluto/orbiter/skins/Basic/menu2";
#endif
	AddExtension("png");
	AddExtension("jpg");
	LastFile = "";
}

FileBrowser::~FileBrowser(void)
{
}

void FileBrowser::AddExtension(string Extension)
{
	list<string> MatchFiles;
	FileUtils::FindFiles(MatchFiles, SearchFolder, "*."+Extension, false, true);
	for(list<string>::iterator FileDesc = MatchFiles.begin(), 
		End = MatchFiles.end(); FileDesc != End; FileDesc++)
	{
		string FileName = *FileDesc;
		FileList.push_back(FileName);
	}


}

string FileBrowser::NextFile()
{
	string Result;
	if(!FileList.size())
		return Result;

	if(FileList.size() == 1)
		return FileList[0];
		
	int Position = -1;
	do {
		Position = rand() % FileList.size();
		Result = FileList[Position];
	} while(Result == LastFile);
	LastFile = Result;

	return Result;
}