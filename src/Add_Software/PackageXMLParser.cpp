#include "PackageXMLParser.h"

#include <algorithm>

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"

//--------------------------------------------------------------------------------------------------
namespace DCE
{
	Logger *g_pPlutoLogger;
}
//--------------------------------------------------------------------------------------------------
PackageXMLParser::PackageXMLParser() : m_CurrentNode(niNone)
{
}
//--------------------------------------------------------------------------------------------------
PackageXMLParser::~PackageXMLParser()
{
	ProcessPackage();

	xmlCleanupParser();
	xmlMemoryDump();
}
//--------------------------------------------------------------------------------------------------
bool PackageXMLParser::ProcessNode(xmlTextReaderPtr reader) 
{
	const char *name, *value;

	name = (char *)xmlTextReaderConstName(reader);
	if(name == NULL)
		return false;

	value = (char *)xmlTextReaderConstValue(reader);

	string sName(name);
	int depth = xmlTextReaderDepth(reader);
	int type = xmlTextReaderNodeType(reader);

	if(sName == "package")
	{
		ProcessPackage();
	}
	else if(sName == "#text")
	{
		if(NULL != value && m_CurrentNode != niNone && type == XML_READER_TYPE_TEXT)
            m_mapCurrentPackageInfo[m_CurrentNode] = value;
	}
	else
	{
		m_CurrentNode = niNone;
		for(int i = 0; i < niNumNodes; ++i)
		{
			if(NodeName[i] == sName)
			{
				m_CurrentNode = (NodeIndex)i;
			}
		}
	}

	return true;
}
//--------------------------------------------------------------------------------------------------
void PackageXMLParser::ProcessPackage()
{
	if(!m_mapCurrentPackageInfo[niTitle].empty() && !m_mapCurrentPackageInfo.empty())
	{
		m_listPackages.push_back(m_mapCurrentPackageInfo);
		m_mapCurrentPackageInfo.clear();
		m_CurrentNode = niNone;
	}
}
//--------------------------------------------------------------------------------------------------
void PackageXMLParser::Parse(string sXmlData)
{
	xmlTextReaderPtr Reader;
	Reader = xmlReaderForMemory(sXmlData.c_str(), static_cast<int>(sXmlData.length()), NULL, NULL, 0);

	int ret;

	if (Reader != NULL) 
	{
		ret = xmlTextReaderRead(Reader);
		while (ret != -1)
		{
			if(!ProcessNode(Reader))
				ret = -1;
			else
				ret = xmlTextReaderRead(Reader);
		}
		
		xmlFreeTextReader(Reader);
		if (ret != 0) 
		{
			fprintf(stderr, "%s : failed to parse\n");
		}
	} 

	return;
}
//--------------------------------------------------------------------------------------------------
const list<PackageInfo>& PackageXMLParser::GetParsedData() const
{
	return m_listPackages;
}
//--------------------------------------------------------------------------------------------------
#ifdef UNIT_TEST_PARSER
int main()
{
	string sFileName = "/temp/packages.xml";

	string sXmlData;
	FileUtils::ReadTextFile(sFileName, sXmlData);
	sXmlData = StringUtils::Replace(sXmlData, "&", "&amp;");

	PackageXMLParser parser;
	parser.Parse(sXmlData);
	const list<PackageInfo>& listPackages = parser.GetParsedData();

	for(list<PackageInfo>::const_iterator it = listPackages.begin(); it != listPackages.end(); ++it)
		for(PackageInfo::const_iterator itp = it->begin(); itp != it->end(); ++itp)
			cout << NodeName[itp->first] << ": " << itp->second << endl;

	return 0;
}
#endif