#ifndef __Table_CommandGroup_H__
#define __Table_CommandGroup_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_CommandGroup : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_CommandGroup(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("CommandGroup")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_CommandGroup();

private:		
	friend class Row_CommandGroup;
	struct Key
	{
		friend class Row_CommandGroup;
		long int pk_PK_CommandGroup;

		
		Key(long int in_PK_CommandGroup);
	
		Key(class Row_CommandGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup::Key &key1, const Table_CommandGroup::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_CommandGroup*> *rows);
	class Row_CommandGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup* GetRow(long int in_PK_CommandGroup);
	

private:	
	
		
	class Row_CommandGroup* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_CommandGroup : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup::Key;
		friend class Table_CommandGroup;
	private:
		Table_CommandGroup *table;
		
		long int m_PK_CommandGroup;
long int m_FK_Array;
long int m_FK_Installation;
string m_Description;
string m_Hint;
short int m_CanTurnOff;
short int m_AlwaysShow;
short int m_CanBeHidden;
long int m_FK_Criteria_Orbiter;
long int m_FK_DesignObj;
long int m_FK_Template;
long int m_AltID;
long int m_FK_Icon;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[18];
	
	public:
		long int PK_CommandGroup_get();
long int FK_Array_get();
long int FK_Installation_get();
string Description_get();
string Hint_get();
short int CanTurnOff_get();
short int AlwaysShow_get();
short int CanBeHidden_get();
long int FK_Criteria_Orbiter_get();
long int FK_DesignObj_get();
long int FK_Template_get();
long int AltID_get();
long int FK_Icon_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_CommandGroup_set(long int val);
void FK_Array_set(long int val);
void FK_Installation_set(long int val);
void Description_set(string val);
void Hint_set(string val);
void CanTurnOff_set(short int val);
void AlwaysShow_set(short int val);
void CanBeHidden_set(short int val);
void FK_Criteria_Orbiter_set(long int val);
void FK_DesignObj_set(long int val);
void FK_Template_set(long int val);
void AltID_set(long int val);
void FK_Icon_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_Array_isNull();
bool FK_Installation_isNull();
bool Hint_isNull();
bool FK_Criteria_Orbiter_isNull();
bool FK_DesignObj_isNull();
bool FK_Template_isNull();
bool AltID_isNull();
bool FK_Icon_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_Array_setNull(bool val);
void FK_Installation_setNull(bool val);
void Hint_setNull(bool val);
void FK_Criteria_Orbiter_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void FK_Template_setNull(bool val);
void AltID_setNull(bool val);
void FK_Icon_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CommandGroup(Table_CommandGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup *Table_CommandGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Array* FK_Array_getrow();
class Row_Installation* FK_Installation_getrow();
class Row_Criteria* FK_Criteria_Orbiter_getrow();
class Row_DesignObj* FK_DesignObj_getrow();
class Row_Template* FK_Template_getrow();
class Row_Icon* FK_Icon_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_CommandGroup_getrows(vector <class Row_CommandGroup_Command*> *rows);
void CommandGroup_EntertainArea_FK_CommandGroup_getrows(vector <class Row_CommandGroup_EntertainArea*> *rows);
void CommandGroup_Room_FK_CommandGroup_getrows(vector <class Row_CommandGroup_Room*> *rows);
void Device_CommandGroup_FK_CommandGroup_getrows(vector <class Row_Device_CommandGroup*> *rows);
void EventHandler_FK_CommandGroup_getrows(vector <class Row_EventHandler*> *rows);


		// Setup binary serialization
		void SetupSerialization() {
			StartSerializeList() + m_PK_CommandGroup+ m_FK_Array+ m_FK_Installation+ m_Description+ m_Hint+ m_CanTurnOff+ m_AlwaysShow+ m_CanBeHidden+ m_FK_Criteria_Orbiter+ m_FK_DesignObj+ m_FK_Template+ m_AltID+ m_FK_Icon+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_CommandGroup_asSQL();
string FK_Array_asSQL();
string FK_Installation_asSQL();
string Description_asSQL();
string Hint_asSQL();
string CanTurnOff_asSQL();
string AlwaysShow_asSQL();
string CanBeHidden_asSQL();
string FK_Criteria_Orbiter_asSQL();
string FK_DesignObj_asSQL();
string FK_Template_asSQL();
string AltID_asSQL();
string FK_Icon_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

