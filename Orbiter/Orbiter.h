/**
 *
 * @file Orbiter.h
 * @brief header file for the ScreenHistory, Orbiter and NeedToRender classes
 * @author
 * @todo notcommented
 * @warning do not change the code between the tags( <-dceag-...-b-> < -dceag-...-e-> ) , it will be overriden by the generator ( unless '!' specified )
 *
 */


//<-dceag-d-b->
#ifndef Orbiter_h
#define Orbiter_h

//	DCE Implemenation for #8 Orbiter

#include "Gen_Devices/OrbiterBase.h"
//<-dceag-d-e->

#include "DCE/Logger.h"
#include "OrbiterData.h"
#include "DesignObj_Orbiter.h"

namespace DCE
{

/**
 * @brief Contains a snapshot of the screen that was viewed and the state so that it can be restored with GoBack
 */
class ScreenHistory
{

public:

	/**
	 * @brief A unique ID that can be specified in the Goto command
	 * A unique ID that can be specified in the Goto command, allowing a particular 'instance' 
	 * of a screen to be removed in 'remove screen from history'
	 */
	string m_sID;
	
	int m_dwPK_Device; /** < The device being controlled */
	int m_dwPK_Users; /** < The current user */
	class LocationInfo *m_pLocationInfo; /** < The current location */
	int m_iCantGoBack; /** < If we get a go back, skip over this screen unless "Force" is set to true */
	class DesignObj_Orbiter *m_pObj; /** < The screen we're viewing */
	map < int, string > m_mapVariables; /** < Any variables we need to restore when returning to this screen */

	/**
	 * @brief constructor
	 */
	ScreenHistory( class DesignObj_Orbiter *pObj, class ScreenHistory *pScreenHistory_Prior ) 
	{ 
		m_pObj = pObj; m_dwPK_Device = m_dwPK_Users = 0; m_iCantGoBack = false; m_pLocationInfo = NULL; 
		if( pScreenHistory_Prior )
		{
			// We'll just copy the standard stuff from the prior screen
			m_dwPK_Device = pScreenHistory_Prior->m_dwPK_Device;
			m_dwPK_Users = pScreenHistory_Prior->m_dwPK_Users;
			m_pLocationInfo = pScreenHistory_Prior->m_pLocationInfo;
		}
	}
};


typedef void ( Orbiter::*OrbiterCallBack )( void *iData );


//<-dceag-decl-b->!custom
/**
 * @brief
 */
class Orbiter : public Orbiter_Command, public OrbiterData
{
//<-dceag-decl-e-> 

	friend class BD_PC_SetVariable; /** < Needs to maniuplate our variables */
	friend class NeedToRender; /** < Needs to maniuplate our variables */
	
//<-dceag-const-b->!

public:

	/**
	 * @brief constructor, assignes values to member data
	 */
	Orbiter( int iDeviceID, string sServerAddress, string sLocalDirectory, bool bLocalMode, int iImageWidth, int iImageHeight );
	
	/**
	 * @brief destructor
	 */
	virtual ~Orbiter();
	
	/**
	 * This function will only be used if this device is loaded into the DCE Router's memory space 
	 * as a plug-in. Otherwise Connect() will be called from the main()
	 */
	virtual bool Register();
	
//<-dceag-const-e->

public:

	/**
	 *	ACCEPT OUTSIDE INPUT
	 */
	
	/**
	 * @brief the specified button was pressed
	 */
	virtual bool ButtonDown( int iPK_C_Button );
	
	/**
	 * @brief a button was relesed
	 * @todo ask
	 */
	virtual bool ButtonUp();
	
	/**
	 * @brief the point belongs to a region that was clicked
	 * @todo ask
	 */
	virtual bool RegionDown( int iX, int iY );
	
	/**
	 * @brief
	 * @todo ask
	 * @warning We don't handle this here
	 */
	virtual bool RegionUp() { return false; };
	
	/**
	 * @brief Something happened, like a touch or a button, reset any timeouts or screen saver
	 */
	virtual void GotActivity();
	

	/**
	 *	UTILITIES
	 */
	
	/**
	 * @brief the text identified by it's primary key is found in an object
	 * @return the object containing the text
	 * @todo ask
	 */
	virtual DesignObjText *FindText( DesignObj_Orbiter *pObj, int iPK_Text );

	/**
	 * @brief Redraw any objects that change during execution of a command
	 */
	virtual bool ReceivedMessage( class Message *pMessageOriginal );

	/**
	 * @brief sets the capture keyboard parameters
	 */
	bool BuildCaptureKeyboardParams( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid );
	
	/**
	 * @brief deletes the last char from the capture keyboard variable (from the variable map)
	 * @todo ask what's this used for?
	 */
	bool CaptureKeyboard_EditText_DeleteLastChar();
	
	/**
	 * @brief adds the char to the capture keyboard variable (from the variable map)
	 * @todo ask what's this used for?
	 */
	bool CaptureKeyboard_EditText_AppendChar( char ch );
	
	/**
	 * @brief sets the value of the specified variable in the variable map
	 */
	void CaptureKeyboard_UpdateVariableAndText( int iVariable, string sVariable );

	/**
	 * @brief Find an object
	 * @todo ask
	 */
	DesignObj_Orbiter *FindObject( string sPK_DesignObj, class DesignObj_Orbiter *pDesignObj_Orbiter = NULL );
	
