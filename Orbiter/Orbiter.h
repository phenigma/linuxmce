//<-dceag-d-b->
#ifndef Orbiter_h
#define Orbiter_h

//	DCE Implemenation for #8 Orbiter

#include "Gen_Devices/OrbiterBase.h"
//<-dceag-d-e->

#include "DCE/Logger.h"
#include "OrbiterData.h"
#include "DesignObj_Orbiter.h"

// For brevity, DesignObj_Orbiter will be abbreviated Obj
namespace DCE
{

// Contains a snapshot of the screen that was viewed and the state so that it can be restored with GoBack
class ScreenHistory
{
public:
	ScreenHistory(class DesignObj_Orbiter *pObj,class ScreenHistory *pScreenHistory_Prior) 
	{ 
		m_pObj=pObj; m_iPK_Device=m_iPK_Users=0; m_bCantGoBack=false; m_pLocationInfo=NULL; 
		if( pScreenHistory_Prior )
		{
			// We'll just copy the standard stuff from the prior screen
			m_iPK_Device = pScreenHistory_Prior->m_iPK_Device;
			m_iPK_Users = pScreenHistory_Prior->m_iPK_Users;
			m_pLocationInfo = pScreenHistory_Prior->m_pLocationInfo;
		}
	}
	string m_sID; // A unique ID that can be specified in the Goto command, allowing a particular 'instance' of a screen to be removed in 'remove screen from history'
	int m_iPK_Device;  // The device being controlled
	int m_iPK_Users; // The current user
	class LocationInfo *m_pLocationInfo; // The current location
	int m_bCantGoBack;  // If we get a go back, skip over this screen unless "Force" is set to true
	class DesignObj_Orbiter *m_pObj;  // The screen we're viewing
	map<int,string> m_mapVariables;  // Any variables we need to restore when returning to this screen
};

typedef void (Orbiter::*OrbiterCallBack)(void *data);

//<-dceag-decl-b->! custom
class Orbiter : public Orbiter_Command, public OrbiterData
{
//<-dceag-decl-e->
	friend class BD_PC_SetVariable;  // Needs to maniuplate our variables
	friend class NeedToRender;
//<-dceag-const-b->!
public:
	// Constructors/Destructor
	Orbiter(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, int iImageWidth, int iImageHeight);
	virtual ~Orbiter();
	virtual bool Register();
//<-dceag-const-e->
	// Private member variables

	// Private methods

protected:
	// Protected member variables
	string m_sLocalDirectory;	// A directory to get files from
	int m_iImageWidth, m_iImageHeight;
	class ScreenHistory *m_pScreenHistory_Current;  // The currently visible screen
	class DesignObj_Orbiter *m_pObj_Highlighted;   // The current object highlighted, changed with the scrolling functions

	bool m_CaptureKeyboard_bOnOff;
	bool m_CaptureKeyboard_bReset;
	bool m_CaptureKeyboard_bTypePin;
	bool m_CaptureKeyboard_bDataGrid;
	int  m_CaptureKeyboard_iPK_Variable;
	string m_CaptureKeyboard_sText;
	string m_CaptureKeyboard_InternalBuffer;
	DesignObjText *m_CaptureKeyboard_pText;

	time_t m_LastActivityTime;  // The last activity 
	bool m_bDisplayOn; // False if the screen has blanked for the screen saver
	bool m_bYieldScreen; // True if the orbiter should make the application desktop full screen (hide itself)
	bool m_bYieldInput; // True if the orbiter should yield all input, like keyboard and mouse.  This is useful when running the Orbiter as a Linux desktop

	// This is an internal counter.  When we start doing stuff that may include lots of embedded commands which want to update the screen, we can create
	// a NeedToRender object on the stack.  Each one will increment this counter, and decrement in the destructor.  When it's 0, it will call RedrawObjects automatically
	bool m_bRerenderScreen;  // Set to true means ignore the objects to redraw, and just redraw the whole screen

