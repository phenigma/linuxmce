#ifndef FileBrowser_H_
#define FileBrowser_H_

#include <string>
#include <vector>
using namespace std;

class FileBrowser
{
	vector <string> m_vectFileList;
	string LastFile;

public:
	FileBrowser(string FileList);
	~FileBrowser(void);

	string NextFile();
};

#endif
