#include "DatabaseHelper.h"
#include "IconDecoder.h"

#include "../DCE/Logger.h"
#include "../pluto_main/Table_Device.h"
#include "../pluto_main/Table_Software.h"
//--------------------------------------------------------------------------------------------------
using namespace DCE;
//--------------------------------------------------------------------------------------------------
DatabaseHelper::DatabaseHelper(string sHost, string sUser, string sPasswd, string sDB, int nPort)
{
	m_pDatabase_pluto_main = new Database_pluto_main(g_pPlutoLogger);

	if(!m_pDatabase_pluto_main->Connect(sHost, sUser, sPasswd, sDB, nPort))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		return;
	}
}
//--------------------------------------------------------------------------------------------------
DatabaseHelper::~DatabaseHelper()
{
	delete m_pDatabase_pluto_main;
	m_pDatabase_pluto_main = NULL;
}
//--------------------------------------------------------------------------------------------------
/*static*/ string DatabaseHelper::GetValue(const PackageInfo& package, NodeIndex key)
{
	PackageInfo::const_iterator it = package.find(key);
	return it != package.end() ? StringUtils::SQLEscape(it->second) : string();
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::ProcessPackages(const list<PackageInfo>& listPackages)
{
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows(
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"WHERE FK_DeviceCategory IN (6,7,8) AND IPaddress is not NULL and IPaddress <> \'\'",
		&vectRow_Device);

	for(list<PackageInfo>::const_iterator it = listPackages.begin(); it != listPackages.end(); ++it)
	{
		const PackageInfo& package = *it;

		string sPackageName = GetValue(package, niPackagename);
		string sVersion = GetValue(package, niVersion);

		if(sPackageName.empty() || sVersion.empty())
			continue;

		for(vector<Row_Device *>::iterator it = vectRow_Device.begin(); it != vectRow_Device.end(); ++it)
		{
			Row_Device *pRow_Device = *it;

			switch(GetPackageStatus(package, pRow_Device->PK_Device_get()))
			{
				case psNotInstalled:
					InstallPackage(package, pRow_Device->PK_Device_get());
					break;

				case psNotUpToDate:
					UpdatePackage(package, pRow_Device->PK_Device_get());
					break;

				case psUpToDate:
					break;

				default:
					break;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------
PackageStatus DatabaseHelper::GetPackageStatus(const PackageInfo& package, int DeviceID)
{
	string sPackageName = GetValue(package, niPackagename);
	string sVersion = GetValue(package, niVersion);

	vector<Row_Software *> vectRow_Software;
	m_pDatabase_pluto_main->Software_get()->GetRows(
		"WHERE PackageName = \"" + sPackageName + "\" AND FK_Device = " + StringUtils::ltos(DeviceID),
		&vectRow_Software);

	if(vectRow_Software.size() >= 1)
	{
		Row_Software *pRow_Software = vectRow_Software[0];
		string sCurrentVersion = pRow_Software->Version_get();
		if(sCurrentVersion != sVersion)
		{
			g_pPlutoLogger->Write(LV_STATUS, "Package %s for device %d not to the latest version %s",
				sPackageName.c_str(), DeviceID, sVersion.c_str());
			
			return psNotUpToDate;
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Package %s for device %d already to the latest version %s",
				sPackageName.c_str(), DeviceID, sVersion.c_str());
			
			return psUpToDate;
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "Package %s for device %d not installed!",
			sPackageName.c_str(), DeviceID);

		return psNotInstalled;
	}

	return psNone;
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::UpdatePackage(const PackageInfo& package, int DeviceID)
{
	string sPackageName = GetValue(package, niPackagename);
	g_pPlutoLogger->Write(LV_WARNING, "Updating package %s for device %d", sPackageName.c_str(), DeviceID);

	vector<Row_Software *> vectRow_Software;
	m_pDatabase_pluto_main->Software_get()->GetRows(
		"WHERE PackageName = \"" + sPackageName + "\" AND FK_Device = " + StringUtils::ltos(DeviceID),
		&vectRow_Software);

	if(vectRow_Software.size() >= 1)
	{
		Row_Software *pRow_Software = vectRow_Software[0];
		pRow_Software->Description_set(GetValue(package, niDescription));
		pRow_Software->Title_set(GetValue(package, niTitle));
		pRow_Software->HomeURL_set(GetValue(package, niHomeurl));
		pRow_Software->Category_set(GetValue(package, niCategory));
		pRow_Software->Downloadurl_set(GetValue(package, niDownloadurl));
		pRow_Software->PackageName_set(GetValue(package, niPackagename));
		pRow_Software->RepositoryName_set(GetValue(package, niRepositoryname));
		pRow_Software->Misc_set(GetValue(package, niMisc));
		pRow_Software->Version_set(GetValue(package, niVersion));
		pRow_Software->Target_set(GetValue(package, niTarget));
		pRow_Software->Importance_set(GetValue(package, niImportance));
		pRow_Software->PC_Type_set(GetValue(package, niPC_Type));
		pRow_Software->Required_Version_Min_set(GetValue(package, niRequired_Version_Min));
		pRow_Software->Required_Version_Max_set(GetValue(package, niRequired_Version_Max));
		pRow_Software->FK_Device_set(DeviceID);
		pRow_Software->Table_Software_get()->Commit();

		size_t unIconSize = 0;
		char *pIconData = DecodeIcon(GetValue(package, niIcon), unIconSize);
		SaveRawIcon(pRow_Software->PK_Software_get(), pIconData, unIconSize);
	}
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::InstallPackage(const PackageInfo& package, int DeviceID)
{
	string sPackageName = GetValue(package, niPackagename);
	g_pPlutoLogger->Write(LV_WARNING, "Installing package %s for device %d", sPackageName.c_str(), DeviceID);

	Row_Software *pRow_Software = m_pDatabase_pluto_main->Software_get()->AddRow();

	pRow_Software->Description_set(GetValue(package, niDescription));
	pRow_Software->Title_set(GetValue(package, niTitle));
	pRow_Software->HomeURL_set(GetValue(package, niHomeurl));
	pRow_Software->Category_set(GetValue(package, niCategory));
	pRow_Software->Downloadurl_set(GetValue(package, niDownloadurl));
	pRow_Software->PackageName_set(GetValue(package, niPackagename));
	pRow_Software->RepositoryName_set(GetValue(package, niRepositoryname));
	pRow_Software->Misc_set(GetValue(package, niMisc));
	pRow_Software->Version_set(GetValue(package, niVersion));
	pRow_Software->Target_set(GetValue(package, niTarget));
	pRow_Software->Importance_set(GetValue(package, niImportance));
	pRow_Software->PC_Type_set(GetValue(package, niPC_Type));
	pRow_Software->Required_Version_Min_set(GetValue(package, niRequired_Version_Min));
	pRow_Software->Required_Version_Max_set(GetValue(package, niRequired_Version_Max));
	pRow_Software->FK_Device_set(DeviceID);
	pRow_Software->Table_Software_get()->Commit();

	size_t unIconSize = 0;
	char *pIconData = DecodeIcon(GetValue(package, niIcon), unIconSize);
	SaveRawIcon(pRow_Software->PK_Software_get(), pIconData, unIconSize);
}
//--------------------------------------------------------------------------------------------------
void DatabaseHelper::SaveRawIcon(int nPK_Software, char *pData, size_t nSize)
{
	string sHeadQuery = "UPDATE `Software` set Iconstr = \"";
	string sEndQuery = "\" WHERE PK_Software = " + StringUtils::ltos(nPK_Software);

	int nGoodDataSize = 0;
	char *pGoodData = new char[nSize * 2 + 1];

	size_t length = nGoodDataSize = mysql_real_escape_string(m_pDatabase_pluto_main->m_pMySQL, pGoodData, pData, (unsigned long)nSize);

	delete [] pData;
	pData = NULL;

	length += sHeadQuery.length();
	length += sEndQuery.length();

	char *pQueryData = new char[length];
	memcpy(pQueryData, sHeadQuery.c_str(), sHeadQuery.length());
	char *pCurrentPos = pQueryData + sHeadQuery.length();
	memcpy(pCurrentPos, pGoodData, nGoodDataSize);
	pCurrentPos += nGoodDataSize;
	memcpy(pCurrentPos, sEndQuery.c_str(), sEndQuery.length());
	delete [] pGoodData;

	if(mysql_real_query(m_pDatabase_pluto_main->m_pMySQL, pQueryData, (unsigned long)length))
	{
		g_pPlutoLogger->Write(LV_STATUS, "Failed to save icon in db!");
	}

	delete [] pQueryData;
}
//--------------------------------------------------------------------------------------------------
#ifdef UNIT_TEST_DATABASE_HELPER
int main()
{
	g_pPlutoLogger = new FileLogger(stdout);

	string sFileName = "/temp/packages.xml";

	string sXmlData;
	FileUtils::ReadTextFile(sFileName, sXmlData);
	sXmlData = StringUtils::Replace(sXmlData, "&", "&amp;");

	PackageXMLParser parser;
	parser.Parse(sXmlData);
	const list<PackageInfo>& listPackages = parser.GetParsedData();

	DatabaseHelper dbhelper("10.0.0.150", "root", "", "pluto_main_devel");
	dbhelper.ProcessPackages(listPackages);

	return 0;
}
#endif