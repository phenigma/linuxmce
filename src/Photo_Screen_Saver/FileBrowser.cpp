#include "FileBrowser.h"

#include "../PlutoUtils/StringUtils.h"
#include "../DCE/Logger.h"

#include<pthread.h>

using namespace DCE;

FileBrowser::FileBrowser(string FileList)
{
	//FileList = "/IMG_0214.jpg\n/IMG_0001.jpg";
	StringUtils::Tokenize(FileList,"\r\n",m_vectFileList);
	srand( (unsigned)time( NULL ) );
}

FileBrowser::~FileBrowser(void)
{
}

string FileBrowser::NextFile()
{
	string Result;
	if(!m_vectFileList.size())
		return Result;

	if(m_vectFileList.size() == 1)
		return m_vectFileList[0];
		
	int Position = -1;
	do {
#ifdef DEBUG
		int r = rand();
		Position = int(r % m_vectFileList.size());
		g_pPlutoLogger->Write(LV_ALARM, "Purpose file rand %d Position %d", r, Position);
#else
		Position = int(rand() % m_vectFileList.size());
#endif
		Result = m_vectFileList[Position];
	} while(Result == LastFile);
	LastFile = Result;

	g_pPlutoLogger->Write(LV_ALARM, "Purpose file Position %d vect size %d file %s", Position, (int) m_vectFileList.size(), Result.c_str());

	return Result;
}
