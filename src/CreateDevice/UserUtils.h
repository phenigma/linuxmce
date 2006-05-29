#ifndef UserUtils_h
#define UserUtils_h

#include "PlutoUtils/MySQLHelper.h"

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
	int AddUser(string sUsername,Command_Impl *pCommand_Impl);
	void RemoveUser(int PK_Users);
};

#endif
