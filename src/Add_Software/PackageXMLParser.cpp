#include "PackageXMLParser.h"

#include <algorithm>

#define UNIT_TEST_PARSER
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
void PackageXMLParser::ProcessNode(xmlTextReaderPtr reader) 
{
	const char *name, *value;

	name = (char *)xmlTextReaderConstName(reader);
	if(name == NULL)
		return;

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
void PackageXMLParser::Parse(string sFileName)
{
	xmlTextReaderPtr Reader;
	Reader = xmlReaderForFile(sFileName.c_str(), NULL, 0);

	int ret;

	if (Reader != NULL) 
	{
		ret = xmlTextReaderRead(Reader);
		while (ret != -1)
		{
			ProcessNode(Reader);
			ret = xmlTextReaderRead(Reader);
		}
		
		xmlFreeTextReader(Reader);
		if (ret != 0) 
		{
			fprintf(stderr, "%s : failed to parse\n", sFileName.c_str());
		}
	} 
	else 
	{
		fprintf(stderr, "Unable to open %s\n", sFileName.c_str());
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

	PackageXMLParser parser;
	parser.Parse(sFileName);
	const list<PackageInfo>& listPackages = parser.GetParsedData();

	for(list<PackageInfo>::const_iterator it = listPackages.begin(); it != listPackages.end(); ++it)
		for(PackageInfo::const_iterator itp = it->begin(); itp != it->end(); ++itp)
			cout << NodeName[itp->first] << ": " << itp->second << endl;

	return 0;
}
#endif