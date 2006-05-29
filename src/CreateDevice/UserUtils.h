#ifndef UserUtils_h
#define UserUtils_h

#include "PlutoUtils/MySQLHelper.h"

namespace DCE
{
	class Command_Impl;
};

class UserUtils
{

#define FIRST_USER_EXTENSION	301
#define LAST_USER_EXTENSION		399

	MySqlHelper *m_pMySqlHelper;
	int m_PK_Installation;

public:
	UserUtils(MySqlHelper *pMySqlHelper, int PK_Installation);
	bool AlreadyHasUsers();
	bool AlreadyHasMasterUsers();
	string GetGoodExtension(map<int,bool> &mapUsedExtensions) ;
	void CheckExtensions();
	int AddUser(string sUsername,DCE::Command_Impl *pCommand_Impl=NULL);
	void RemoveUser(int PK_Users);
};

#endif
