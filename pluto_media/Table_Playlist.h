#ifndef __Table_Playlist_H__
#define __Table_Playlist_H__

#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Playlist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DLL_EXPORT Table_Playlist : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
    pluto_pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_MutexAttr;
		
	Table_Playlist(Database_pluto_media *pDatabase):database(pDatabase), m_Mutex("Playlist")
	{
		pthread_mutexattr_init(&m_MutexAttr);
		pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		m_Mutex.Init(&m_MutexAttr); 
	};
	~Table_Playlist();

private:		
	friend class Row_Playlist;
	struct Key
	{
		friend class Row_Playlist;
		long int pk_PK_Playlist;

		
		Key(long int in_PK_Playlist);
	
		Key(class Row_Playlist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Playlist::Key &key1, const Table_Playlist::Key &key2) const;
	};	

	
	

public:				
	bool Commit();
	bool GetRows(string where_statement,vector<class Row_Playlist*> *rows);
	class Row_Playlist* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Playlist* GetRow(long int in_PK_Playlist);
	

private:	
	
		
	class Row_Playlist* FetchRow(SingleLongKey &key);
		
			
};

class DLL_EXPORT Row_Playlist : public TableRow, public SerializeClass
	{
		friend struct Table_Playlist::Key;
		friend class Table_Playlist;
	private:
		Table_Playlist *table;
		
		long int m_PK_Playlist;
long int m_EK_User;
string m_Name;

		bool is_null[3];
	
	public:
		long int PK_Playlist_get();
long int EK_User_get();
string Name_get();

		
		void PK_Playlist_set(long int val);
void EK_User_set(long int val);
void Name_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Playlist(Table_Playlist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Playlist *Table_Playlist_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void PlaylistEntry_FK_Playlist_getrows(vector <class Row_PlaylistEntry*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Playlist+ m_EK_User+ m_Name;
		}
	private:
		void SetDefaultValues();
		
		string PK_Playlist_asSQL();
string EK_User_asSQL();
string Name_asSQL();

	};

#endif

