#ifndef __Table_Firewall_H__
#define __Table_Firewall_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Firewall.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Firewall : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Firewall(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("Firewall")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Firewall();

private:		
	friend class Row_Firewall;
	struct Key
	{
		friend class Row_Firewall;
		long int pk_PK_PortForward;

		
		Key(long int in_PK_PortForward);
	
		Key(class Row_Firewall *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Firewall::Key &key1, const Table_Firewall::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Firewall*> *rows);
	class Row_Firewall* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Firewall* GetRow(long int in_PK_PortForward);
	

private:	
	
		
	class Row_Firewall* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Firewall : public TableRow, public SerializeClass
	{
		friend struct Table_Firewall::Key;
		friend class Table_Firewall;
	private:
		Table_Firewall *table;
		
		long int m_PK_PortForward;
string m_Protocol;
long int m_SourcePort;
long int m_SourcePortEnd;
long int m_DestinationPort;
string m_DestinationIP;
string m_RuleType;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[12];
	
	public:
		long int PK_PortForward_get();
string Protocol_get();
long int SourcePort_get();
long int SourcePortEnd_get();
long int DestinationPort_get();
string DestinationIP_get();
string RuleType_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_PortForward_set(long int val);
void Protocol_set(string val);
void SourcePort_set(long int val);
void SourcePortEnd_set(long int val);
void DestinationPort_set(long int val);
void DestinationIP_set(string val);
void RuleType_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Firewall(Table_Firewall *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Firewall *Table_Firewall_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PortForward+ m_Protocol+ m_SourcePort+ m_SourcePortEnd+ m_DestinationPort+ m_DestinationIP+ m_RuleType+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_PortForward_asSQL();
string Protocol_asSQL();
string SourcePort_asSQL();
string SourcePortEnd_asSQL();
string DestinationPort_asSQL();
string DestinationIP_asSQL();
string RuleType_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

