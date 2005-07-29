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
		int m_iDirNumber;
		time_t m_tDate;

		FileDetails() : m_sBaseName(""), m_sFileName(""), m_sDescription("(not initialized)"), m_bIsDir(false), m_tDate(0) {}
		FileDetails(string sBaseName, string sFileName, bool bIsDir, int iDirNumber, time_t tDate, string sDesc = "")
			: m_sBaseName(sBaseName), m_sFileName(sFileName), m_sDescription(sDesc), m_bIsDir(bIsDir), m_iDirNumber(iDirNumber), m_tDate(tDate) {}
};

void GetDirContents(list<FileDetails *> &listFileName,string Path, string sValidExtensions_CSV);

#endif /* FileListOps_h */
