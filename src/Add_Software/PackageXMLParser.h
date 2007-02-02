#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
//--------------------------------------------------------------------------------------------------
#include <string>
#include <map>
#include <list>
#include <iostream>
using namespace std;
//--------------------------------------------------------------------------------------------------
enum NodeIndex
{
	niNone,
	niIcon,
	niTitle,
	niDescription,
	niHomeurl,
	niCategory,
	niDownloadurl,
	niRepositoryname,
	niPackagename,
	niMisc,
	niVersion,
	niTarget,
	niImportance,
	niPC_Type,
	niRequired_Version_Min,
	niRequired_Version_Max,
	niNumNodes
};
//--------------------------------------------------------------------------------------------------
string NodeName[] =
{
	"",
	"icon",
	"title",
	"description",
	"homeurl",
	"category",
	"downloadurl",
	"repositoryname",
	"packagename",
	"misc",
	"version",
	"target",
	"importance",
	"PC_Type",
	"Required_Version_Min",
	"Required_Version_Max"
};
//--------------------------------------------------------------------------------------------------
typedef map<NodeIndex, string> PackageInfo;
//--------------------------------------------------------------------------------------------------
class PackageXMLParser
{
private:

	PackageInfo m_mapCurrentPackageInfo;
	list<PackageInfo> m_listPackages;
	NodeIndex m_CurrentNode;
	
	void ProcessPackage();
	bool ProcessNode(xmlTextReaderPtr reader);

public:

	PackageXMLParser();
	~PackageXMLParser();
	void Parse(string sXmlData);
	const list<PackageInfo>& GetParsedData() const;
};
//--------------------------------------------------------------------------------------------------
