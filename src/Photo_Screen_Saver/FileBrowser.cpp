#include "FileBrowser.h"

#include "../PlutoUtils/StringUtils.h"
#include "../DCE/Logger.h"

using namespace DCE;

FileBrowser::FileBrowser(string FileList)
{
	StringUtils::Tokenize(FileList,"\r\n",m_vectFileList);
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
		Position = int(rand() % m_vectFileList.size());
		Result = m_vectFileList[Position];
	} while(Result == LastFile);
	LastFile = Result;

	g_pPlutoLogger->Write(LV_ALARM, "Purpose file %s", Result.c_str());

	return Result;
}