	/**
	 * @brief finds an object identified by a single numeber by searching in the object hierarchi
	 * @todo ask
	 */
	DesignObj_Orbiter *FindSingleNumberObject( int iPK_ID_Object, DesignObj_Orbiter *pObj );
	
	/**
	 * @brief executes the commands in the command list
	 * @todo ask
	 */
	void ExecuteCommandsInList( DesignObjCommandList *pDesignObjCommandList, DesignObj_Orbiter *pDesignObj_Orbiter, int iX = 0, int iY = 0 ); // Execute commands
	
	/**
	 * @brief returns the grid cell dimensions in the parameters
	 * @todo ask
	 */
	void GetGridCellDimensions( DesignObj_DataGrid *pObj, int iColSpan, int iRowSpan, int iColumn, int iRow, int &iX, int &iY, int &iWidth, int &iHeight );
	
	/**
	 * @brief substitutes some defined markers with values taken from the class data
	 * @todo ask
	 */
	string SubstituteVariables( string Input, DesignObj_Orbiter *pDesignObj_Orbiter, int iX, int iY );
	
	/**
	 * @brief A helper function we can call internally rather than the full CMD_GotoScreen
	 */
	void GotoScreen( string sScreen, string sID = "" ) { CMD_Goto_Screen( 0, sScreen, sID, "", false ); }
	
	/**
	 * @brief does a custom comparation (you ca also specify the operand to use) with the value from the variable identified by the key and the other parmeter
	 * @param sComparisson specifies the operand to use
	 */
	bool DoComparisson( int PK_Variable, string sComparisson, string sValue );
	
	/**
	 * @brief
	 * @todo ask
	 */
	void FindDGArrows( DesignObj_Orbiter *pObj, DesignObj_DataGrid *pDGObj );
	
	/**
	 * @brief gets the grid from the DesignObj_DataGrid object into the pDataGridTable parmeter
	 */
	bool AcquireGrid( DesignObj_DataGrid *pObj, int GridCurCol, int GridCurRow, class DataGridTable* &pDataGridTable );
	
	/**
	 * @brief returns the selected identifiing it by the row number
	 * @todo ask
	 */
	bool SelectedGrid( int iDGRow );

	/**
	 * @brief We need the maintenance function to be called at this time
	 */
	void CallMaintenanceAtTime( time_t t, OrbiterCallBack fnCallBack, void *iData ) { CallMaintenanceInTicks( ( clock_t ) ( t-time( NULL ) ) * CLOCKS_PER_SEC, fnCallBack, iData ); }
	
	/**
	 * @brief We need the maintenance function to be called in this many clock ticks
	 */
	void CallMaintenanceInTicks( clock_t c, OrbiterCallBack fnCallBack, void *iData );

	/**
	 *	MAINTENANCE CALL BACKS
	 *
	 *	When you need something to be done at a particular time, like popup buttons, call 'CallMaintenance', passing in the time you need the function
	 *	called, and any iData you want the function to get, as a void *. The call backs must be void Function( void *iData ) syntax.
	 */

	/**
	 * @brief
	 * @param iData is a pointer to an object to deselect
	 */
	void DeselectObjects( void *iData );

	
	/** temporary solution only */
	
	
	/**
	* @brief calculates the new current row after the specified number of cells have been skiped (upwards)
	* @todo ask
	*/
	void CalculateGridUp( DesignObj_DataGrid *pObj, int &iCurRow, int iCellsToSkip );
	
	/**
	* @brief calculates the new current row after the specified number of cells have been skiped (down)
	* @todo ask
	*/
	void CalculateGridDown( DesignObj_DataGrid *pObj, int &iCurRow, int iCellsToSkip );
	
	/**
	* @brief calculates the new current column after the specified number of cells have been skiped (left)
	* @todo ask
	*/
	void CalculateGridLeft( DesignObj_DataGrid *pObj, int &iCurCol, int iCellsToSkip );
	
	/**
	* @brief calculates the new current column after the specified number of cells have been skiped (right)
	* @todo ask
	*/
	void CalculateGridRight( DesignObj_DataGrid *pObj, int &iCurCol, int iCellsToSkip );

protected:
	
	string m_sLocalDirectory; /** < A directory to get files from */
	int m_iImageWidth; /** < image width duh */
	int m_iImageHeight; /** < image height */
	class ScreenHistory *m_pScreenHistory_Current; /** < The currently visible screen */
	class DesignObj_Orbiter *m_pObj_Highlighted; /** < The current object highlighted, changed with the scrolling functions */

	/** flags */
	
	bool m_bCaptureKeyboard_bOnOff; /** < flag for capture keyboard */
	bool m_bCaptureKeyboard_bReset; /** < flag for capture keyboard */
	bool m_bCaptureKeyboard_bTypePin; /** < flag for capture keyboard */
	bool m_bCaptureKeyboard_bDataGrid; /** < flag for capture keyboard */
	
	int m_dwPK_CaptureKeyboard_Variable; /** < capture keyboard variable (coresponds to primary key) @todo ask */
	string m_sCaptureKeyboard_Text; /** < text for capture keyboard @todo ask */
	string m_sCaptureKeyboard_InternalBuffer; /** < capture keyboard internal buffer */
	DesignObjText *m_pCaptureKeyboard_Text; /** < @todo ask */

	time_t m_LastActivityTime; /** < The last activity time */
	bool m_bDisplayOn; /** < False if the screen has blanked for the screen saver */
	bool m_bYieldScreen; /** < True if the orbiter should make the application desktop full screen ( hide itself ) */
	bool m_bYieldInput; /** < True if the orbiter should yield all input, like keyboard and mouse. This is useful when running the Orbiter as a Linux desktop */

