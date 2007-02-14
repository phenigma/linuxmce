#ifndef __DATABASE_HELPER_H__
#define __DATABASE_HELPER_H__
//--------------------------------------------------------------------------------------------------
#include "PackageXMLParser.h"
//--------------------------------------------------------------------------------------------------
class Database_pluto_main;
//--------------------------------------------------------------------------------------------------
enum PackageStatus
{
	psNone,
	psNotInstalled,
	psNotUpToDate,
	psUpToDate
};
//--------------------------------------------------------------------------------------------------
class DatabaseHelper
{
	Database_pluto_main	*m_pDatabase_pluto_main;

public:

	DatabaseHelper(string sHost = "dcerouter", string sUser = "root", string sPasswd = "", 
		string sDB = "pluto_main", int nPort = 3306);
	~DatabaseHelper();

	void ProcessPackages(const list<PackageInfo>& listPackages);

private:

	static string GetValue(const PackageInfo& package, NodeIndex key);
	
	PackageStatus GetPackageStatus(const PackageInfo& package, int DeviceID);
	void UpdatePackage(const PackageInfo& package, int DeviceID);
	void InstallPackage(const PackageInfo& package, int DeviceID);
	void SaveRawIcon(int nPK_Software, char *pData, size_t nSize);
};
//--------------------------------------------------------------------------------------------------
#endif //__DATABASE_HELPER_H__
