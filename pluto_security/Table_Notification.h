#ifndef __Table_Notification_H__
#define __Table_Notification_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Notification.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Notification : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Notification(Database_pluto_security *pDatabase):database(pDatabase), m_Mutex("Notification")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Notification();

private:		
	friend class Row_Notification;
	struct Key
	{
		friend class Row_Notification;
		long int pk_PK_Notification;

		
		Key(long int in_PK_Notification);
	
		Key(class Row_Notification *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Notification::Key &key1, const Table_Notification::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Notification*> *rows);
	class Row_Notification* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_Notification* GetRow(long int in_PK_Notification);
	

private:	
	
		
	class Row_Notification* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Notification : public TableRow, public SerializeClass
	{
		friend struct Table_Notification::Key;
		friend class Table_Notification;
	private:
		Table_Notification *table;
		
		long int m_PK_Notification;
long int m_FK_Alert;
string m_NotificationTime;
string m_Info;
string m_Result;

		bool is_null[5];
	
	public:
		long int PK_Notification_get();
long int FK_Alert_get();
string NotificationTime_get();
string Info_get();
string Result_get();

		
		void PK_Notification_set(long int val);
void FK_Alert_set(long int val);
void NotificationTime_set(string val);
void Info_set(string val);
void Result_set(string val);

		
		bool Info_isNull();
bool Result_isNull();

			
		void Info_setNull(bool val);
void Result_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Notification(Table_Notification *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Notification *Table_Notification_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Alert* FK_Alert_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Notification+ m_FK_Alert+ m_NotificationTime+ m_Info+ m_Result;
		}
	private:
		void SetDefaultValues();
		
		string PK_Notification_asSQL();
string FK_Alert_asSQL();
string NotificationTime_asSQL();
string Info_asSQL();
string Result_asSQL();

	};

#endif

