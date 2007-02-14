#ifndef __PACKAGE_XML_PARSER_H__
#define __PACKAGE_XML_PARSER_H__
//--------------------------------------------------------------------------------------------------
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
#endif //__PACKAGE_XML_PARSER_H__

