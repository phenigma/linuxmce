// Another user could replace this function with their own.  This is specific to Pluto's user database
// The function will take as the UserName either a username or a psc_user, and the password can be 
// either plain text or md5.  The function must return 0 if the validation failed, or return the psc_user if successful

#include "PlutoUtils/CommonIncludes.h"	
#include "CommonFunctions.h"
#include "PlutoUtils/MySQLHelper.h"

using namespace sqlCVS;

int ValidateUser(string Username,string Password,bool &bNoPassword,bool &bSupervisor)
{
cout << "Ready to validate Masterusers with host: " << g_GlobalConfig.dceConfig.m_sDBHost << " user: " << g_GlobalConfig.dceConfig.m_sDBUser << " pass: " << g_GlobalConfig.dceConfig.m_sDBPassword << endl;
	MySqlHelper mySqlHelper(g_GlobalConfig.dceConfig.m_sDBHost,g_GlobalConfig.dceConfig.m_sDBUser,g_GlobalConfig.dceConfig.m_sDBPassword,"MasterUsers");
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	string md5;
	int psc_user=0;
	if( (result_set.r=mySqlHelper.mysql_query_result("SELECT PK_MasterUsers,Password,sqlCvsAdmin FROM MasterUsers WHERE Username='" + Username + "'")) && (row = mysql_fetch_row(result_set.r)) )
	{
		psc_user = atoi(row[0]);
		md5 = row[1];
		bSupervisor = (row[2][0]=='1');
	}
	else if( (result_set.r=mySqlHelper.mysql_query_result("SELECT PK_MasterUsers,Password,sqlCvsAdmin FROM MasterUsers WHERE PK_MasterUsers='" + Username + "'")) && (row = mysql_fetch_row(result_set.r)) )
	{
		psc_user = atoi(row[0]);
		md5 = row[1];
		bSupervisor = (row[2][0]=='1');
	}
	else return 0;

	bNoPassword = (Password=="nopass" || Password=="0945fc9611f55fd0e183fb8b044f1afe"); // The md5 of nopass
	if( bNoPassword || Password==md5 || mySqlHelper.md5(Password)==md5 )
		return psc_user;

	cout << "Username: " << Username << " Password: " << Password << " md5: " << md5 << " failed login" << endl;
	return 0;
}
