#ifndef __XML_FINDER_H__
#define __XML_FINDER_H__ 

#include <string>
#include <vector>
//--------------------------------------------------------------------------------------------------
class XMLFinder
{
public:

	static void FindXMLs(std::string sFilename, std::vector<std::string>& vectXmls);
	static void FindURLs(std::string sFilename, std::vector<std::string>& vectUrls);
};
//--------------------------------------------------------------------------------------------------
#endif //__XML_FINDER_H__
