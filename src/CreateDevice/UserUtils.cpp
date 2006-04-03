#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "PlutoUtils/MySQLHelper.h"
#include "UserUtils.h"

UserUtils::UserUtils(MySqlHelper *pMySqlHelper, int PK_Installation)
{
	m_pMySqlHelper=pMySqlHelper;
	m_PK_Installation=PK_Installation;
}

bool UserUtils::AlreadyHasUsers()
{
	string sSQL = "SELECT PK_Users FROM Users JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation="
		+ StringUtils::itos(m_PK_Installation) + " LIMIT 1";

	PlutoSqlResult result_set_room;
	if( (result_set_room.r=m_pMySqlHelper->mysql_query_result(sSQL)) && result_set_room.r->row_count )
		return true;
	return false;
}

bool UserUtils::AlreadyHasMasterUsers()
{
	string sSQL = "SELECT PK_Users FROM Users JOIN Installation_Users ON FK_Users=PK_Users WHERE userCanModifyInstallation=1 and FK_Installation="
		+ StringUtils::itos(m_PK_Installation) + " LIMIT 1";

	PlutoSqlResult result_set_room;
	if( (result_set_room.r=m_pMySqlHelper->mysql_query_result(sSQL)) && result_set_room.r->row_count )
		return true;
	return false;
}

string UserUtils::GetGoodExtension(map<int,bool> &mapUsedExtensions) 
{
	// Don't put a condition.  If we use up more than the allowed extensions (shouldn't happen)
	// we'll just keep going anyway
	for(int i=FIRST_USER_EXTENSION;;++i)
	{
		if( mapUsedExtensions[i]==false )
		{
			mapUsedExtensions[i]=true;
			return StringUtils::itos(i);
		}
	}
}

void UserUtils::CheckExtensions(  )
{
	// First mark off which valid extensions are already used
	map<int,bool> mapUsedExtensions;
	string sSQL = "select Extension FROM Users JOIN Installation_Users ON FK_Users=PK_Users "
		"WHERE Extension IS NOT NULL AND Extension>=" + StringUtils::itos(FIRST_USER_EXTENSION) +
		" AND Extension<=" + StringUtils::itos(LAST_USER_EXTENSION) + 
		" ORDER BY Extension";

	PlutoSqlResult result_set_valid_ext;
	MYSQL_ROW row;
	if( (result_set_valid_ext.r=m_pMySqlHelper->mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set_valid_ext.r)))
			mapUsedExtensions[ atoi(row[0]) ] = true;

	sSQL = "select PK_Users FROM Users JOIN Installation_Users ON FK_Users=PK_Users "
		"WHERE Extension IS NULL OR Extension<" + StringUtils::itos(FIRST_USER_EXTENSION) +
		" OR Extension>" + StringUtils::itos(LAST_USER_EXTENSION) + 
		" ORDER BY UserName";

	PlutoSqlResult result_set_bad_ext;
	if( (result_set_bad_ext.r=m_pMySqlHelper->mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set_bad_ext.r)))
		{
			sSQL = "UPDATE Users SET Extension=" + GetGoodExtension(mapUsedExtensions) 
				+ " WHERE PK_Users=" + row[0];
			m_pMySqlHelper->threaded_mysql_query(sSQL);

		}
}

int UserUtils::AddUser(string sUsername)
{
	StringUtils::TrimSpaces(sUsername);
	bool bExistingUsers=AlreadyHasMasterUsers();
	string sSQL;
	
	MYSQL_ROW row;
	string sOriginalUsername=sUsername;
	// Be sure the username is unique, up to 99 users of the same name should be more than enough
	for(int i=1;i<99;++i)
	{
		if( i>1 )
			sUsername = sOriginalUsername + "_" + StringUtils::itos(i);
			
		sSQL = "SELECT count(PK_Users) from Users WHERE UserName='" + StringUtils::SQLEscape(sUsername) + "';";
		PlutoSqlResult result_set;
		if( (result_set.r=m_pMySqlHelper->mysql_query_result(sSQL))==NULL || 
			(row = mysql_fetch_row(result_set.r))==NULL || atoi(row[0])==0 )
				break;  // This username is unique
	}
	sSQL = "INSERT INTO Users(UserName,Password,PINCode) VALUES('" + StringUtils::SQLEscape(sUsername) + "',md5('" + StringUtils::SQLEscape(sUsername) + "'),md5('1234'));";

	int PK_Users = m_pMySqlHelper->threaded_mysql_query_withID(sSQL);
	sSQL = "INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation,userCanChangeHouseMode) "
		"VALUES(" + StringUtils::itos(m_PK_Installation) + "," + StringUtils::itos(PK_Users) + 
		(bExistingUsers ? ",0,0)" : ",1,1)");

	m_pMySqlHelper->threaded_mysql_query(sSQL);

	CheckExtensions();
	string sCmd = "/usr/pluto/bin/SetPasswords.sh " + StringUtils::itos(PK_Users) + " \"" + sUsername + "\"";
	g_pPlutoLogger->Write(LV_STATUS,"Executing %s",sCmd.c_str());
	system(sCmd.c_str());

	MySqlHelper mySqlHelper_telecom(m_pMySqlHelper->m_sMySQLHost,m_pMySqlHelper->m_sMySQLUser,m_pMySqlHelper->m_sMySQLPass,"pluto_telecom");

	string sPK_Users = StringUtils::itos(PK_Users);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",1,0,0,1,'ring,');";
	mySqlHelper_telecom.threaded_mysql_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",2,0,0,1,'voicemail,0');";
	mySqlHelper_telecom.threaded_mysql_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",4,0,0,1,'voicemail,0');";
	mySqlHelper_telecom.threaded_mysql_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",4,1,1,1,'ring,');";
	mySqlHelper_telecom.threaded_mysql_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",3,0,0,1,'voicemail,0');";
	mySqlHelper_telecom.threaded_mysql_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",3,1,1,1,'ring,');";
	mySqlHelper_telecom.threaded_mysql_query(sSQL);

	return PK_Users;
}

void UserUtils::RemoveUser(int PK_User)
{
	string sSQL = "DELETE FROM Users WHERE PK_Users=" + StringUtils::itos(PK_User);
	m_pMySqlHelper->threaded_mysql_query(sSQL);

	sSQL = "DELETE FROM Installation_Users WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pMySqlHelper->threaded_mysql_query(sSQL);

	sSQL = "DELETE FROM Device_Users WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pMySqlHelper->threaded_mysql_query(sSQL);

	sSQL = "DELETE FROM Orbiter_Users_PasswordReq WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pMySqlHelper->threaded_mysql_query(sSQL);

	sSQL = "DELETE FROM Room_Users WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pMySqlHelper->threaded_mysql_query(sSQL);

}

