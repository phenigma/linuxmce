#include "Web_DHCP_Query.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

#include <iostream>

using namespace std;
using namespace DCE;
using namespace nsWeb_DHCP_Query;

int main()
{
	Web_DHCP_Query Query("http://10.0.0.175/plutohome-com/getRegisteredDevices.php");
	Web_DHCP_Query_Result Result1, Result2;
	Web_DHCP_Query_Params Params;

	Params["MAC"] = "ff:12:2a:4b:66:77";
	Query.Query(Params, Result1);
	Params["PK"] = "15";
	Query.Query(Params, Result2);

	for (int i = 0; i < Result1.size(); i++)
	{
		for (int j = 0; j < Result1[i].size(); j++)
		{
			cout << Result1[i][j] << ",";
		}
		cout << endl;
	}
	
	for (int i = 0; i < Result2.size(); i++)
	{
		for (int j = 0; j < Result2[i].size(); j++)
		{
			cout << Result2[i][j] << ",";
		}
		cout << endl;
	}
	
	return 0;
}
