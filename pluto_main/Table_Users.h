#ifndef __Table_Users_H__
#define __Table_Users_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Users.h"
#include "SerializeClass/SerializeClass.h"

class DLL_EXPORT Table_Users
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Users(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Users")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Users();

private:		
	friend class Row_Users;
	struct Key
	{
		friend class Row_Users;
		long int pk_PK_Users;

		
		Key(long int in_PK_Users);
	
		Key(class Row_Users *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Users::Key &key1, const Table_Users::Key &key2) const;
	};	

	map<Table_Users::Key, class Row_Users*, Table_Users::Key_Less> cachedRows;
	map<Table_Users::Key, class Row_Users*, Table_Users::Key_Less> deleted_cachedRows;
	vector<class Row_Users*> addedRows;
	vector<class Row_Users*> deleted_addedRows;	
		

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_Users*> *rows);
	class Row_Users* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Users* GetRow(long int in_PK_Users);
	

private:	
	
		
	class Row_Users* FetchRow(Key &key);
		
			
};

class DLL_EXPORT Row_Users : public TableRow, public SerializeClass
	{
		friend struct Table_Users::Key;
		friend class Table_Users;
	private:
		Table_Users *table;
		
		long int m_PK_Users;
string m_UserName;
string m_Password;
short int m_samePasswordMasterUsers;
short int m_HasMailbox;
short int m_AccessGeneralMailbox;
long int m_Extension;
string m_FirstName;
string m_LastName;
string m_Nickname;
long int m_ExtensionRingTimeout;
string m_ForwardEmail;
short int m_FK_Language;
long int m_FK_Installation_Main;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[19];
	
		bool is_deleted;
		bool is_added;
		bool is_modified;					
	
	public:
		long int PK_Users_get();
string UserName_get();
string Password_get();
short int samePasswordMasterUsers_get();
short int HasMailbox_get();
short int AccessGeneralMailbox_get();
long int Extension_get();
string FirstName_get();
string LastName_get();
string Nickname_get();
long int ExtensionRingTimeout_get();
string ForwardEmail_get();
short int FK_Language_get();
long int FK_Installation_Main_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_Users_set(long int val);
void UserName_set(string val);
void Password_set(string val);
void samePasswordMasterUsers_set(short int val);
void HasMailbox_set(short int val);
void AccessGeneralMailbox_set(short int val);
void Extension_set(long int val);
void FirstName_set(string val);
void LastName_set(string val);
void Nickname_set(string val);
void ExtensionRingTimeout_set(long int val);
void ForwardEmail_set(string val);
void FK_Language_set(short int val);
void FK_Installation_Main_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool Extension_isNull();
bool FirstName_isNull();
bool LastName_isNull();
bool Nickname_isNull();
bool ForwardEmail_isNull();
bool FK_Installation_Main_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void Extension_setNull(bool val);
void FirstName_setNull(bool val);
void LastName_setNull(bool val);
void Nickname_setNull(bool val);
void ForwardEmail_setNull(bool val);
void FK_Installation_Main_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Users(Table_Users *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Users *Table_Users_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Language* FK_Language_getrow();
class Row_Installation* FK_Installation_Main_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_Users_Maintainer_getrows(vector <class Row_DeviceTemplate*> *rows);
void Device_Users_FK_Users_getrows(vector <class Row_Device_Users*> *rows);
void Installation_Users_FK_Users_getrows(vector <class Row_Installation_Users*> *rows);
void Orbiter_Users_PasswordReq_FK_Users_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_Users+ m_UserName+ m_Password+ m_samePasswordMasterUsers+ m_HasMailbox+ m_AccessGeneralMailbox+ m_Extension+ m_FirstName+ m_LastName+ m_Nickname+ m_ExtensionRingTimeout+ m_ForwardEmail+ m_FK_Language+ m_FK_Installation_Main+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_Users_asSQL();
string UserName_asSQL();
string Password_asSQL();
string samePasswordMasterUsers_asSQL();
string HasMailbox_asSQL();
string AccessGeneralMailbox_asSQL();
string Extension_asSQL();
string FirstName_asSQL();
string LastName_asSQL();
string Nickname_asSQL();
string ExtensionRingTimeout_asSQL();
string ForwardEmail_asSQL();
string FK_Language_asSQL();
string FK_Installation_Main_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

