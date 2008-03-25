#include <iostream>
#include "../FileUtils.h"

int main(int argc, char* argv[])
{
	string sTargetDir = "/home/test";
	string sFileSpec = "*.lock";

	cout << "Number of parameters specified: " << argc << endl;

	if(argc >= 3)
		sFileSpec = argv[2];

	if(argc >= 2)
		sTargetDir = argv[1];

	cout << "Target folder : '" << sTargetDir << "'" << endl;
	cout << "File spec : '" << sFileSpec << "'" << endl;
	cout << endl;

	list<string> listFiles;
	FileUtils::FindFiles(listFiles, sTargetDir, sFileSpec);	

	cout << "Files from folder per file spec: " << endl;
	for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); ++it)
	{
		cout << "\t" << *it << endl;
	}

	return 0;
}
