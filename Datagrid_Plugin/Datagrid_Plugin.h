/**
 *
 * @file Datagrid_Plugin.h
 * @brief header file for the DataGridGeneratorPlugIn, DataGridGeneratorCallBack classes
 *
 */
 
//<-dceag-d-b->
#ifndef Datagrid_Plugin_h
#define Datagrid_Plugin_h

//	DCE Implemenation for #9 Datagrid Plug-in

#include "Gen_Devices/Datagrid_PluginBase.h"
//<-dceag-d-e->

namespace DCE
{
class Message;

/** This plug-in has it's own plug-in architecture as well to keep track of datagrid generator */
typedef map<string, class DataGridTable *> DataGridMap;

class DataGridGeneratorPlugIn {
};

/**
 * If a non-null pointer is returned, the framework will remove any existing datagrid by this name and replace it with this one.
 */
 
typedef class DataGridTable *( DataGridGeneratorPlugIn::*DCEDataGridGeneratorFn )( string GridID, string Parms, void *pExtraData
				, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage );

/** Contains a pointer to the datagrid callback */

class DataGridGeneratorCallBack
{
public:
	class DataGridGeneratorPlugIn *m_pDataGridGeneratorPlugIn;
	DCEDataGridGeneratorFn m_pDCEDataGridGeneratorFn;
	/**
	 * @note There is a reason why the pointer to DataGridGeneratorPlugIn must be passed in as void. If the class being passed in doesn't
	 * derive from DataGridGeneratorPlugIn as the first parent, the "this" pointer gets offset when it's passed in. Then when the callback is
	 * made, it no longer pointer to the right place, and "this" ends up pointing to an incorrect memory table. The solution is to pass
	 * in the plug-ins as void * and then convert back. See the sample app in voidptr.cpp within this project to demonstrate it.
	
	 * DataGridGeneratorCallBack( class DataGridGeneratorPlugIn *pDataGridGeneratorPlugIn, DCEDataGridGeneratorFn pDCEDataGridGeneratorFn )   <-- this won't work
	 */

	/** @brief constructor */
	
	DataGridGeneratorCallBack( void *pDataGridGeneratorPlugIn, DCEDataGridGeneratorFn pDCEDataGridGeneratorFn )
	{
		m_pDataGridGeneratorPlugIn=( class DataGridGeneratorPlugIn * ) pDataGridGeneratorPlugIn;
		m_pDCEDataGridGeneratorFn=pDCEDataGridGeneratorFn;
	}
};

//<-dceag-decl-b->!
	
	/**
	 * @brief documenatation
	 * @todo complete documentation
	 */
	 
	class Datagrid_Plugin : public Datagrid_Plugin_Command
	{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Datagrid_Plugin( int DeviceID, string ServerAddress, bool bConnectEventHandler=true, bool bLocalMode=false, class Router *pRouter=NULL );
		virtual ~Datagrid_Plugin();
		virtual bool Register();
//<-dceag-const-e->

	/** Private member variables */
	
	pluto_pthread_mutex_t m_DataGridMutex;
	DataGridMap m_DataGrids;
	map<int, class DataGridGeneratorCallBack *> m_mapDataGridGeneratorCallBack;

	/** Private methods */
public:
	/** Public member variables */

	void RegisterDatagridGenerator( class DataGridGeneratorCallBack *pCallBack, int PK_DataGrid );

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_Touch_or_click( int iX_Position, int iY_Position );

			*****COMMANDS***** we need to implement
	*/

/* 
	COMMAND: #34 - Request Datagrid Contents
	COMMENTS: The orbiters use this to request the contents of a datagrid, so they can display it for the user. Before you can request a grid, the command Populate Datagrid must be called, passing in the information required to populate the grid as well a \"GridID\", 
	PARAMETERS:
		#10 ID
			For debugging purposes if problems arise with a request not being filled, or a grid not populated when it should be. If the Orbiter specified an ID when requesting the grid or populating it, the Datagrid plug-in will log the ID and status so the develope
		#15 DataGrid ID
			A unique identifier for the grid that was passed to populate grid.
		#19 Data
			The binary contents of the grid.
		#32 Row
			The starting row
		#33 Column
			The starting column
		#34 Row count
			How many rows to fetch
		#35 Column count
			How many columns to fetch.
		#36 Keep Row Header
			True means the first row ( 0 ) is locked and will always be at the top as a header.
		#37 Keep Column Header
			True means the first column( 0 ) is locked and will always be at the top as a header.
		#49 Add Up-Down Arrows
			If to add up/down arrows to the returned datagrid when datagrid doesn't fully fit.
*/
	virtual void CMD_Request_Datagrid_Contents( string sID, string sDataGrid_ID, int iRow, int iColumn, int iRow_count, int iColumn_count, bool bKeep_Row_Header, bool bKeep_Column_Header, bool bAdd_UpDown_Arrows, char **pData, int *iData_Size ) { string sCMD_Result; CMD_Request_Datagrid_Contents( sID.c_str(), sDataGrid_ID.c_str(), iRow, iColumn, iRow_count, iColumn_count, bKeep_Row_Header, bKeep_Column_Header, bAdd_UpDown_Arrows, pData, iData_Size, sCMD_Result, NULL );};
	virtual void CMD_Request_Datagrid_Contents( string sID, string sDataGrid_ID, int iRow, int iColumn, int iRow_count, int iColumn_count, bool bKeep_Row_Header, bool bKeep_Column_Header, bool bAdd_UpDown_Arrows, char **pData, int *iData_Size, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #35 - Populate Datagrid
	COMMENTS: Before an orbiter can request the contents of a datagrid, the datagrid plug-in must first populate it. This is how the orbiter tells the plug-in to create and populate a datagrid, referenced by the unique ID passed in with this command.
	PARAMETERS:
		#4 PK_Variable
			The populate grid can optionally return a variable number to assign a value into. For example, the current path in the file grid.
		#5 Value To Assign
			The value to assign into the variable.
		#10 ID
			For debugging purposes if problems arise with a request not being filled, or a grid not populated when it should be. If the Orbiter specified an ID when requesting the grid or populating it, the Datagrid plug-in will log the ID and status so the develope
		#15 DataGrid ID
			A unique ID for this instance of the grid that will be passed with the Request Datagrid Contents command.
		#38 PK_Datagrid
			Which grid should be populated
		#39 Options
			The options are specific the type of grid ( PK_Datagrid ). These are not pre-defined. The grid generator and orbiter must both pass the options in the correct format for the type of grid.
		#40 IsSuccessful
			Returns false if the grid could not be populated. Perhaps there was no registered datagrid generator.
*/
	virtual void CMD_Populate_Datagrid( string sID, string sDataGrid_ID, int iPK_Datagrid, string sOptions, int *iPK_Variable, string *sValue_To_Assign, bool *bIsSuccessful ) { string sCMD_Result; CMD_Populate_Datagrid( sID.c_str(), sDataGrid_ID.c_str(), iPK_Datagrid, sOptions.c_str(), iPK_Variable, sValue_To_Assign, bIsSuccessful, sCMD_Result, NULL );};
	virtual void CMD_Populate_Datagrid( string sID, string sDataGrid_ID, int iPK_Datagrid, string sOptions, int *iPK_Variable, string *sValue_To_Assign, bool *bIsSuccessful, string &sCMD_Result, Message *pMessage );

//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
