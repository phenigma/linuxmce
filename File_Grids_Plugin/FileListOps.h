#ifndef FileListOps_h
#define FileListOps_h

#include <list>
#include <string>
using namespace std;

class FileDetails
{
	public:
		string m_sBaseName;
		string m_sFileName;
		string m_sDescription;
		bool m_bIsDir;
		time_t m_tDate;

		FileDetails() : m_sBaseName(""), m_sFileName(""), m_sDescription("(not initialized)"), m_bIsDir(false), m_tDate(0) {}
		FileDetails(string sBaseName, string sFileName, bool bIsDir, time_t tDate, string sDesc = "")
			: m_sBaseName(sBaseName), m_sFileName(sFileName), m_sDescription(sDesc), m_bIsDir(bIsDir), m_tDate(tDate) {}
};

list <FileDetails> GetDirContents(string Path, string sPK_User, bool bSortByDate, string sValidExtensions_CSV = ".*", bool bIncludeParent=true);

#endif /* FileListOps_h */
