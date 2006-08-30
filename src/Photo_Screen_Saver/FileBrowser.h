#ifndef FileBrowser_H_
#define FileBrowser_H_

#include <string>
#include <vector>
using namespace std;

class FileBrowser
{
	string SearchFolder;
	bool FirstFile;
	vector <string> FileList;
	string LastFile;

	void AddExtension(string Extension);
public:
	FileBrowser(string SearchFolder);
	~FileBrowser(void);

	string NextFile();
};

#endif
