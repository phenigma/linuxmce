#ifndef __Table_DesignObjVariation_DesignObj_H__
#define __Table_DesignObjVariation_DesignObj_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_DesignObj.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_DesignObjVariation_DesignObj : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_DesignObjVariation_DesignObj(Database_pluto_main *pDatabase):database(pDatabase), m_Mutex("DesignObjVariation_DesignObj")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_DesignObjVariation_DesignObj();

private:		
	friend class Row_DesignObjVariation_DesignObj;
	struct Key
	{
		friend class Row_DesignObjVariation_DesignObj;
		long int pk_PK_DesignObjVariation_DesignObj;

		
		Key(long int in_PK_DesignObjVariation_DesignObj);
	
		Key(class Row_DesignObjVariation_DesignObj *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_DesignObj::Key &key1, const Table_DesignObjVariation_DesignObj::Key &key2) const;
	};	

	
	

public:				
	void Commit();
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_DesignObj*> *rows);
	class Row_DesignObjVariation_DesignObj* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_DesignObj* GetRow(long int in_PK_DesignObjVariation_DesignObj);
	

private:	
	
		
	class Row_DesignObjVariation_DesignObj* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_DesignObjVariation_DesignObj : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_DesignObj::Key;
		friend class Table_DesignObjVariation_DesignObj;
	private:
		Table_DesignObjVariation_DesignObj *table;
		
		long int m_PK_DesignObjVariation_DesignObj;
long int m_FK_DesignObjVariation_Parent;
long int m_FK_DesignObj_Child;
long int m_DisplayOrder;
long int m_X;
long int m_Y;
long int m_Width;
long int m_Height;
long int m_FK_DesignObj_InsteadOf;
short int m_CanBeHidden;
short int m_HideByDefault;
short int m_RegenerateForEachScreen;
short int m_DisplayChildrenBeforeText;
short int m_DisplayChildrenBehindBackground;
short int m_DontMergeBackground;
short int m_IsTabStop;
long int m_FK_DesignObj_Up;
long int m_FK_DesignObj_Down;
long int m_FK_DesignObj_Left;
long int m_FK_DesignObj_Right;
string m_sFK_DesignObj_TiedTo;
string m_VisibleStates;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;

		bool is_null[27];
	
	public:
		long int PK_DesignObjVariation_DesignObj_get();
long int FK_DesignObjVariation_Parent_get();
long int FK_DesignObj_Child_get();
long int DisplayOrder_get();
long int X_get();
long int Y_get();
long int Width_get();
long int Height_get();
long int FK_DesignObj_InsteadOf_get();
short int CanBeHidden_get();
short int HideByDefault_get();
short int RegenerateForEachScreen_get();
short int DisplayChildrenBeforeText_get();
short int DisplayChildrenBehindBackground_get();
short int DontMergeBackground_get();
short int IsTabStop_get();
long int FK_DesignObj_Up_get();
long int FK_DesignObj_Down_get();
long int FK_DesignObj_Left_get();
long int FK_DesignObj_Right_get();
string sFK_DesignObj_TiedTo_get();
string VisibleStates_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();

		
		void PK_DesignObjVariation_DesignObj_set(long int val);
void FK_DesignObjVariation_Parent_set(long int val);
void FK_DesignObj_Child_set(long int val);
void DisplayOrder_set(long int val);
void X_set(long int val);
void Y_set(long int val);
void Width_set(long int val);
void Height_set(long int val);
void FK_DesignObj_InsteadOf_set(long int val);
void CanBeHidden_set(short int val);
void HideByDefault_set(short int val);
void RegenerateForEachScreen_set(short int val);
void DisplayChildrenBeforeText_set(short int val);
void DisplayChildrenBehindBackground_set(short int val);
void DontMergeBackground_set(short int val);
void IsTabStop_set(short int val);
void FK_DesignObj_Up_set(long int val);
void FK_DesignObj_Down_set(long int val);
void FK_DesignObj_Left_set(long int val);
void FK_DesignObj_Right_set(long int val);
void sFK_DesignObj_TiedTo_set(string val);
void VisibleStates_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);

		
		bool FK_DesignObjVariation_Parent_isNull();
bool FK_DesignObj_Child_isNull();
bool Width_isNull();
bool Height_isNull();
bool FK_DesignObj_InsteadOf_isNull();
bool FK_DesignObj_Up_isNull();
bool FK_DesignObj_Down_isNull();
bool FK_DesignObj_Left_isNull();
bool FK_DesignObj_Right_isNull();
bool sFK_DesignObj_TiedTo_isNull();
bool VisibleStates_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();

			
		void FK_DesignObjVariation_Parent_setNull(bool val);
void FK_DesignObj_Child_setNull(bool val);
void Width_setNull(bool val);
void Height_setNull(bool val);
void FK_DesignObj_InsteadOf_setNull(bool val);
void FK_DesignObj_Up_setNull(bool val);
void FK_DesignObj_Down_setNull(bool val);
void FK_DesignObj_Left_setNull(bool val);
void FK_DesignObj_Right_setNull(bool val);
void sFK_DesignObj_TiedTo_setNull(bool val);
void VisibleStates_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_DesignObj(Table_DesignObjVariation_DesignObj *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_DesignObj *Table_DesignObjVariation_DesignObj_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_Parent_getrow();
class Row_DesignObj* FK_DesignObj_Child_getrow();
class Row_DesignObj* FK_DesignObj_InsteadOf_getrow();
class Row_DesignObj* FK_DesignObj_Up_getrow();
class Row_DesignObj* FK_DesignObj_Down_getrow();
class Row_DesignObj* FK_DesignObj_Left_getrow();
class Row_DesignObj* FK_DesignObj_Right_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DesignObjVariation_DesignObj+ m_FK_DesignObjVariation_Parent+ m_FK_DesignObj_Child+ m_DisplayOrder+ m_X+ m_Y+ m_Width+ m_Height+ m_FK_DesignObj_InsteadOf+ m_CanBeHidden+ m_HideByDefault+ m_RegenerateForEachScreen+ m_DisplayChildrenBeforeText+ m_DisplayChildrenBehindBackground+ m_DontMergeBackground+ m_IsTabStop+ m_FK_DesignObj_Up+ m_FK_DesignObj_Down+ m_FK_DesignObj_Left+ m_FK_DesignObj_Right+ m_sFK_DesignObj_TiedTo+ m_VisibleStates+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_DesignObj_asSQL();
string FK_DesignObjVariation_Parent_asSQL();
string FK_DesignObj_Child_asSQL();
string DisplayOrder_asSQL();
string X_asSQL();
string Y_asSQL();
string Width_asSQL();
string Height_asSQL();
string FK_DesignObj_InsteadOf_asSQL();
string CanBeHidden_asSQL();
string HideByDefault_asSQL();
string RegenerateForEachScreen_asSQL();
string DisplayChildrenBeforeText_asSQL();
string DisplayChildrenBehindBackground_asSQL();
string DontMergeBackground_asSQL();
string IsTabStop_asSQL();
string FK_DesignObj_Up_asSQL();
string FK_DesignObj_Down_asSQL();
string FK_DesignObj_Left_asSQL();
string FK_DesignObj_Right_asSQL();
string sFK_DesignObj_TiedTo_asSQL();
string VisibleStates_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();

	};

#endif

