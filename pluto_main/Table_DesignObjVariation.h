#ifndef __Table_DesignObjVariation_H__
#define __Table_DesignObjVariation_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DesignObjVariation : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjVariation(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjVariation")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjVariation();

private:		
	friend class Row_DesignObjVariation;
	struct Key
	{
		friend class Row_DesignObjVariation;
		long int pk_PK_DesignObjVariation;

		
		Key(long int in_PK_DesignObjVariation);
	
		Key(class Row_DesignObjVariation *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation::Key &key1, const Table_DesignObjVariation::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation*> *rows);
	class Row_DesignObjVariation* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation* GetRow(long int in_PK_DesignObjVariation);
	

private:	
	
		
	class Row_DesignObjVariation* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DesignObjVariation : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation::Key;
		friend class Table_DesignObjVariation;
	private:
		Table_DesignObjVariation *table;
		
		long int m_PK_DesignObjVariation;
long int m_FK_DesignObj;
long int m_FK_Criteria_D;
long int m_FK_DesignObj_Goto;
long int m_FK_CommandGroup_D_OnActivate;
long int m_FK_CommandGroup_D_OnLoad;
long int m_FK_CommandGroup_D_OnUnload;
long int m_FK_CommandGroup_D_OnTimeout;
long int m_FK_CommandGroup_D_OnStartup;
long int m_FK_Button;
long int m_FK_Criteria_Orbiter;
short int m_DontResetSelectedState;
long int m_FK_StabilityStatus;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[18];
	
	public:
		long int PK_DesignObjVariation_get();
long int FK_DesignObj_get();
long int FK_Criteria_D_get();
long int FK_DesignObj_Goto_get();
long int FK_CommandGroup_D_OnActivate_get();
long int FK_CommandGroup_D_OnLoad_get();
long int FK_CommandGroup_D_OnUnload_get();
long int FK_CommandGroup_D_OnTimeout_get();
long int FK_CommandGroup_D_OnStartup_get();
long int FK_Button_get();
long int FK_Criteria_Orbiter_get();
short int DontResetSelectedState_get();
long int FK_StabilityStatus_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DesignObjVariation_set(long int val);
void FK_DesignObj_set(long int val);
void FK_Criteria_D_set(long int val);
void FK_DesignObj_Goto_set(long int val);
void FK_CommandGroup_D_OnActivate_set(long int val);
void FK_CommandGroup_D_OnLoad_set(long int val);
void FK_CommandGroup_D_OnUnload_set(long int val);
void FK_CommandGroup_D_OnTimeout_set(long int val);
void FK_CommandGroup_D_OnStartup_set(long int val);
void FK_Button_set(long int val);
void FK_Criteria_Orbiter_set(long int val);
void DontResetSelectedState_set(short int val);
void FK_StabilityStatus_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Criteria_D_isNull();
bool FK_DesignObj_Goto_isNull();
bool FK_CommandGroup_D_OnActivate_isNull();
bool FK_CommandGroup_D_OnLoad_isNull();
bool FK_CommandGroup_D_OnUnload_isNull();
bool FK_CommandGroup_D_OnTimeout_isNull();
bool FK_CommandGroup_D_OnStartup_isNull();
bool FK_Button_isNull();
bool FK_Criteria_Orbiter_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Criteria_D_setNull(bool val);
void FK_DesignObj_Goto_setNull(bool val);
void FK_CommandGroup_D_OnActivate_setNull(bool val);
void FK_CommandGroup_D_OnLoad_setNull(bool val);
void FK_CommandGroup_D_OnUnload_setNull(bool val);
void FK_CommandGroup_D_OnTimeout_setNull(bool val);
void FK_CommandGroup_D_OnStartup_setNull(bool val);
void FK_Button_setNull(bool val);
void FK_Criteria_Orbiter_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation(Table_DesignObjVariation *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation *Table_DesignObjVariation_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObj* FK_DesignObj_getrow();
class Row_Criteria_D* FK_Criteria_D_getrow();
class Row_DesignObj* FK_DesignObj_Goto_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnActivate_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnLoad_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnUnload_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnTimeout_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnStartup_getrow();
class Row_Button* FK_Button_getrow();
class Row_Criteria* FK_Criteria_Orbiter_getrow();
class Row_StabilityStatus* FK_StabilityStatus_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_DesignObj_FK_DesignObjVariation_Parent_getrows(vector <class Row_DesignObjVariation_DesignObj*> *rows);
void DesignObjVariation_DesignObjParameter_FK_DesignObjVariation_getrows(vector <class Row_DesignObjVariation_DesignObjParameter*> *rows);
void DesignObjVariation_Text_FK_DesignObjVariation_getrows(vector <class Row_DesignObjVariation_Text*> *rows);
void DesignObjVariation_Zone_FK_DesignObjVariation_getrows(vector <class Row_DesignObjVariation_Zone*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_DesignObjVariation+ m_FK_DesignObj+ m_FK_Criteria_D+ m_FK_DesignObj_Goto+ m_FK_CommandGroup_D_OnActivate+ m_FK_CommandGroup_D_OnLoad+ m_FK_CommandGroup_D_OnUnload+ m_FK_CommandGroup_D_OnTimeout+ m_FK_CommandGroup_D_OnStartup+ m_FK_Button+ m_FK_Criteria_Orbiter+ m_DontResetSelectedState+ m_FK_StabilityStatus+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_asSQL();
string FK_DesignObj_asSQL();
string FK_Criteria_D_asSQL();
string FK_DesignObj_Goto_asSQL();
string FK_CommandGroup_D_OnActivate_asSQL();
string FK_CommandGroup_D_OnLoad_asSQL();
string FK_CommandGroup_D_OnUnload_asSQL();
string FK_CommandGroup_D_OnTimeout_asSQL();
string FK_CommandGroup_D_OnStartup_asSQL();
string FK_Button_asSQL();
string FK_Criteria_Orbiter_asSQL();
string DontResetSelectedState_asSQL();
string FK_StabilityStatus_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

