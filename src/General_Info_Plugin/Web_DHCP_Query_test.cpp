#include "Web_DHCP_Query.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

#include <iostream>

using namespace std;
using namespace DCE;
using namespace nsWeb_DHCP_Query;

int main()
{
	g_pPlutoLogger = new NullLogger();
	
	Web_DHCP_Query Query("http://10.0.0.175/plutohome-com/getRegisteredDevices.php");
	Web_DHCP_Query_Result Result1, Result2;
	Web_DHCP_Query_Params Params;

	Params["MAC"] = "11:22:11:22:11:22";
	Query.Query(Params, Result1);
	Params["PK"] = "24";
	Query.Query(Params, Result2);

	cout << "First result:" << endl;
	for (size_t i = 0; i < Result1.size(); i++)
	{
		for (size_t j = 0; j < Result1[i].size(); j++)
		{
			cout << Result1[i][j] << ",";
		}
		cout << endl;
	}
	
	cout << "Second result:" << endl;
	for (size_t i = 0; i < Result2.size(); i++)
	{
		for (size_t j = 0; j < Result2[i].size(); j++)
		{
			cout << Result2[i][j] << ",";
		}
		cout << endl;
	}

	return 0;
}
