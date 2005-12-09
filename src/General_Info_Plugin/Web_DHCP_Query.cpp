#include "Web_DHCP_Query.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

extern "C"
{
#include <http_fetcher.h>
}

using namespace DCE;

namespace nsWeb_DHCP_Query
{
	void Web_DHCP_Query::Query(Web_DHCP_Query_Params Params, Web_DHCP_Query_Result & Result)
	{
		Result.clear();

		// build HTTP GET query string
		Web_DHCP_Query_Params::iterator it;
		string sQueryString = "";
		if (Params.size() != 0)
		{
			it = Params.begin();
			sQueryString = QueryString_Pair(it->first, it->second);
			it++;
			for (; it != Params.end(); it++)
			{
				sQueryString += "&" + QueryString_Pair(it->first, it->second);
			}
		}
		
		char * buffer = NULL;
		string sURL = m_sURL_Base + "?" + sQueryString;
		int iResult = http_fetch(sURL.c_str(), &buffer);
		
		buffer = (char *) realloc(buffer, iResult + 1); // http_fetch doesn't store a \0 in the end
		buffer[iResult] = 0;
		
		if (iResult == -1)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Error fetching page: %s", http_strerror());
			return;
		}
		else
		{
			Web_DHCP_Query_Row Rows;
			string sBuffer = buffer;
			free(buffer); // allocated by http_fetch
			StringUtils::Tokenize(sBuffer, "\r\n", Rows);

			for (size_t i = 0; i < Rows.size(); i++)
			{
				Web_DHCP_Query_Row Fields;
				StringUtils::Tokenize(Rows[i], "\t", Fields);
				Result.push_back(Fields);
			}
		}
	}

	string Web_DHCP_Query::QueryString_Pair(const string & sVar, const string & sValue)
	{
		return StringUtils::URLEncode(sVar) + "=" + StringUtils::URLEncode(sValue);
	}
}