	int m_iDataGridRequestCounter;  // For testing purposes we can log all datagrid requests
	DesignObj_OrbiterMap m_mapObj_All;	// All objects with the object ID in x.y.z.a.b format
	map<string,DesignObj_DataList *> m_mapObj_AllNoSuffix;  // All object with the object ID as a string
	list<ScreenHistory *> m_listScreenHistory; // A history of the screens we've visited
	list<class Device *> m_listDevice_Selected; // We can select multiple devices on the floorplan to send messages to, instead of the usual one
	vector<class DesignObj_Orbiter *> m_vectObjs_NeedRedraw, // When it's time to redraw some objects without redrawing the whole screen, store them here
															// then call RedrawObjects(), rather than updating the screen over and over if several objects change at once
									m_vectObjs_TabStops,	// All the tab stops
									m_vectObjs_Selected;  // All the objects currently selected
	vector<class DesignObj_DataGrid *> m_vectObjs_GridsOnScreen;  // All the grids currently on the screen

	pthread_mutexattr_t m_MutexAttr;
	pthread_cond_t m_MessageQueueCond;
	pluto_pthread_mutex_t	m_ScreenMutex,// Anything that should not be done during a screen render, change, etc.  Blocking this will prevent screen changes
						m_VariableMutex, // Short mutex to protect members like strings and maps
						m_DatagridMutex; // Don't allow 2 threads to operate on datagrids at the same time

	/*
		GENERAL PURPOSE METHODS
	*/

	// These methods are general purpose, and will call Orbiter-specific methods to do the work
	virtual void RenderScreen();	// Render the screen in m_pScreenHistory_Current
	virtual void RedrawObjects();   // These will redraw any objects in m_vectObjsToRedraw.  Use this to queue objects to redraw, such as those tht 
void RealRedraw(void *data);  // temp hack -- see comments
	virtual void RenderObject(DesignObj_Orbiter *pDesignObj_Orbiter, DesignObj_Orbiter *pDesignObj_Orbiter_Screen);
	virtual void PrepareRenderDataGrid(DesignObj_DataGrid *pObj, string& DelSelections);
	virtual void RenderDataGrid(DesignObj_DataGrid *pObj);
	// The framework will call this when it's time to change screens.  This immediately calls RenderScreen.  Normally we add the current
	// screen to the history list so we can go back again.  However, if we're changing screens because of a GO BACK command, we won't do that
	virtual void NeedToChangeScreens(ScreenHistory *pScreenHistory,bool bAddToHistory=true);
	virtual void ObjectOnScreenWrapper();
	virtual void ObjectOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter,DesignObj_Orbiter *pObj,bool bDontResetState);
	virtual void ObjectOffScreen(DesignObj_Orbiter *pObj);
	virtual void CheckSpecialOnScreenConditions() {};
	void GetVideoFrame(void *data);

	void SelectedObject(DesignObj_Orbiter *pDesignObj_Orbiter, int X=0, int Y=0);
	bool SelectedGrid(DesignObj_DataGrid *pDesignObj_DataGrid, int X, int Y);
	bool SelectedGrid(DesignObj_DataGrid *pDesignObj_DataGrid, class DataGridCell *pCell);

	virtual void SpecialHandlingObjectSelected(DesignObj_Orbiter *pDesignObj_Orbiter) {};

	// The above methods are called by the orbiter-specific class to indicate input, they will call these following methods:
	bool ClickedButton(DesignObj_Orbiter *pDesignObj_Orbiter,int PK_Button);
	bool ClickedRegion(DesignObj_Orbiter *pDesignObj_Orbiter,int X,int Y,DesignObj_Orbiter *&pTopMostAnimatedObject);

	// Highlighting is used to show the currently 'highlighted' object, particularly with a remote that has up/down/left/right/enter controls
	// These functions move between objects that have their 'tab stop' property set to true
	virtual void HighlightObject(class DesignObj_Orbiter *pObj);  // Highlight this object
	virtual void HighlightFirstObject();  // Find the first 'tab stop' object on screen and highlight it
	virtual bool HighlightFirstChildObject(DesignObj_Orbiter* pObj);  // A recursive loop for HighlightFirstObject
	void FindObjectToHighlight(DesignObj_Orbiter **ppNextObjectToRight, DesignObj_Orbiter *pObj, int PK_Direction);  // This is used by the following function.
	virtual void HighlightNextObject(int PK_Direction); // Given a direction (UDLR) find the object.

