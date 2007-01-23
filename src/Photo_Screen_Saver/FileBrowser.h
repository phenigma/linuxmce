#ifndef FileBrowser_H_
#define FileBrowser_H_

#include <string>
#include <vector>
using namespace std;

#include "DCE/Logger.h"

class FileBrowser
{
	pluto_pthread_mutex_t m_BrowserMutex;
	vector <string> m_vectFileList;
	string LastFile;

public:
	FileBrowser(string FileList);
	~FileBrowser(void);

	void RefreshFileList(string sFileList);
	string NextFile();
};

#endif