	bool m_bRerenderScreen; /** <  */ // Set to true means ignore the objects to redraw, and just redraw the whole screen

	/**
	 * This is an internal counter. When we start doing stuff that may include lots of embedded commands which want to update the screen, we can create
	 * a NeedToRender object on the stack. Each one will increment this counter, and decrement in the destructor. When it's 0, it will call RedrawObjects automatically
	 * @todo ask if I placed this comment where it should be
	 */
	 
	int m_dwIDataGridRequestCounter; /** < For testing purposes we can log all datagrid requests */
	
	DesignObj_OrbiterMap m_mapObj_All; /** < All objects with the object ID in x.y.z.a.b format */
	map < string, DesignObj_DataList * > m_mapObj_AllNoSuffix; /** < All object with the object ID as a string */
	list < ScreenHistory * > m_listScreenHistory; /** < A history of the screens we've visited */
	list < class Device * > m_listDevice_Selected; /** < We can select multiple devices on the floorplan to send messages to, instead of the usual one */
	
	/**
	 * @brief stores objects that need to be redrawned
	 * When it's time to redraw some objects without redrawing the whole screen, store them here
	 * then call RedrawObjects(), rather than updating the screen over and over if several objects change at once
	 */
	vector < class DesignObj_Orbiter * > m_vectObjs_NeedRedraw;
	vector < class DesignObj_Orbiter * > m_vectObjs_TabStops; /** < All the tab stops */
	vector < class DesignObj_Orbiter * > m_vectObjs_Selected; /** < All the objects currently selected */
	vector < class DesignObj_DataGrid * > m_vectObjs_GridsOnScreen; /** < All the grids currently on the screen */

	/** for threads and shared memory control @todo ask */
	
	pthread_mutexattr_t m_MutexAttr; /** < @todo ask */
	pthread_cond_t m_MessageQueueCond; /** < @todo ask */
	pluto_pthread_mutex_t m_ScreenMutex; /** < Anything that should not be done during a screen render, change, etc. Blocking this will prevent screen changes */
	pluto_pthread_mutex_t m_VariableMutex; /** < Short mutex to protect members like strings and maps */
	pluto_pthread_mutex_t m_DatagridMutex; /** < Don't allow 2 threads to operate on datagrids at the same time */

	/** these methods are general purpose, and will call Orbiter-specific methods to do the work */
	
	/**
	 * @brief Render the screen in m_pScreenHistory_Current
	 */
	virtual void RenderScreen();
	
	/**
	 * @brief These will redraw any objects in m_vectObjsToRedraw. Use this to queue objects to redraw, such as those tht 
	 */
	virtual void RedrawObjects();
	
	/**
	 * @brief temp hack -- see comments
	 * @todo ask - add comments
	 */
	void RealRedraw( void *iData );
	
	/**
	 * @brief renders an object on the screen
	 * @todo ask
	 */
	virtual void RenderObject( DesignObj_Orbiter *pDesignObj_Orbiter, DesignObj_Orbiter *pDesignObj_Orbiter_Screen );
	
	/**
	 * @brief prepares the render datagrid for rendering
	 * @todo ask
	 */
	virtual void PrepareRenderDataGrid( DesignObj_DataGrid *pObj, string& sDelSelections );
	
	/**
	 * @brief renders the data grid
	 * @todo ask better comment
	 */
	virtual void RenderDataGrid( DesignObj_DataGrid *pObj );
	 
	/**
	 * @brief The framework will call this when it's time to change screens
	 * This immediately calls RenderScreen. Normally we add the current screen to the history list so we can go back again. However,
	 * if we're changing screens because of a GO BACK command, we won't do that
	 */
	virtual void NeedToChangeScreens( ScreenHistory *pScreenHistory, bool bAddToHistory = true );
	
	/**
	 * @brief
	 * @todo ask
	 */
	virtual void ObjectOnScreenWrapper();
	
	/**
	 * @brief puts the object on screen
	 * @param pObj pointer to the object to put on screen
	 * @todo ask (makesure)
	 */
	virtual void ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, bool bDontResetState );
	
	/**
	 * @brief takes out the specified object from screen
	 * @param pObj the object to remove from screen
	 * @todo ask
	 */
	virtual void ObjectOffScreen( DesignObj_Orbiter *pObj );
	
	/**
	 * @brief checks the special on screen conditions
	 * @todo ask
	 */
	virtual void CheckSpecialOnScreenConditions() {};
	
	/**
	 * @brief returnes the video frame from the DesignObj_Orbiter object pointed by iData
	 * @param iData points to the DesignObj_Orbiter object we want the video frame from
	 */
	void GetVideoFrame( void *iData );

	/**
	 * @brief
	 * @todo ask
	 */
	void SelectedObject( DesignObj_Orbiter *pDesignObj_Orbiter, int iX = 0, int iY = 0 );
	
