#include "XMLFinder.h"

#include "../PlutoUtils/FileUtils.h"
#include "../DCE/Logger.h"

using namespace std;
using namespace DCE;
//--------------------------------------------------------------------------------------------------
string XMLFinder::FindURLToXML(string sFilename)
{
	const string csPackagesToken = "packages.xml";
	const string csHrefToken = "<a href=\"";
	const size_t cnNeighbourhood = 100;

	g_pPlutoLogger->Write(LV_WARNING, "Finding xml in html...");

	string sHTMLData;
	if(FileUtils::ReadTextFile(sFilename, sHTMLData))
	{
		size_t pos = sHTMLData.find(csPackagesToken);
		string sUsefullData = sHTMLData.substr(pos > cnNeighbourhood ? pos - cnNeighbourhood : 0, 
			cnNeighbourhood + csPackagesToken.length());

		string sUrlRawData =  sUsefullData.substr(
			sUsefullData.find(csHrefToken) + csHrefToken.length(), 
			sUsefullData.find(csPackagesToken) + csPackagesToken.length());

		return sUrlRawData;
	}

	return "";
}

//--------------------------------------------------------------------------------------------------
#ifdef UNIT_TEST_XML_FINDER

namespace DCE
{
	Logger *g_pPlutoLogger;
};

int main()
{
	g_pPlutoLogger = new FileLogger(stdout);

	string sURLToXML = XMLFinder::FindURLToXML("/temp/search.html");
	g_pPlutoLogger->Write(LV_WARNING, "Found xml : %s", sURLToXML.c_str());

	return 0;
}
#endif
