#ifndef DCEConfig_H
#define DCEConfig_H

#include <string>
#include <map>

using namespace std;

class DCEConfig
{
public:
	// serialized values
	string m_sDBHost,m_sDBUser,m_sDBPassword,m_sDBName,m_sDCERouter;
	int m_iPK_Device_Computer,m_iDBPort,m_iDCERouterPort;
	map<string,string> m_mapParameters;
	string m_mapParameters_Find(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it == m_mapParameters.end() ? NULL : (*it).second; }
	bool m_mapParameters_Exists(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it != m_mapParameters.end(); }

	DCEConfig(string sFilename="/etc/pluto.conf");
};
#endif 