	/*
		INITIALIZATION
	*/

	virtual void Initialize(GraphicType Type);
	void InitializeGrid(DesignObj_DataGrid *pObj);
	bool ParseConfigurationData(GraphicType Type);  
	void ParseObject(DesignObj_Orbiter *pObj,DesignObj_Orbiter *pObj_Screen,DesignObj_Orbiter *pObj_Parent,GraphicType Type, int Lev);

	/*
		PURE VIRTUALS
	*/
	virtual bool RenderDesktop(class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal); // Not all controllers will be able to do this.  Mainly the 'x' nested
	virtual bool RenderCell(class DesignObj_DataGrid *pObj, class DataGridTable *pT, class DataGridCell *pCell, int j, int i, int GraphicToDisplay);
	virtual void RenderGraphic(class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, bool bDisableAspectRatio=false)=0;
	virtual void RenderText(class DesignObjText *Text,class TextStyle *pTextStyle)=0;
	virtual void DrawRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity = 100) = 0;
	virtual void DrawLine(int x, int y, int width, int height, PlutoColor color, int Opacity = 100) = 0;
	virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor) = 0;
	virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) = 0;
	virtual void FillRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity=100) = 0;
	virtual void SetTime(char *ServerTimeString) = 0;
	virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj)=0;  // We're going to be redrawing something on top of this object.  Save it's state, so that during the next redraw this will be used

	/*
		FUNCTIONS A DERIVED ORBITER MAY WANT TO IMPLEMENT 
	*/

	virtual void GraphicOffScreen(class CHAGraphic *pGraphic) {}  // A graphic is no longer on screen.  Maybe remove it from cache.

public:

	/*
		ACCEPT OUTSIDE INPUT
	*/
	virtual bool ButtonDown(int PK_C_Button);
	virtual bool ButtonUp();
	virtual bool RegionDown(int x, int y);
	virtual bool RegionUp() { return false; };  // We don't handle this here
	virtual void GotActivity(); // Something happened, like a touch or a button, reset any timeouts or screen saver

	/*
		UTILITIES
	*/
	virtual DesignObjText *FindText(DesignObj_Orbiter *pObj, int iPK_Text);

	virtual bool ReceivedMessage(class Message *pMessageOriginal); // Redraw any objects that change during execution of a command

	bool BuildCaptureKeyboardParams(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid);
	bool CaptureKeyboard_EditText_DeleteLastChar();
	bool CaptureKeyboard_EditText_AppendChar(char ch);
	void CaptureKeyboard_UpdateVariableAndText(int iVariable, string sVariable);

	DesignObj_Orbiter *FindObject(string PK_DesignObj,class DesignObj_Orbiter *pDesignObj_Orbiter=NULL); // Find an object
	DesignObj_Orbiter *FindSingleNumberObject(int PKID_Object, DesignObj_Orbiter *pObj);
	void ExecuteCommandsInList(DesignObjCommandList *pDesignObjCommandList, DesignObj_Orbiter *pDesignObj_Orbiter, int X=0, int Y=0);  // Execute commands
	void GetGridCellDimensions(DesignObj_DataGrid *pObj, int ColSpan, int RowSpan, int Column, int Row, int &x, int &y, int &w, int &h);
	string SubstituteVariables(string Input, DesignObj_Orbiter *pDesignObj_Orbiter, int X, int Y);
	void GotoScreen(string sScreen,string sID="") { CMD_Goto_Screen(0,sScreen,sID,"",false); }  // A helper function we can call internally rather than the full CMD_GotoScreen
	bool DoComparisson(int PK_Variable, string oComparisson, string oValue);
	void FindDGArrows(DesignObj_Orbiter *pObj,DesignObj_DataGrid *pDGObj);
	bool AcquireGrid(DesignObj_DataGrid *pObj, int GridCurCol, int GridCurRow, class DataGridTable* &pDataGridTable);
	bool SelectedGrid(int DGRow);

	void CallMaintenanceAtTime(time_t t,OrbiterCallBack fnCallBack,void *data) { CallMaintenanceInTicks((clock_t) (t-time(NULL)) * CLOCKS_PER_SEC,fnCallBack,data); } // We need the maintenance function to be called at this time
	void CallMaintenanceInTicks(clock_t c,OrbiterCallBack fnCallBack,void *data);  // We need the maintenance function to be called in this many clock ticks

	/*
		MAINTENANCE CALL BACKS

		When you need something to be done at a particular time, like popup buttons, call 'CallMaintenance', passing in the time you need the function
		called, and any data you want the function to get, as a void *.  The call backs must be void Function(void *data) syntax.
	*/

	void DeselectObjects(void *data);  // data is a pointer to an object to deselect


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_PK_Users();
	string DATA_Get_Current_Screen();
	void DATA_Set_Current_Screen(string Value);

			*****EVENT***** accessors inherited from base class
	void EVENT_Touch_or_click(int iX_Position,int iY_Position);

			*****COMMANDS***** we need to implement
	*/

