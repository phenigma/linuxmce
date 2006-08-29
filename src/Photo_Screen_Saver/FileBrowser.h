#pragma once

#include <string>
using namespace std;

class FileBrowser
{
	bool FirstFile;
public:
	FileBrowser(string SearchFolder);
	~FileBrowser(void);

	string NextFile();
};