	/**
	 * @brief
	 * @todo ask
	 */
	bool SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid, int iX, int iY );
	
	/**
	 * @brief
	 * @todo ask
	 */
	bool SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid, class DataGridCell *pCell );

	/**
	 * @brief
	 * @todo ask
	 */
	virtual void SpecialHandlingObjectSelected( DesignObj_Orbiter *pDesignObj_Orbiter ) {};

	/** The above methods are called by the orbiter-specific class to indicate input, they will call these following methods: */
	
	/**
	 * @brief returns true if the specified button was clicked
	 * @param dwPK_Button specifies the id for the button
	 * @todo ask
	 */
	bool ClickedButton( DesignObj_Orbiter *pDesignObj_Orbiter, int dwPK_Button );
	
	/**
	 * @brief returns true if the specified region was clicked
	 * @todo ask
	 */
	bool ClickedRegion( DesignObj_Orbiter *pDesignObj_Orbiter, int iX, int iY, DesignObj_Orbiter *&pTopMostAnimatedObject );

	/**
	 * Highlighting is used to show the currently 'highlighted' object, particularly with a remote that has up/down/left/right/enter controls
	 * These functions move between objects that have their 'tab stop' property set to true
	 */
	
	/**
	 * @brief Highlight this object
	 */
	virtual void HighlightObject( class DesignObj_Orbiter *pObj );
	
	/**
	 * @brief Find the first 'tab stop' object on screen and highlight it
	 */
	virtual void HighlightFirstObject();
	
	/**
	 * @brief A recursive loop for HighlightFirstObject
	 */
	virtual bool HighlightFirstChildObject( DesignObj_Orbiter* pObj );
	
	/**
	 * @brief This is used by the following function
	 */
	void FindObjectToHighlight( DesignObj_Orbiter **ppNextObjectToRight, DesignObj_Orbiter *pObj, int dwPK_Direction );
	
	/**
	 * @brief Given a direction ( UDLR ) find the object
	 */
	virtual void HighlightNextObject( int dwPK_Direction );

	/**
	 *	INITIALIZATION
	 */

	/**
	 * @brief initializises the orbiter with the specified graphic type
	 * @todo ask
	 */
	virtual void Initialize( GraphicType Type );
	
	/**
	 * @brief initializises the specified Grid
	 * @todo ask
	 */
	void InitializeGrid( DesignObj_DataGrid *pObj );
	
	/**
	 * @brief parses the configuration data based on the graphic type
	 * @todo ask
	 */
	bool ParseConfigurationData( GraphicType Type ); 
	
	/**
	 * @brief parses an object
	 * @todo ask
	 */
	void ParseObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen, DesignObj_Orbiter *pObj_Parent, GraphicType Type, int iLev );

	/**
	 *	PURE VIRTUALS
	 */
	
	/**
	 * @brief renders desktop
	 * Not all controllers will be able to do this. Mainly the 'iX' nested
	 * @todo ask
	 */
	virtual bool RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal );
	
	/**
	 * @brief renders a cell in the specified cell of the specified table at the specified coordinates
	 * @todo ask
	 */
	virtual bool RenderCell( class DesignObj_DataGrid *pObj, class DataGridTable *pT, class DataGridCell *pCell, int j, int i, int iGraphicToDisplay );
	
	/**
	 * @brief renders a graphic object in the specified rectangle
	 * @todo ask
	 */
	virtual void RenderGraphic( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, bool bDisableAspectRatio = false ) = 0;
	
	/**
	 * @brief renders text with the specified style
	 */
	virtual void RenderText( class DesignObjText *Text, class TextStyle *pTextStyle ) = 0;
	
	/**
	 * @brief draws an rectangle
	 */
	virtual void DrawRectangle( int iX, int iY, int iWidth, int iHeight, PlutoColor color, int iOpacity = 100 ) = 0;
	
	/**
	 * @brief draws an line
	 */
	virtual void DrawLine( int iX, int iY, int iWidth, int iHeight, PlutoColor color, int iOpacity = 100 ) = 0;
	
	/**
	 * @brief replaces the specified color from within the specified rectangle with another one
	 */
	virtual void ReplaceColorInRectangle( int iX, int iY, int iWidth, int iHeight, PlutoColor ColorToReplace, PlutoColor ReplacementColor ) = 0;
	
	/**
	 * @brief floods the region that containes the specified point, replacing one color 
	 * A graphic is no longer on screen. Maybe remove it from cache.
	 */
	virtual void FloodFill( int iX, int iY, PlutoColor ColorToReplace, PlutoColor ReplacementColor ) = 0;
	
	/**
	 * @brief filles the rectanle specified with a chosen color
	 */
	virtual void FillRectangle( int iX, int iY, int iWidth, int iHeight, PlutoColor color, int iOpacity = 100 ) = 0;
	
	/**
	 * @brief sets the time to the one specified by the parameter
	 */
	virtual void SetTime( char *pcServerTimeString ) = 0;
	
	/**
	 * @brief We're going to be redrawing something on top of this object. Save it's state, so that during the next redraw this will be used
	 */
	virtual void SaveBackgroundForDeselect( DesignObj_Orbiter *pObj ) = 0;

	/**
	 *	FUNCTIONS A DERIVED ORBITER MAY WANT TO IMPLEMENT 
	 */
	
	/**
	 * @brief A graphic is no longer on screen. Maybe remove it from cache
	 */
	virtual void GraphicOffScreen( class CHAGraphic *pGraphic ) {}
	
public:

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	 */

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_PK_Users();
	string DATA_Get_Current_Screen();
	void DATA_Set_Current_Screen( string Value );

			*****EVENT***** accessors inherited from base class
	void EVENT_Touch_or_click( int iX_Position, int iY_Position );

			*****COMMANDS***** we need to implement
	 */

