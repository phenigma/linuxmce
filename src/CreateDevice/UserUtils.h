#ifndef UserUtils_h
#define UserUtils_h

#include "PlutoUtils/MySQLHelper.h"

class UserUtils
{

#define FIRST_USER_EXTENSION	100
#define LAST_USER_EXTENSION		199

	MySqlHelper *m_pMySqlHelper;
	int m_PK_Installation;

public:
	UserUtils(MySqlHelper *pMySqlHelper, int PK_Installation);
	bool AlreadyHasUsers();
	string GetGoodExtension(map<int,bool> &mapUsedExtensions) ;
	void CheckExtensions();
	int AddUser(string sUsername);
	void RemoveUser(int PK_Users);
};

#endif