/* 
	COMMAND: #1 - Capture Keyboard To Variable
	COMMENTS: As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object.
	PARAMETERS:
		#3 PK_DesignObj
			The Design Object which contains the text Object
		#4 PK_Variable
			The variable in which to store the input
		#8 On/Off
			If 0, this stops capturing
		#14 Type
			1=normal, 2=pin, 3=phone number
		#24 Reset
			if true, the next keypress will clear the variable and start new
		#25 PK_Text
			The text object in which to store the current input
		#55 DataGrid
			If 1, we'll scroll the data grid too when typing keys.
*/
	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { string sCMD_Result; CMD_Capture_Keyboard_To_Variable(sPK_DesignObj.c_str(),iPK_Variable,sOnOff.c_str(),sType.c_str(),sReset.c_str(),iPK_Text,bDataGrid,sCMD_Result,NULL);};
	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #2 - Orbiter Beep
	COMMENTS: Make the orbiter beep
	PARAMETERS:
*/
	virtual void CMD_Orbiter_Beep() { string sCMD_Result; CMD_Orbiter_Beep(sCMD_Result,NULL);};
	virtual void CMD_Orbiter_Beep(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #3 - Display On/Off
	COMMENTS: Turn the display on or off
	PARAMETERS:
		#8 On/Off
			0=Off, 1=On
*/
	virtual void CMD_Display_OnOff(string sOnOff) { string sCMD_Result; CMD_Display_OnOff(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Display_OnOff(string sOnOff,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #4 - Go back
	COMMENTS: Make the orbiter go back to the prior screen, like the back button in a web browser
	PARAMETERS:
		#16 PK_DesignObj_CurrentScreen
			If this is specified, the orbiter will ignore the command unless this is the current screen
		#21 Force
			Screens can be flagged, "Cant go back", meaning the go back will skip over them.  If Force=1, the Orbiter returns to the prior screen regardless
*/
	virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce) { string sCMD_Result; CMD_Go_back(sPK_DesignObj_CurrentScreen.c_str(),sForce.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #5 - Goto Screen
	COMMENTS: Goto a specific screen
	PARAMETERS:
		#2 PK_Device
			For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen.  When the screen changes, it will be reset
		#3 PK_DesignObj
			The screen to go to.  Can be be fully qualified (x.y.z), or just contain the screen #
		#10 ID
			Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue.
		#16 PK_DesignObj_CurrentScreen
			If this is specified, the orbiter will ignore the command unless this is the current screen.  If this is -1, that will match a main menu or screen saver (ie the Orbiter is not in use).
		#22 Store Variables
			If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen
*/
	virtual void CMD_Goto_Screen(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables) { string sCMD_Result; CMD_Goto_Screen(iPK_Device,sPK_DesignObj.c_str(),sID.c_str(),sPK_DesignObj_CurrentScreen.c_str(),bStore_Variables,sCMD_Result,NULL);};
	virtual void CMD_Goto_Screen(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #6 - Show Object
	COMMENTS: Change an objects visible state.
	PARAMETERS:
		#3 PK_DesignObj
			The object to show or hide
		#4 PK_Variable
			The variable to use in the comparisson.  See Comparisson Value.
		#6 Comparisson Operator
			A type of comparisson: =  <  <>  !=  >
		#7 Comparisson Value
			If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true
		#8 On/Off
			1=show object, 2=hide object
*/
	virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { string sCMD_Result; CMD_Show_Object(sPK_DesignObj.c_str(),iPK_Variable,sComparisson_Operator.c_str(),sComparisson_Value.c_str(),sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #7 - Terminate Orbiter
	COMMENTS: Causes the Orbiter application to exit
	PARAMETERS:
*/
	virtual void CMD_Terminate_Orbiter() { string sCMD_Result; CMD_Terminate_Orbiter(sCMD_Result,NULL);};
	virtual void CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #8 - Remove Screen From History
	COMMENTS: The orbiter keeps a history of visible screens, allowing the user to go back.  See Go_Back.  This removes screens from the queue that should not available anymore.  An example is when a call comes in, the controllers are sent to an incoming call screen.
	PARAMETERS:
		#3 PK_DesignObj
			The screen to remove
		#10 ID
			If specified, only screens that match this ID will be removed
*/
	virtual void CMD_Remove_Screen_From_History(string sPK_DesignObj,string sID) { string sCMD_Result; CMD_Remove_Screen_From_History(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Remove_Screen_From_History(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #9 - Scroll Grid
	COMMENTS: Scroll a datagrid
	PARAMETERS:
		#1 Relative Level
			The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page.
		#3 PK_DesignObj
			The grid to scroll.  If not specified, any currently visible grids will scroll
		#30 PK_Direction
			The direction to scroll the grid
*/
	virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Scroll_Grid(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
	virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #10 - Move Highlight
	COMMENTS: Move the current highlight pointer
	PARAMETERS:
		#1 Relative Level
			The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page.
		#3 PK_DesignObj
			The grid to scroll.  If not specified, any currently visible grids will scroll
		#30 PK_Direction
			The direction to move the highlight
*/
	virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Move_Highlight(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
	virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #13 - Play Sound
	COMMENTS: Plays a sound file on the orbiter
	PARAMETERS:
		#19 Data
			A pointer to a block of memory representing the sound file to play
		#20 Format
			Indicates what type of data is in the memory block.  1=wav, 2=mp3
*/
	virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat) { string sCMD_Result; CMD_Play_Sound(pData,iData_Size,sFormat.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #14 - Refresh
	COMMENTS: Invalidates and redraws the current screen, optionally re-requesting data from a datagrid.  The OnLoad commands are not fired.  See Regen Screen.
	PARAMETERS:
		#15 DataGrid ID
			Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen
*/
	virtual void CMD_Refresh(string sDataGrid_ID) { string sCMD_Result; CMD_Refresh(sDataGrid_ID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #15 - Regen Screen
	COMMENTS: The screen is reloaded like the user was going to it for the first time.  The OnUnload and OnLoad commands are fired.
	PARAMETERS:
*/
	virtual void CMD_Regen_Screen() { string sCMD_Result; CMD_Regen_Screen(sCMD_Result,NULL);};
	virtual void CMD_Regen_Screen(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #16 - Requires Special Handling
	COMMENTS: When a button needs to do something too sophisticated for a normal command, attach this command.  When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code.
	PARAMETERS:
*/
	virtual void CMD_Requires_Special_Handling() { string sCMD_Result; CMD_Requires_Special_Handling(sCMD_Result,NULL);};
	virtual void CMD_Requires_Special_Handling(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #17 - Seek Data Grid
	COMMENTS: Causes a datagrid to seek to a particular position.
	PARAMETERS:
		#9 Text
			If specified, the orbiter will jump to the first row which has a cell that starts with this text.  Specify Position X to use a column other than the first one.
		#11 Position X
			The column to jump to.  If Text is not blank, the column to search.
		#12 Position Y
			The row to jump to.  Ignored if Text is not blank
		#15 DataGrid ID
			The datagrid to scroll.  If not specified, the first visible one will be used
*/
	virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { string sCMD_Result; CMD_Seek_Data_Grid(sText.c_str(),iPosition_X,iPosition_Y,sDataGrid_ID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #18 - Set Graphic To Display
	COMMENTS: All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes".  These are like "views".  A Selected mode may appear depressed, for example.  All children of this object will also be set.
	PARAMETERS:
		#3 PK_DesignObj
			The object to set
		#10 ID
			0=standard mode, -1=selected -2=highlight a positive number is one of the alternates
*/
	virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID) { string sCMD_Result; CMD_Set_Graphic_To_Display(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #19 - Set House Mode
	COMMENTS: change the house's mode
	PARAMETERS:
		#5 Value To Assign
			A value from the HouseMode table
		#18 Errors
			not used by the Orbiter.  This is used only when sending the action to the core.
*/
	virtual void CMD_Set_House_Mode(string sValue_To_Assign,string sErrors) { string sCMD_Result; CMD_Set_House_Mode(sValue_To_Assign.c_str(),sErrors.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_House_Mode(string sValue_To_Assign,string sErrors,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #20 - Set Object Parameter
	COMMENTS: changes one of the object's DesignObjParameters
	PARAMETERS:
		#3 PK_DesignObj
			The object to change
		#5 Value To Assign
			The value to assign
		#27 PK_DesignObjParameter
			The parameter
*/
	virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { string sCMD_Result; CMD_Set_Object_Parameter(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),iPK_DesignObjParameter,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #21 - Set Object Position
	COMMENTS: Change an objects's position on the screen
	PARAMETERS:
		#3 PK_DesignObj
			The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position X
			
		#12 Position Y
			
*/
	virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Position(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #22 - Set Object Size
	COMMENTS: Change an object's size
	PARAMETERS:
		#3 PK_DesignObj
			The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position X
			
		#12 Position Y
			
*/
	virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Size(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #23 - Set Pos Rel To Parent
	COMMENTS: Like Set Object Position, but the X and Y coordinates are assumed to be relative to the parent rather than absolute
	PARAMETERS:
		#3 PK_DesignObj
			The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position X
			
		#12 Position Y
			
*/
	virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Pos_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #24 - Set Size Rel To Parent
	COMMENTS: Change an object's size, relative to it's parent object
	PARAMETERS:
		#3 PK_DesignObj
			The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#11 Position X
			The percentage of the parent object's width.  100=the parent's full width.
		#12 Position Y
			The percentage of the parent object's height.  100=the parent's full height.
*/
	virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Size_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #25 - Set Text
	COMMENTS: Change the text within a text object on the fly
	PARAMETERS:
		#3 PK_DesignObj
			The Design Object which contains the text object.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen.
		#9 Text
			The text to assign
		#25 PK_Text
			The text object in which to store the current input
*/
	virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text) { string sCMD_Result; CMD_Set_Text(sPK_DesignObj.c_str(),sText.c_str(),iPK_Text,sCMD_Result,NULL);};
	virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #26 - Set User Mode
	COMMENTS: Changes a user's mode
	PARAMETERS:
		#5 Value To Assign
			A Value from the UserMode table
		#17 PK_Users
			The User to change
*/
	virtual void CMD_Set_User_Mode(string sValue_To_Assign,int iPK_Users) { string sCMD_Result; CMD_Set_User_Mode(sValue_To_Assign.c_str(),iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_User_Mode(string sValue_To_Assign,int iPK_Users,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #27 - Set Variable
	COMMENTS: Change the value of a variable
	PARAMETERS:
		#4 PK_Variable
			The variable to change
		#5 Value To Assign
			The value to assign
*/
	virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign) { string sCMD_Result; CMD_Set_Variable(iPK_Variable,sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #28 - Simulate Keypress
	COMMENTS: Simulates that a key has been touched.  Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse.  Also works with the "Capture Keyboard to Variable" command.
	PARAMETERS:
		#26 PK_Button
			What key to simulate being pressed
		#50 Name
			The application to send the keypress to. If not specified, it goes to the DCE device.
*/
	virtual void CMD_Simulate_Keypress(int iPK_Button,string sName) { string sCMD_Result; CMD_Simulate_Keypress(iPK_Button,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(int iPK_Button,string sName,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #29 - Simulate Mouse Click
	COMMENTS: Simulates a mouse click or touch on the indicated x & y coordinates
	PARAMETERS:
		#11 Position X
			
		#12 Position Y
			
*/
	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #30 - Stop Sound
	COMMENTS: If a sound file is being played, it will be stopped.
	PARAMETERS:
*/
	virtual void CMD_Stop_Sound() { string sCMD_Result; CMD_Stop_Sound(sCMD_Result,NULL);};
	virtual void CMD_Stop_Sound(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #31 - Store Variables
	COMMENTS: The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value.
	PARAMETERS:
*/
	virtual void CMD_Store_Variables() { string sCMD_Result; CMD_Store_Variables(sCMD_Result,NULL);};
	virtual void CMD_Store_Variables(string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #32 - Update Object Image
	COMMENTS: Changes the background image within an object
	PARAMETERS:
		#3 PK_DesignObj
			The object in which to put the bitmap
		#14 Type
			1=bmp, 2=jpg, 3=png
		#19 Data
			The contents of the bitmap, like reading from the file into a memory buffer
		#23 Disable Aspect Lock
			If 1, the image will be stretched to fit the object
*/
	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { string sCMD_Result; CMD_Update_Object_Image(sPK_DesignObj.c_str(),sType.c_str(),pData,iData_Size,sDisable_Aspect_Lock.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #66 - Select Object
	COMMENTS: The same as clicking on an object.
	PARAMETERS:
		#3 PK_DesignObj
			The object to select.
*/
	virtual void CMD_Select_Object(string sPK_DesignObj) { string sCMD_Result; CMD_Select_Object(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Select_Object(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #72 - Surrender to OS
	COMMENTS: Let the O/S take over.  This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone.  The orbiter will then let the other application ta
	PARAMETERS:
		#8 On/Off
			1=Hide and let the OS take over.  0=The orbiter comes up again.
		#54 Fully release keyboard
			Only applies if on/off is 1.  If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch.  If true, it will pass all keys.
*/
	virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard) { string sCMD_Result; CMD_Surrender_to_OS(sOnOff.c_str(),bFully_release_keyboard,sCMD_Result,NULL);};
	virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #85 - Rest Highlight
	COMMENTS: Resets the currently highlighted object.  Do this when you hide or unhide blocks that have tab stops.
	PARAMETERS:
*/
	virtual void CMD_Rest_Highlight() { string sCMD_Result; CMD_Rest_Highlight(sCMD_Result,NULL);};
	virtual void CMD_Rest_Highlight(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->




// temporary solution only
void CalculateGridUp(DesignObj_DataGrid *pObj, int &CurCol, int CellsToSkip);
void CalculateGridDown(DesignObj_DataGrid *pObj, int &CurCol, int CellsToSkip);
void CalculateGridLeft(DesignObj_DataGrid *pObj, int &CurCol, int CellsToSkip);
void CalculateGridRight(DesignObj_DataGrid *pObj, int &CurCol, int CellsToSkip);


};


extern int g_iDontRender;
extern clock_t g_cLastTime; // debug only
class NeedToRender
{
	class Orbiter *m_pOrbiter;
	const char *m_pWhere;
public:
	NeedToRender(class Orbiter *pOrbiter,const char *pWhere) 
	{
		if( g_cLastTime && (clock() - g_cLastTime)>CLOCKS_PER_SEC * 3 && g_iDontRender )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Need to render has blocked!!!");
			g_iDontRender=0;
		}
		m_pWhere=pWhere;
		m_pOrbiter=pOrbiter;
		g_cLastTime=clock();
		g_iDontRender++;
	}
	~NeedToRender()
	{
		g_iDontRender--;
		g_pPlutoLogger->Write(LV_STATUS,"Need To Render exit: %s m_iDontRender: %d",m_pWhere,g_iDontRender);
		if( g_iDontRender==0 )
		{
			g_pPlutoLogger->Write(LV_STATUS,"calling redraw");
			m_pOrbiter->RedrawObjects();
		}

	}
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