/* 
	COMMAND: #1 - Capture Keyboard To Variable
	COMMENTS: As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object.
	PARAMETERS:
		#3 sPK_DesignObj
			The Design Object which contains the text Object
		#4 PK_Variable
			The variable in which to store the input
		#8 On/Off
			If 0, this stops capturing
		#14 Type
			1 = normal, 2 = pin, 3 = phone number
		#24 Reset
			if true, the next keypress will clear the variable and start new
		#25 PK_Text
			The text object in which to store the current input
		#55 DataGrid
			If 1, we'll scroll the iData grid too when typing keys.
 */
	virtual void CMD_Capture_Keyboard_To_Variable( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid ) { string sCMD_Result; CMD_Capture_Keyboard_To_Variable( sPK_DesignObj.c_str(), iPK_Variable, sOnOff.c_str(), sType.c_str(), sReset.c_str(), iPK_Text, bDataGrid, sCMD_Result, NULL );};
	virtual void CMD_Capture_Keyboard_To_Variable( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #2 - Orbiter Beep
	COMMENTS: Make the orbiter beep
	PARAMETERS:
 */
	virtual void CMD_Orbiter_Beep() { string sCMD_Result; CMD_Orbiter_Beep( sCMD_Result, NULL );};
	virtual void CMD_Orbiter_Beep( string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #3 - Display On/Off
	COMMENTS: Turn the display on or off
	PARAMETERS:
		#8 On/Off
			0 = Off, 1 = On
 */
	virtual void CMD_Display_OnOff( string sOnOff ) { string sCMD_Result; CMD_Display_OnOff( sOnOff.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Display_OnOff( string sOnOff, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #4 - Go back
	COMMENTS: Make the orbiter go back to the prior screen, like the back button in a web browser
	PARAMETERS:
		#16 PK_DesignObj_CurrentScreen
			If this is specified, the orbiter will ignore the command unless this is the current screen
		#21 Force
			Screens can be flagged, "Cant go back", meaning the go back will skip over them. If Force = 1, the Orbiter returns to the prior screen regardless
 */
	virtual void CMD_Go_back( string sPK_DesignObj_CurrentScreen, string sForce ) { string sCMD_Result; CMD_Go_back( sPK_DesignObj_CurrentScreen.c_str(), sForce.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Go_back( string sPK_DesignObj_CurrentScreen, string sForce, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #5 - Goto Screen
	COMMENTS: Goto a specific screen
	PARAMETERS:
		#2 PK_Device
			For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen. When the screen changes, it will be reset
		#3 sPK_DesignObj
			The screen to go to. Can be be fully qualified ( iX.iY.z ), or just contain the screen #
		#10 ID
			Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen. There can be lots of instances of the same screen in the history queue ( such as call in progress ). This allows a program to pop a particular one out of the queue.
		#16 PK_DesignObj_CurrentScreen
			If this is specified, the orbiter will ignore the command unless this is the current screen. If this is -1, that will match a main menu or screen saver ( ie the Orbiter is not in use ).
		#22 Store Variables
			If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen
 */
	virtual void CMD_Goto_Screen( int iPK_Device, string sPK_DesignObj, string sID, string sPK_DesignObj_CurrentScreen, bool bStore_Variables ) { string sCMD_Result; CMD_Goto_Screen( iPK_Device, sPK_DesignObj.c_str(), sID.c_str(), sPK_DesignObj_CurrentScreen.c_str(), bStore_Variables, sCMD_Result, NULL );};
	virtual void CMD_Goto_Screen( int iPK_Device, string sPK_DesignObj, string sID, string sPK_DesignObj_CurrentScreen, bool bStore_Variables, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #6 - Show Object
	COMMENTS: Change an objects visible state.
	PARAMETERS:
		#3 sPK_DesignObj
			The object to show or hide
		#4 PK_Variable
			The variable to use in the comparisson. See Comparisson Value.
		#6 Comparisson Operator
			A type of comparisson: = < < != 
		#7 Comparisson Value
			If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true
		#8 On/Off
			1 = show object, 2 = hide object
 */
	virtual void CMD_Show_Object( string sPK_DesignObj, int iPK_Variable, string sComparisson_Operator, string sComparisson_Value, string sOnOff ) { string sCMD_Result; CMD_Show_Object( sPK_DesignObj.c_str(), iPK_Variable, sComparisson_Operator.c_str(), sComparisson_Value.c_str(), sOnOff.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Show_Object( string sPK_DesignObj, int iPK_Variable, string sComparisson_Operator, string sComparisson_Value, string sOnOff, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #7 - Terminate Orbiter
	COMMENTS: Causes the Orbiter application to exit
	PARAMETERS:
 */
	virtual void CMD_Terminate_Orbiter() { string sCMD_Result; CMD_Terminate_Orbiter( sCMD_Result, NULL );};
	virtual void CMD_Terminate_Orbiter( string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #8 - Remove Screen From History
	COMMENTS: The orbiter keeps a history of visible screens, allowing the user to go back. See Go_Back. This removes screens from the queue that should not available anymore. An example is when a call comes in, the controllers are sent to an incoming call screen.
	PARAMETERS:
		#3 sPK_DesignObj
			The screen to remove
		#10 ID
			If specified, only screens that match this ID will be removed
 */
	virtual void CMD_Remove_Screen_From_History( string sPK_DesignObj, string sID ) { string sCMD_Result; CMD_Remove_Screen_From_History( sPK_DesignObj.c_str(), sID.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Remove_Screen_From_History( string sPK_DesignObj, string sID, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #9 - Scroll Grid
	COMMENTS: Scroll a datagrid
	PARAMETERS:
		#1 Relative Level
			The grid will scroll this many lines. If prefaced with a P, it will scroll this many pages. If not specified, it will scroll 1 page.
		#3 sPK_DesignObj
			The grid to scroll. If not specified, any currently visible grids will scroll
		#30 dwPK_Direction
			The direction to scroll the grid
 */
	virtual void CMD_Scroll_Grid( string sRelative_Level, string sPK_DesignObj, int iPK_Direction ) { string sCMD_Result; CMD_Scroll_Grid( sRelative_Level.c_str(), sPK_DesignObj.c_str(), iPK_Direction, sCMD_Result, NULL );};
	virtual void CMD_Scroll_Grid( string sRelative_Level, string sPK_DesignObj, int iPK_Direction, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #10 - Move Highlight
	COMMENTS: Move the current highlight pointer
	PARAMETERS:
		#1 Relative Level
			The grid will scroll this many lines. If prefaced with a P, it will scroll this many pages. If not specified, it will scroll 1 page.
		#3 sPK_DesignObj
			The grid to scroll. If not specified, any currently visible grids will scroll
		#30 dwPK_Direction
			The direction to move the highlight
 */
	virtual void CMD_Move_Highlight( string sRelative_Level, string sPK_DesignObj, int iPK_Direction ) { string sCMD_Result; CMD_Move_Highlight( sRelative_Level.c_str(), sPK_DesignObj.c_str(), iPK_Direction, sCMD_Result, NULL );};
	virtual void CMD_Move_Highlight( string sRelative_Level, string sPK_DesignObj, int iPK_Direction, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #13 - Play Sound
	COMMENTS: Plays a sound file on the orbiter
	PARAMETERS:
		#19 Data
			A pointer to a block of memory representing the sound file to play
		#20 Format
			Indicates what type of iData is in the memory block. 1 = wav, 2 = mp3
 */
	virtual void CMD_Play_Sound( char *pData, int iData_Size, string sFormat ) { string sCMD_Result; CMD_Play_Sound( pData, iData_Size, sFormat.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Play_Sound( char *pData, int iData_Size, string sFormat, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #14 - Refresh
	COMMENTS: Invalidates and redraws the current screen, optionally re-requesting iData from a datagrid. The OnLoad commands are not fired. See Regen Screen.
	PARAMETERS:
		#15 DataGrid ID
			Normally refresh does not cause the orbiter to re-request iData. But if a specific grid ID is specified, that grid will be refreshed. Specify * to re-request all grids on the current screen
 */
	virtual void CMD_Refresh( string sDataGrid_ID ) { string sCMD_Result; CMD_Refresh( sDataGrid_ID.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Refresh( string sDataGrid_ID, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #15 - Regen Screen
	COMMENTS: The screen is reloaded like the user was going to it for the first time. The OnUnload and OnLoad commands are fired.
	PARAMETERS:
 */
	virtual void CMD_Regen_Screen() { string sCMD_Result; CMD_Regen_Screen( sCMD_Result, NULL );};
	virtual void CMD_Regen_Screen( string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #16 - Requires Special Handling
	COMMENTS: When a button needs to do something too sophisticated for a normal command, attach this command. When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code.
	PARAMETERS:
 */
	virtual void CMD_Requires_Special_Handling() { string sCMD_Result; CMD_Requires_Special_Handling( sCMD_Result, NULL );};
	virtual void CMD_Requires_Special_Handling( string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #17 - Seek Data Grid
	COMMENTS: Causes a datagrid to seek to a particular position.
	PARAMETERS:
		#9 Text
			If specified, the orbiter will jump to the first row which has a cell that starts with this text. Specify Position iX to use a column other than the first one.
		#11 Position iX
			The column to jump to. If Text is not blank, the column to search.
		#12 Position iY
			The row to jump to. Ignored if Text is not blank
		#15 DataGrid ID
			The datagrid to scroll. If not specified, the first visible one will be used
 */
	virtual void CMD_Seek_Data_Grid( string sText, int iPosition_X, int iPosition_Y, string sDataGrid_ID ) { string sCMD_Result; CMD_Seek_Data_Grid( sText.c_str(), iPosition_X, iPosition_Y, sDataGrid_ID.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Seek_Data_Grid( string sText, int iPosition_X, int iPosition_Y, string sDataGrid_ID, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #18 - Set Graphic To Display
	COMMENTS: All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes". These are like "views". A Selected mode may appear depressed, for example. All children of this object will also be set.
	PARAMETERS:
		#3 sPK_DesignObj
			The object to set
		#10 ID
			0 = standard mode, -1 = selected -2 = highlight a positive number is one of the alternates
 */
	virtual void CMD_Set_Graphic_To_Display( string sPK_DesignObj, string sID ) { string sCMD_Result; CMD_Set_Graphic_To_Display( sPK_DesignObj.c_str(), sID.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Set_Graphic_To_Display( string sPK_DesignObj, string sID, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #19 - Set House Mode
	COMMENTS: change the house's mode
	PARAMETERS:
		#5 Value To Assign
			A value from the HouseMode table
		#18 Errors
			not used by the Orbiter. This is used only when sending the action to the core.
 */
	virtual void CMD_Set_House_Mode( string sValue_To_Assign, string sErrors ) { string sCMD_Result; CMD_Set_House_Mode( sValue_To_Assign.c_str(), sErrors.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Set_House_Mode( string sValue_To_Assign, string sErrors, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #20 - Set Object Parameter
	COMMENTS: changes one of the object's DesignObjParameters
	PARAMETERS:
		#3 sPK_DesignObj
			The object to change
		#5 Value To Assign
			The value to assign
		#27 PK_DesignObjParameter
			The parameter
 */
	virtual void CMD_Set_Object_Parameter( string sPK_DesignObj, string sValue_To_Assign, int iPK_DesignObjParameter ) { string sCMD_Result; CMD_Set_Object_Parameter( sPK_DesignObj.c_str(), sValue_To_Assign.c_str(), iPK_DesignObjParameter, sCMD_Result, NULL );};
	virtual void CMD_Set_Object_Parameter( string sPK_DesignObj, string sValue_To_Assign, int iPK_DesignObjParameter, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #21 - Set Object Position
	COMMENTS: Change an objects's position on the screen
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.iY.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			
		#12 Position iY
			
 */
	virtual void CMD_Set_Object_Position( string sPK_DesignObj, int iPosition_X, int iPosition_Y ) { string sCMD_Result; CMD_Set_Object_Position( sPK_DesignObj.c_str(), iPosition_X, iPosition_Y, sCMD_Result, NULL );};
	virtual void CMD_Set_Object_Position( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #22 - Set Object Size
	COMMENTS: Change an object's size
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.iY.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			
		#12 Position iY
			
 */
	virtual void CMD_Set_Object_Size( string sPK_DesignObj, int iPosition_X, int iPosition_Y ) { string sCMD_Result; CMD_Set_Object_Size( sPK_DesignObj.c_str(), iPosition_X, iPosition_Y, sCMD_Result, NULL );};
	virtual void CMD_Set_Object_Size( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #23 - Set Pos Rel To Parent
	COMMENTS: Like Set Object Position, but the iX and iY coordinates are assumed to be relative to the parent rather than absolute
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.iY.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			
		#12 Position iY
			
 */
	virtual void CMD_Set_Pos_Rel_To_Parent( string sPK_DesignObj, int iPosition_X, int iPosition_Y ) { string sCMD_Result; CMD_Set_Pos_Rel_To_Parent( sPK_DesignObj.c_str(), iPosition_X, iPosition_Y, sCMD_Result, NULL );};
	virtual void CMD_Set_Pos_Rel_To_Parent( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #24 - Set Size Rel To Parent
	COMMENTS: Change an object's size, relative to it's parent object
	PARAMETERS:
		#3 sPK_DesignObj
			The object to move. Can be a fully qualified object ( iX.iY.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position iX
			The percentage of the parent object's iWidth. 100 = the parent's full iWidth.
		#12 Position iY
			The percentage of the parent object's iHeight. 100 = the parent's full iHeight.
 */
	virtual void CMD_Set_Size_Rel_To_Parent( string sPK_DesignObj, int iPosition_X, int iPosition_Y ) { string sCMD_Result; CMD_Set_Size_Rel_To_Parent( sPK_DesignObj.c_str(), iPosition_X, iPosition_Y, sCMD_Result, NULL );};
	virtual void CMD_Set_Size_Rel_To_Parent( string sPK_DesignObj, int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #25 - Set Text
	COMMENTS: Change the text within a text object on the fly
	PARAMETERS:
		#3 sPK_DesignObj
			The Design Object which contains the text object. Can be a fully qualified object ( iX.iY.z ), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#9 Text
			The text to assign
		#25 PK_Text
			The text object in which to store the current input
 */
	virtual void CMD_Set_Text( string sPK_DesignObj, string sText, int iPK_Text ) { string sCMD_Result; CMD_Set_Text( sPK_DesignObj.c_str(), sText.c_str(), iPK_Text, sCMD_Result, NULL );};
	virtual void CMD_Set_Text( string sPK_DesignObj, string sText, int iPK_Text, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #26 - Set User Mode
	COMMENTS: Changes a user's mode	
	PARAMETERS:
		#5 Value To Assign
			A Value from the UserMode table
		#17 PK_Users
			The User to change
 */
	virtual void CMD_Set_User_Mode( string sValue_To_Assign, int iPK_Users ) { string sCMD_Result; CMD_Set_User_Mode( sValue_To_Assign.c_str(), iPK_Users, sCMD_Result, NULL );};
	virtual void CMD_Set_User_Mode( string sValue_To_Assign, int iPK_Users, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #27 - Set Variable
	COMMENTS: Change the value of a variable
	PARAMETERS:
		#4 PK_Variable
			The variable to change
		#5 Value To Assign
			The value to assign
 */
	virtual void CMD_Set_Variable( int iPK_Variable, string sValue_To_Assign ) { string sCMD_Result; CMD_Set_Variable( iPK_Variable, sValue_To_Assign.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Set_Variable( int iPK_Variable, string sValue_To_Assign, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #28 - Simulate Keypress
	COMMENTS: Simulates that a key has been touched. Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse. Also works with the "Capture Keyboard to Variable" command.
	PARAMETERS:
		#26 dwPK_Button
			What key to simulate being pressed
		#50 Name
			The application to send the keypress to. If not specified, it goes to the DCE device.
 */
	virtual void CMD_Simulate_Keypress( int idwPK_Button, string sName ) { string sCMD_Result; CMD_Simulate_Keypress( idwPK_Button, sName.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Simulate_Keypress( int idwPK_Button, string sName, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #29 - Simulate Mouse Click
	COMMENTS: Simulates a mouse click or touch on the indicated iX & iY coordinates
	PARAMETERS:
		#11 Position iX
			
		#12 Position iY
			
 */
	virtual void CMD_Simulate_Mouse_Click( int iPosition_X, int iPosition_Y ) { string sCMD_Result; CMD_Simulate_Mouse_Click( iPosition_X, iPosition_Y, sCMD_Result, NULL );};
	virtual void CMD_Simulate_Mouse_Click( int iPosition_X, int iPosition_Y, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #30 - Stop Sound
	COMMENTS: If a sound file is being played, it will be stopped.
	PARAMETERS:
 */
	virtual void CMD_Stop_Sound() { string sCMD_Result; CMD_Stop_Sound( sCMD_Result, NULL );};
	virtual void CMD_Stop_Sound( string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #31 - Store Variables
	COMMENTS: The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value.
	PARAMETERS:
 */
	virtual void CMD_Store_Variables() { string sCMD_Result; CMD_Store_Variables( sCMD_Result, NULL );};
	virtual void CMD_Store_Variables( string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #32 - Update Object Image
	COMMENTS: Changes the background image within an object
	PARAMETERS:
		#3 sPK_DesignObj
			The object in which to put the bitmap
		#14 Type
			1 = bmp, 2 = jpg, 3 = png
		#19 Data
			The contents of the bitmap, like reading from the file into a memory buffer
		#23 Disable Aspect Lock
			If 1, the image will be stretched to fit the object
 */
	virtual void CMD_Update_Object_Image( string sPK_DesignObj, string sType, char *pData, int iData_Size, string sDisable_Aspect_Lock ) { string sCMD_Result; CMD_Update_Object_Image( sPK_DesignObj.c_str(), sType.c_str(), pData, iData_Size, sDisable_Aspect_Lock.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Update_Object_Image( string sPK_DesignObj, string sType, char *pData, int iData_Size, string sDisable_Aspect_Lock, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #58 - Set Current User
	COMMENTS: Sets what user is currently using the orbiter.
	PARAMETERS:
		#17 PK_Users
			The user currently using the orbiter.
*/
	virtual void CMD_Set_Current_User(int iPK_Users) { string sCMD_Result; CMD_Set_Current_User(iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #66 - Select Object
	COMMENTS: The same as clicking on an object.
	PARAMETERS:
		#3 sPK_DesignObj
			The object to select.
 */
	virtual void CMD_Select_Object( string sPK_DesignObj ) { string sCMD_Result; CMD_Select_Object( sPK_DesignObj.c_str(), sCMD_Result, NULL );};
	virtual void CMD_Select_Object( string sPK_DesignObj, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #72 - Surrender to OS
	COMMENTS: Let the O/S take over. This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone. The orbiter will then let the other application ta
	PARAMETERS:
		#8 On/Off
			1 = Hide and let the OS take over. 0 = The orbiter comes up again.
		#54 Fully release keyboard
			Only applies if on/off is 1. If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch. If true, it will pass all keys.
 */
	virtual void CMD_Surrender_to_OS( string sOnOff, bool bFully_release_keyboard ) { string sCMD_Result; CMD_Surrender_to_OS( sOnOff.c_str(), bFully_release_keyboard, sCMD_Result, NULL );};
	virtual void CMD_Surrender_to_OS( string sOnOff, bool bFully_release_keyboard, string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #85 - Rest Highlight
	COMMENTS: Resets the currently highlighted object. Do this when you hide or unhide blocks that have tab stops.
	PARAMETERS:
 */
	virtual void CMD_Rest_Highlight() { string sCMD_Result; CMD_Rest_Highlight( sCMD_Result, NULL );};
	virtual void CMD_Rest_Highlight( string &sCMD_Result, Message *pMessage );

/* 
	COMMAND: #88 - Set Current Location
	COMMENTS: Sets the location the orbiter is in.  The location is a combination of room and entertainment area.
	PARAMETERS:
		#65 LocationID
			The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area.
*/
	virtual void CMD_Set_Current_Location(int iLocationID) { string sCMD_Result; CMD_Set_Current_Location(iLocationID,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->


};


extern int g_iDontRender; /** < @todo ask */
extern clock_t g_cLastTime; /** < time of he last input from the user @todo ask */

/**
 * @brief
 */
class NeedToRender
{
	class Orbiter *m_pOrbiter; /** <  */
	const char *m_pWhere; /** <  */
	
public:

	/**
	 * @brief constructor
	 */
	NeedToRender( class Orbiter *pOrbiter, const char *pWhere ) 
	{
		if( g_cLastTime && ( clock() - g_cLastTime ) > CLOCKS_PER_SEC * 3 && g_iDontRender )
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Need to render has blocked!!!" );
			g_iDontRender = 0;
		}
		m_pWhere = pWhere;
		m_pOrbiter = pOrbiter;
		g_cLastTime = clock();
		g_iDontRender++;
	}
	
	/**
	 * @brief destructor
	 */
	~NeedToRender()
	{
		g_iDontRender--;
		g_pPlutoLogger->Write( LV_STATUS, "Need To Render exit: %s m_dwIDontRender: %d", m_pWhere, g_iDontRender );
		if( g_iDontRender == 0 )
		{
			g_pPlutoLogger->Write( LV_STATUS, "calling redraw" );
			m_pOrbiter->RedrawObjects();
		}

	}
};

/**
 * @brief
 * @todo ask
 */
class CallBackInfo
{

public:

	Orbiter *m_pOrbiter; /** < pointer to the orbiter object */
	OrbiterCallBack m_fnCallBack; /** < @todo ask */
	clock_t m_clock; /** < @todo ask */
	void *m_pData; /** < @todo ask */
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e-> 
