/**
 *
 * @file Orbiter.h
 * @brief header file for the ScreenHistory,  Orbiter and NeedToRender classes
 * @author
 * @todo notcommented
 * @warning do not change the code between the tags(  <-dceag-...-b-> < -dceag-...-e->  ) ,  it will be overriden by the generator (  unless '!' specified  )
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
#include "Floorplan.h"
#include "CacheImageManager.h"

/** For brevity,  DesignObj_Orbiter will be abbreviated Obj */

namespace DCE
{

/**
 * @brief Contains a snapshot of the screen that was viewed and the state so that it can be restored with GoBack
 */
class ScreenHistory
{

public:

	/**
	 * @brief constructor
	 */
	ScreenHistory( class DesignObj_Orbiter *pObj, class ScreenHistory *pScreenHistory_Prior )
	{
		m_tTime = time(NULL);
		m_pObj=pObj; 
		m_dwPK_Device=0;
		m_bCantGoBack=false; 
		if(  pScreenHistory_Prior  )
		{
			m_dwPK_Device = pScreenHistory_Prior->m_dwPK_Device;
		}
	}

	/**
	 * @brief A unique ID that can be specified in the Goto command
	 * A unique ID that can be specified in the Goto command, allowing a particular 'instance'
	 * of a screen to be removed in 'remove screen from history'
	 */
	string m_sID;
	time_t m_tTime;
	int m_dwPK_Device; /** < The device being controlled */
	bool m_bCantGoBack; /** < If we get a go back, skip over this screen unless "Force" is set to true */
	class DesignObj_Orbiter *m_pObj; /** < The screen we're viewing */
	map<int, string> m_mapVariable; /** < Any variables we need to restore when returning to this screen */
};

typedef void ( Orbiter::*OrbiterCallBack )( void *data );
enum ePurgeExisting { pe_NO=0, pe_ALL, pe_Match_Data };

//todo : need to serialize it ? 
class PlutoPopup
{
public:
    string m_sName;
    PlutoPoint m_Position;
    bool m_bDontAutohide;
	DesignObj_Orbiter *m_pObj;

    PlutoPopup(DesignObj_Orbiter *pObj, string sName, PlutoPoint Position)
    {
		m_pObj = pObj;
        m_sName = sName;
        m_Position = Position;    
		m_bDontAutohide = false;
    }

	~PlutoPopup()
	{
int k=2;
	}
};

//<-dceag-decl-b->! custom
/**
 * @brief
 */
class Orbiter : public Orbiter_Command,  public OrbiterData
{
void DumpScreenHistory(); // temporary function

//<-dceag-decl-e->
	friend class BD_PC_SetVariable; /** < Needs to maniuplate our variables */
	friend class NeedToRender; /** < Needs to maniuplate our variables */

public: //data
	int m_iImageWidth; /** < image width duh */
	int m_iImageHeight; /** < image height */

//<-dceag-const-b->!

public:

	/**
	 * @brief constructor, assignes values to member data
	 */
	Orbiter( int DeviceID,  string ServerAddress,  string sLocalDirectory,  bool bLocalMode,  int iImageWidth,  int iImageHeight );

	/**
	 * @brief destructor
	 */
	virtual ~Orbiter(  );

	/**
	 * This function will only be used if this device is loaded into the DCE Router's memory space
	 * as a plug-in. Otherwise Connect() will be called from the main()
	 */
	virtual bool Register(  );

	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);

	time_t GetLastScreenChangedTime();
	string GetCurrentScreenID();

	/**
	* @brief searches for updates, returns true if an update was found
	* @todo ask
	*/
	virtual bool SelfUpdate() { return false; };

	pluto_pthread_mutex_t m_NeedRedrawVarMutex; //this will protect needredraw vectors
	pluto_pthread_mutex_t m_MaintThreadMutex;  // This will also protect the callback map
	pthread_cond_t m_MaintThreadCond;

//<-dceag-const-e->

protected:

	int m_dwPK_Users; /** < The current user */
	class LocationInfo *m_pLocationInfo; /** < The current location */

	string m_sLocalDirectory; /** < A directory to get files from */
	ScreenHistory *m_pScreenHistory_Current; /** < The currently visible screen */
	class DesignObj_Orbiter *m_pObj_Highlighted; /** < The current object highlighted, changed with the scrolling functions */
	class DesignObj_Orbiter *m_pObj_LastSelected;   // The last object we selected.  Used by floorplans to toggle states
	int m_iLastEntryInDeviceGroup; // Used by floorplans to go through a selected device group
	map<int,class DeviceGroup *> m_mapDeviceGroups;
	class LocationInfo *m_pLocationInfo_Initial; // The initial location

	/** flags */
	bool m_bCaptureKeyboard_OnOff; /** < flag for capture keyboard */
	bool m_bCaptureKeyboard_Reset; /** < flag for capture keyboard */
	int m_iCaptureKeyboard_EditType; /** < 0 = normal, 1 = numbers only, 2 = pin */
	bool m_bCaptureKeyboard_DataGrid; /** < flag for capture keyboard */
	bool m_bWeCanRepeat; /** < true if the rendering device we're using gives us Region Up Messages */
	bool m_bRestrictedOp_IsUser;
	int m_iRestrictedOp_ID;
    bool m_bShowShortcuts;
	bool m_bIsOSD; // true if this is an on-screen display

	int  m_iCaptureKeyboard_PK_Variable; /** < capture keyboard variable (coresponds to primary key) @todo ask */
	string m_sCaptureKeyboard_Text; /** < text for capture keyboard @todo ask */
	string m_sCaptureKeyboard_InternalBuffer; /** < capture keyboard internal buffer */

	// The remotes for the current media
	int m_iPK_DesignObj_Remote,m_iPK_DesignObj_Remote_Popup,m_iPK_DesignObj_FileList,m_iPK_DesignObj_FileList_Popup,m_iPK_DesignObj_RemoteOSD;
	string m_sNowPlaying; /** < set by the media engine, this is whatever media is currently playing */
	int m_dwPK_Device_NowPlaying;  /** < set by the media engine, this is whatever media device is currently playing */
	PlutoPopup *m_pActivePopup;

	int m_iTimeoutScreenSaver,m_iTimeoutBlank;  /** < When we're not on the screen saver screen how long to timeout before going to it, and when we are, how long before blacking the screen */
	time_t m_tTimeoutTime;  /** < On the screen saver screen, this is the time when the display will go blank */
    timespec m_tButtonDown;  
    string m_sCacheFolder; /** < Where to store graphic files for caching */
	int m_iCacheSize; /** < The maximum size of the graphics files stored (MB) */
    int m_iVideoFrameInterval; /** < The interval between two frame requested from the router */

    bool m_bShiftDown;
    bool m_bControlDown;
    bool m_bAltDown;
    bool m_bCapsLock;

	DesignObjText *m_pCaptureKeyboard_Text; /** < @todo ask */
	map<int,  vector<PlutoGraphic*> *> m_mapUserIcons; /** < user icons */
	map<int,FloorplanObjectVectorMap *> m_mapFloorplanObjectVector;
	FloorplanObjectVectorMap *m_mapFloorplanObjectVector_Find(int Page)	{ map<int,FloorplanObjectVectorMap *>::iterator it = m_mapFloorplanObjectVector.find(Page); return it==m_mapFloorplanObjectVector.end() ? NULL : (*it).second; }

	time_t m_LastActivityTime; /** < The last activity time */
	bool m_bDisplayOn; /** < False if the screen has blanked for the screen saver */
	bool m_bYieldScreen; /** < True if the orbiter should make the application desktop full screen ( hide itself ) */
	bool m_bYieldInput; /** < True if the orbiter should yield all input, like keyboard and mouse. This is useful when running the Orbiter as Linux desktop */
	bool m_bBypassScreenSaver; /** < True if we don't want the screen to blank */
	bool m_bRepeatingObject; /** < True if we're currently holding down a repeating button */
	bool m_bRerenderScreen; /** <  Set to true means ignore the objects to redraw, and just redraw the whole screen */

	/**
	 * This is an internal counter. When we start doing stuff that may include lots of embedded commands which want to update the screen, we can create
	 * a NeedToRender object on the stack. Each one will increment this counter, and decrement in the destructor. When it's 0, it will call RedrawObjects automatically
	 * @todo ask if I placed this comment where it should be
	 */

	int m_dwIDataGridRequestCounter;  // For testing purposes we can log all datagrid requests

	DesignObj_OrbiterMap m_mapObj_All; /** < All objects with the object ID in x.y.z.a.b format */
	map < string, DesignObj_DataList * > m_mapObj_AllNoSuffix; /** < All object with the object ID as a string */
	list < ScreenHistory * > m_listScreenHistory; /** < A history of the screens we've visited */
	map<int,class DeviceData_Base *> m_mapDevice_Selected;  /** < We can select multiple devices on the floorplan to send messages to, instead of the usual one */

	string m_sObj_Popop_RemoteControl,m_sObj_Popop_FileList;
	int m_Popop_RemoteControl_X,m_Popop_RemoteControl_Y,m_Popop_FileList_X,m_Popop_FileList_Y;

	map < string, DesignObj_DataList * > m_mapObj_Bound; /** < All objects bound with the Bind Icon command */
	DesignObj_DataList *m_mapObj_Bound_Find(string PK_DesignObj) { map<string,DesignObj_DataList *>::iterator it = m_mapObj_Bound.find(PK_DesignObj); return it==m_mapObj_Bound.end() ? NULL : (*it).second; }

	/**
	 * @brief stores objects that need to be redrawned
	 * When it's time to redraw some objects without redrawing the whole screen, store them here
	 * then call RedrawObjects(), rather than updating the screen over and over if several objects change at once
	 */
	vector < class DesignObj_Orbiter * > m_vectObjs_NeedRedraw;
	vector < class DesignObjText * > m_vectTexts_NeedRedraw;

	vector < class DesignObj_Orbiter * > m_vectObjs_TabStops; /** < All the tab stops */
	vector < class DesignObj_Orbiter * > m_vectObjs_Selected; /** < All the objects currently selected */
	vector < class DesignObj_DataGrid * > m_vectObjs_GridsOnScreen; /** < All the grids currently on the screen */
	vector < class DesignObj_Orbiter * > m_vectObjs_VideoOnScreen; /** < All the video on screen */
	bool m_bAlreadyQueuedVideo; // We only put 1 GetVideFrame in the queue

    list<class PlutoPopup*> m_listPopups;

	map< string, class DesignObj_DataGrid * > m_mapObjs_AllGrids; /** < All the datagrids */

	/** for threads and shared memory control @todo ask */

	pthread_mutexattr_t m_MutexAttr; /** < @todo ask */
	pthread_cond_t m_MessageQueueCond; /** < @todo ask */
	pluto_pthread_mutex_t m_ScreenMutex; /** < Anything that should not be done during a screen render, change, etc. Blocking this will prevent screen changes */
	pluto_pthread_mutex_t m_VariableMutex; /** < Short mutex to protect members like strings and maps */
	pluto_pthread_mutex_t m_DatagridMutex; /** < Don't allow 2 threads to operate on datagrids at the same time */
	pthread_t m_MaintThreadID;

	CacheImageManager *m_pCacheImageManager;

	// Be sure we're only reading the correct version
	virtual bool OkayToDeserialize(int iSC_Version);

	/** these methods are general purpose, and will call Orbiter-specific methods to do the work */

    /**
    * @brief Show a popup
    */
	void AddPopup(list<class PlutoPopup*> &listPopups,class PlutoPopup *pPopup);

    /**
    * @brief Hide a popup
    */
    virtual void HidePopups(DesignObj_Orbiter *pObj);

    /**
    * @brief Render a popup
    */
    virtual void RenderPopup(PlutoPopup *pPopup, PlutoPoint point);

    /**
    * @brief Get the popup with the id or name specified
    */
    PlutoPopup *FindPopupByName(DesignObj_Orbiter *pObj,string sPopupName);

	/**
	 * @brief Render the screen in m_pScreenHistory_Current
	 */
	virtual void RenderScreen();
	virtual void BeginPaint() {};
	virtual void EndPaint() {};
	virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point) {};

	/**
	 * @brief These will redraw any objects in m_vectObjsToRedraw. Use this to queue objects to redraw, such as those tht
	 */
	virtual void RedrawObjects();

	/**
	 * @brief temp hack -- see comments
	 * @todo ask - add comments
	 */
	void RealRedraw( void *iData );

	// Don't show shortcuts anymore
	void RemoveShortcuts( void *iData );

	void RedrawObject( void *iData );

	// Reselect the given object, used for repeating buttons
	void ReselectObject( void *data );

public: // temp - remove this
	/**
	 * @brief Timeout the object, which is data
	 */
	void Timeout( void *data );

	/**
	 * @brief Handle the screen saver
	 */
	void ScreenSaver( void *data );

    /**
    * @brief Adjust the window size
    */
    virtual void AdjustWindowSize(int iWidth, int iHeight) {}

	/**
	 * @brief renders an object on the screen
	 * @todo ask
	 */
	virtual void RenderObject( DesignObj_Orbiter *pDesignObj_Orbiter, DesignObj_Orbiter *pDesignObj_Orbiter_Screen, PlutoPoint point = PlutoPoint(0, 0) );

	/**
	 * @brief renders a floorplan
	 * @todo ask
	 */
	virtual void RenderFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter, DesignObj_Orbiter *pDesignObj_Orbiter_Screen, PlutoPoint point = PlutoPoint(0, 0));

	/**
	 * @brief prepares the render datagrid for rendering
	 * @todo ask
	 */
	virtual void PrepareRenderDataGrid( DesignObj_DataGrid *pObj, string& sDelSelections );

	/**
	 * @brief renders the data grid
	 * @todo ask better comment
	 */
	virtual void RenderDataGrid( DesignObj_DataGrid *pObj, PlutoPoint point = PlutoPoint(0, 0) );

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
	virtual void ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj );
	void HandleNewObjectsOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter);

	/**
	 * @brief takes out the specified object from screen
	 * @param pObj the object to remove from screen
	 * @todo ask
	 */
	virtual void ObjectOffScreen( DesignObj_Orbiter *pObj );

	/**
	 * @brief returnes the video frame from the DesignObj_Orbiter object pointed by iData
	 * @param iData points to the DesignObj_Orbiter object we want the video frame from
	 */
	virtual void GetVideoFrame( void *iData );

	/**
	 * @brief
	 * @param if X and Y are -1, the object was selected by pushing a keyboard key, rather than touching the screen
	 */
	void SelectedObject( DesignObj_Orbiter *pDesignObj_Orbiter, int iX = -1, int iY = -1);

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

	virtual void ShowProgress() {}

	/**
	 * @brief
	 * @todo An object that requires special handling was selected
	 */
	virtual void SpecialHandlingObjectSelected(DesignObj_Orbiter *pDesignObj_Orbiter);

	/**
	 * @brief
	 * @todo A floorplan object was selected
	 */
	virtual void SelectedFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter);

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
    * @brief Render selected state for this object
    */
    virtual void SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point = PlutoPoint(0, 0) );

	/**
	 * @brief Highlight this object
	 */
	virtual void HighlightObject( class DesignObj_Orbiter *pObj,  PlutoPoint point = PlutoPoint(0, 0));

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
	 * @brief Returns false if a screen was specified and it's not the current one
	 */
	bool TestCurrentScreen(string &sPK_DesignObj_CurrentScreen);

	/**
	 * @brief A "DoMaint" callback when we need to select an object after a delay
	 */
	void DelayedSelectObject( void *data );

	/**
	 * @brief A "DoMaint" callback to continually refresh the screen
	 */
	void ContinuousRefresh( void *data );

	/**
	 * @brief Kill the maintenance thread that handles tasks like redraws.  This function will block until the thread exits
	 */
	void KillMaintThread();

    void StopSimulatorThread();

	/**
	 * @brief Convert a virtual device (a negative device that corresponds to the VirtDev entries in DeviceTemplate) into the real device ID
	 */
	int TranslateVirtualDevice(int PK_DeviceTemplate);

/*	virtual void RenderScreen(  );	// Render the screen in m_pScreenHistory_Current
	virtual void RedrawObjects(  );   // These will redraw any objects in m_vectObjsToRedraw.  Use this to queue objects to redraw,  such as those tht
void RealRedraw( void *data );  // temp hack -- see comments
	virtual void RenderObject( DesignObj_Orbiter *pDesignObj_Orbiter,  DesignObj_Orbiter *pDesignObj_Orbiter_Screen );
	virtual void PrepareRenderDataGrid( DesignObj_DataGrid *pObj,  string& DelSelections );
	virtual void RenderDataGrid( DesignObj_DataGrid *pObj );
	// The framework will call this when it's time to change screens.  This immediately calls RenderScreen.  Normally we add the current
	// screen to the history list so we can go back again.  However,  if we're changing screens because of a GO BACK command,  we won't do that
	virtual void NeedToChangeScreens( ScreenHistory *pScreenHistory, bool bAddToHistory=true );
	virtual void ObjectOnScreenWrapper(  );
	virtual void ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, bool bDontResetState );
	virtual void ObjectOffScreen( DesignObj_Orbiter *pObj );
	virtual void CheckSpecialOnScreenConditions(  ) {};
	void GetVideoFrame( void *data );

	void SelectedObject( DesignObj_Orbiter *pDesignObj_Orbiter,  int X=0,  int Y=0 );
	bool SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid,  int X,  int Y );
	bool SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid,  class DataGridCell *pCell );

	virtual void SpecialHandlingObjectSelected( DesignObj_Orbiter *pDesignObj_Orbiter ) {};

	// The above methods are called by the orbiter-specific class to indicate input,  they will call these following methods:
	bool ClickedButton( DesignObj_Orbiter *pDesignObj_Orbiter, int PK_Button );
	bool ClickedRegion( DesignObj_Orbiter *pDesignObj_Orbiter, int X, int Y, DesignObj_Orbiter *&pTopMostAnimatedObject );

	// Highlighting is used to show the currently 'highlighted' object,  particularly with a remote that has up/down/left/right/enter controls
	// These functions move between objects that have their 'tab stop' property set to true
	virtual void HighlightObject( class DesignObj_Orbiter *pObj );  // Highlight this object
	virtual void HighlightFirstObject(  );  // Find the first 'tab stop' object on screen and highlight it
	virtual bool HighlightFirstChildObject( DesignObj_Orbiter* pObj );  // A recursive loop for HighlightFirstObject
	void FindObjectToHighlight( DesignObj_Orbiter **ppNextObjectToRight,  DesignObj_Orbiter *pObj,  int PK_Direction );  // This is used by the following function.
	virtual void HighlightNextObject( int PK_Direction ); // Given a direction ( UDLR ) find the object.
*/

	/**
	 *	INITIALIZATION
	 */

	/**
	 * @brief initializises the orbiter with the specified graphic type
	 * @todo ask
	 */
	virtual void Initialize( GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0 );

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
	virtual bool RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point = PlutoPoint(0, 0) );

	/**
	 * @brief renders a cell in the specified cell of the specified table at the specified coordinates
	 * @todo ask
	 */
	virtual bool RenderCell( class DesignObj_DataGrid *pObj, class DataGridTable *pT, class DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0) );

	/**
	 * @brief renders a graphic object in the specified rectangle
	 * @todo ask
	 */
	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio = false, PlutoPoint point = PlutoPoint(0, 0)) = 0;

	virtual void RenderGraphic( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, bool bDisableAspectRatio = false, PlutoPoint point = PlutoPoint(0, 0) );

	virtual PlutoGraphic *CreateGraphic() = 0;

	/**
	 * @brief The derived class should implement this if it can.  It moves the mouse pointer to the given coordinates
	 */
	virtual void SetMousePointer(int X, int Y) {}

	virtual void PlayMNG_CallBack(void *data);

    virtual void GetRepeatedKeysForScreen(DesignObj_Orbiter* pObj, string& sKeysList);

    virtual bool IsRepeatedKeyForScreen(DesignObj_Orbiter* pObj, int iPK_Button);

	/**
	 * @brief renders text with the specified style
	 */
	virtual void RenderText( class DesignObjText *Text, class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0) ) = 0;

	/**
	 * @brief draws an rectangle
	 */
	virtual void SolidRectangle( int iX, int iY, int iWidth, int iHeight, PlutoColor color, int iOpacity = 100 ) = 0;

	/**
	 * @brief draws an x-or'd rectangle outline.  Used to highlight something on screen
	 */
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)=0;

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
	 * @brief sets the time to the one specified by the parameter
	 */
	virtual void SetTime( char *pcServerTimeString ) = 0;

	/**
	 * @brief We're going to be redrawing something on top of this object. Save it's state, so that during the next redraw this will be used
	 */
	virtual void SaveBackgroundForDeselect( DesignObj_Orbiter *pObj, PlutoPoint point ) = 0;

/*
	virtual void Initialize( GraphicType Type );
	void InitializeGrid( DesignObj_DataGrid *pObj );
	bool ParseConfigurationData( GraphicType Type );
	void ParseObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen, DesignObj_Orbiter *pObj_Parent, GraphicType Type,  int Lev );


	virtual bool RenderDesktop( class DesignObj_Orbiter *pObj,  PlutoRectangle rectTotal ); // Not all controllers will be able to do this.  Mainly the 'x' nested
	virtual bool RenderCell( class DesignObj_DataGrid *pObj,  class DataGridTable *pT,  class DataGridCell *pCell,  int j,  int i,  int GraphicToDisplay );
	virtual void RenderGraphic( class DesignObj_Orbiter *pObj,  PlutoRectangle rectTotal,  bool bDisableAspectRatio=false )=0;
	virtual void RenderText( class DesignObjText *Text, class TextStyle *pTextStyle )=0;
	virtual void SolidRectangle( int x,  int y,  int width,  int height,  PlutoColor color,  int Opacity = 100 ) = 0;
	virtual void DrawLine( int x,  int y,  int width,  int height,  PlutoColor color,  int Opacity = 100 ) = 0;
	virtual void ReplaceColorInRectangle( int x,  int y,  int width,  int height,  PlutoColor ColorToReplace,  PlutoColor ReplacementColor ) = 0;
	virtual void FloodFill( int x,  int y,  PlutoColor ColorToReplace,  PlutoColor ReplacementColor ) = 0;
	virtual void SolidRectangle( int x,  int y,  int width,  int height,  PlutoColor color,  int Opacity=100 ) = 0;
	virtual void SetTime( char *ServerTimeString ) = 0;
	virtual void SaveBackgroundForDeselect( DesignObj_Orbiter *pObj )=0;  // We're going to be redrawing something on top of this object.  Save it's state,  so that during the next redraw this will be used
*/

	/**
	 *	FUNCTIONS A DERIVED ORBITER MAY WANT TO IMPLEMENT
	 */

	/**
	 * @brief A graphic is no longer on screen. Maybe remove it from cache
	 */
	virtual void GraphicOffScreen(vector<class PlutoGraphic*> *pvectGraphic);

public:

	class Event
	{
	public:
		typedef enum _EventType {
			QUIT,
			NOT_PROCESSED,
			BUTTON_DOWN,
			BUTTON_UP,
			REGION_DOWN,
			REGION_UP
		} EventType;

		EventType type;

		union {
			struct {
				int m_iPK_Button;
			} button;

			struct {
				int m_iButton;
				int m_iX;
				int m_iY;
			} region;
		} data;
	};

	virtual bool PreprocessEvent( Orbiter::Event &event );
	virtual bool ProcessEvent( Orbiter::Event &event );

	void QueueEventForProcessing( void *eventData );

    virtual bool HandleButtonEvent(int PK_Button);

protected:
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
	virtual bool ButtonUp(int iPK_C_Button);

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
	virtual bool RegionUp(int iX, int iY);

    virtual void StopRepeatRelatedEvents();

public:
	/**
	 * @brief Something happened, like a touch or a button, reset any timeouts or screen saver.  Returns false if it should be ignored
	 */
	virtual bool GotActivity(  );

	virtual void SimulateMouseClick(int x, int y);

	virtual void SimulateKeyPress(long key);

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
	 * @brief Reset the selected state of an object because it's coming on screen
	 */
	void ResetState(DesignObj_Orbiter *pObj, bool bDontResetState=false);

	/**
	 * @brief Redraw any objects that change during execution of a command
	 */
	virtual bool ReceivedMessage( class Message *pMessageOriginal );

	/**
	 * @brief sets the capture keyboard parameters
	 */
	bool BuildCaptureKeyboardParams( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid );

	/**
	 * @brief Get all the buttons that this object and its children will want to trap for
	 */
	void GetButtonsInObject( DesignObj_Data *pObj, vector<int> &vectButtons );

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
	 * @brief Find an object.  If pDesignObj_Orbiter is past and the ObjectID is not fully qualified, it will look for sPK_DesignObj as a child of pDesignObj_Orbiter
	 */
	DesignObj_Orbiter *FindObject( string sPK_DesignObj, class DesignObj_Orbiter *pDesignObj_Orbiter = NULL );

	/**
	 * @brief finds an object identified by a single numeber by searching in the object hierarchi
	 * @todo ask
	 */
	DesignObj_Orbiter *FindSingleNumberObject( int iPK_ID_Object, DesignObj_Orbiter *pObj );

	/**
	 * @brief executes the commands in the command list.  If there is a goto screen, we don't want to execute it inline--it should be executed last
	 * after setting all the variables and such.  So, rather than executing a goto screen, if one is found, it will be set in pMessage_GotoScreen
	 * and the caller is expected to pass it to ReceiveMessage after execute commands in list has finished.  We don't just put it at the end of
	 * execute commands, because we could be calling ExecuteCommands more than 1 at a time, such as if multiple objects were selected.

	 AB 5/29/2005 -- This means that if a button does a goto screen, then a show object for a button by default hidden,
	 it won't be unhidden because the goto screen (which resets the hidden flag) is executed last.  This concept of 
	 holding up the goto is probably no longer needed since we now use NeedToRender, which serves the same purpose
	 */
	void ExecuteCommandsInList( DesignObjCommandList *pDesignObjCommandList, DesignObj_Orbiter *pDesignObj_Orbiter, int iX = -1, int iY = -1, int Repeat=0 ); // Execute commands

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
	void GotoScreen( string sScreen, string sID = "" ) { CMD_Goto_Screen( 0, sScreen, sID, "", false, false ); }

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
	 * @brief gets the grid from the DesignObj_DataGrid object into the pDataGridTable parmeter.  This may change the
	 * value of GridCurRow if the pObj->m_sSeek is set to seek to a given position
	 */
	bool AcquireGrid( DesignObj_DataGrid *pObj, int GridCurCol, int &GridCurRow, class DataGridTable* &pDataGridTable );

	/**
	 * @brief returns the selected identifiing it by the row number
	 * @todo ask
	 */
	bool SelectedGrid( int iDGRow );

	/**
	 * @brief We need the maintenance function to be called in this many clock ticks
	 */
	void CallMaintenanceInMiliseconds( time_t miliseconds, OrbiterCallBack fnCallBack, void *iData, ePurgeExisting e_PurgeExisting, bool bPurgeTaskWhenScreenIsChanged = true );

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

	virtual void OnReload() { Orbiter_Command::OnReload(); OnQuit(); g_pPlutoLogger->Write(LV_WARNING,"Orbiter reloading"); }  // The OnQuit handlers take care of terminating the orbiter's event procesor, such as by firing an SDL event

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Path();
	int DATA_Get_PK_Users();
	string DATA_Get_Current_Screen();
	void DATA_Set_Current_Screen(string Value);
	int DATA_Get_PK_Distro();
	bool DATA_Get_Development();
	bool DATA_Get_No_Effects();
	int DATA_Get_PK_Skin();
	int DATA_Get_PK_Size();
	int DATA_Get_PK_Language();
	string DATA_Get_FK_EntertainArea();
	string DATA_Get_Update_Name();
	string DATA_Get_Communication_file();
	string DATA_Get_Timeout();
	string DATA_Get_CacheFolder();
	int DATA_Get_CacheSize();
	bool DATA_Get_Use_OCG_Format();
	int DATA_Get_VideoFrameInterval();
	int DATA_Get_ImageQuality();
	bool DATA_Get_Leave_Monitor_on_for_OSD();
	string DATA_Get_Ignore_State();
	bool DATA_Get_Dont_Auto_Jump_to_Remote();

			*****EVENT***** accessors inherited from base class
	void EVENT_Touch_or_click(int iX_Position,int iY_Position);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #1 - Capture Keyboard To Variable */
	/** As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object. */
		/** @param #3 PK_DesignObj */
			/** The Design Object which contains the text Object */
		/** @param #4 PK_Variable */
			/** The variable in which to store the input */
		/** @param #8 On/Off */
			/** If 0, this stops capturing */
		/** @param #14 Type */
			/** 1=normal, 2=pin, 3=phone number */
		/** @param #24 Reset */
			/** if true, the next keypress will clear the variable and start new */
		/** @param #25 PK_Text */
			/** The text object in which to store the current input */
		/** @param #55 DataGrid */
			/** If 1, we'll scroll the data grid too when typing keys. */

	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { string sCMD_Result; CMD_Capture_Keyboard_To_Variable(sPK_DesignObj.c_str(),iPK_Variable,sOnOff.c_str(),sType.c_str(),sReset.c_str(),iPK_Text,bDataGrid,sCMD_Result,NULL);};
	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #2 - Orbiter Beep */
	/** Make the orbiter beep */

	virtual void CMD_Orbiter_Beep() { string sCMD_Result; CMD_Orbiter_Beep(sCMD_Result,NULL);};
	virtual void CMD_Orbiter_Beep(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #3 - Display On/Off */
	/** Turn the display on or off */
		/** @param #8 On/Off */
			/** 0=Off, 1=On */
		/** @param #125 Already processed */
			/** Normally Orbiter will forward the on/off through DCE so the other devices can turn on/off.  If this is true, it won't. */

	virtual void CMD_Display_OnOff(string sOnOff,bool bAlready_processed) { string sCMD_Result; CMD_Display_OnOff(sOnOff.c_str(),bAlready_processed,sCMD_Result,NULL);};
	virtual void CMD_Display_OnOff(string sOnOff,bool bAlready_processed,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #4 - Go back */
	/** Make the orbiter go back to the prior screen, like the back button in a web browser */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If this is specified, the orbiter will ignore the command unless this is the current screen */
		/** @param #21 Force */
			/** Screens can be flagged, "Cant go back", meaning the go back will skip over them.  If Force=1, the Orbiter returns to the prior screen regardless */

	virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce) { string sCMD_Result; CMD_Go_back(sPK_DesignObj_CurrentScreen.c_str(),sForce.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #5 - Goto Screen */
	/** Goto a specific screen */
		/** @param #2 PK_Device */
			/** For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen.  When the screen changes, it will be reset */
		/** @param #3 PK_DesignObj */
			/** The screen to go to.  Can be be fully qualified (x.y.z), or just contain the screen # */
		/** @param #10 ID */
			/** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If this is specified, the orbiter will ignore the command unless this is the current screen.  If this is -1, that will match a main menu or screen saver (ie the Orbiter is not in use). */
		/** @param #22 Store Variables */
			/** If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen */
		/** @param #114 Cant Go Back */
			/** If true, then when this screen goes away the user won't be able to return to it -- it will be skipped over, unless Go Back with Force=1 is used.  This prevents layers of popup screens. */

	virtual void CMD_Goto_Screen(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back) { string sCMD_Result; CMD_Goto_Screen(iPK_Device,sPK_DesignObj.c_str(),sID.c_str(),sPK_DesignObj_CurrentScreen.c_str(),bStore_Variables,bCant_Go_Back,sCMD_Result,NULL);};
	virtual void CMD_Goto_Screen(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #6 - Show Object */
	/** Change an objects visible state. */
		/** @param #3 PK_DesignObj */
			/** The object to show or hide */
		/** @param #4 PK_Variable */
			/** The variable to use in the comparisson.  See Comparisson Value. */
		/** @param #6 Comparisson Operator */
			/** A type of comparisson: =  <  <>  !=  > */
		/** @param #7 Comparisson Value */
			/** If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true */
		/** @param #8 On/Off */
			/** 1=show object, 2=hide object */

	virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { string sCMD_Result; CMD_Show_Object(sPK_DesignObj.c_str(),iPK_Variable,sComparisson_Operator.c_str(),sComparisson_Value.c_str(),sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #7 - Terminate Orbiter */
	/** Causes the Orbiter application to exit */

	virtual void CMD_Terminate_Orbiter() { string sCMD_Result; CMD_Terminate_Orbiter(sCMD_Result,NULL);};
	virtual void CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #8 - Remove Screen From History */
	/** The orbiter keeps a history of visible screens, allowing the user to go back.  See Go_Back.  This removes screens from the queue that should not available anymore.  An example is when a call comes in, the controllers are sent to an incoming call screen. */
		/** @param #3 PK_DesignObj */
			/** The screen to remove */
		/** @param #10 ID */
			/** If specified, only screens that match this ID will be removed */

	virtual void CMD_Remove_Screen_From_History(string sPK_DesignObj,string sID) { string sCMD_Result; CMD_Remove_Screen_From_History(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Remove_Screen_From_History(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #9 - Scroll Grid */
	/** Scroll a datagrid */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to scroll the grid */

	virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Scroll_Grid(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
	virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #10 - Move Highlight */
	/** Move the current highlight pointer */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to move the highlight */

	virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Move_Highlight(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
	virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #13 - Play Sound */
	/** Plays a sound file on the orbiter */
		/** @param #19 Data */
			/** A pointer to a block of memory representing the sound file to play */
		/** @param #20 Format */
			/** Indicates what type of data is in the memory block.  1=wav, 2=mp3 */

	virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat) { string sCMD_Result; CMD_Play_Sound(pData,iData_Size,sFormat.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #14 - Refresh */
	/** Invalidates and redraws the current screen, optionally re-requesting data from a datagrid.  The OnLoad commands are not fired.  See Regen Screen. */
		/** @param #15 DataGrid ID */
			/** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

	virtual void CMD_Refresh(string sDataGrid_ID) { string sCMD_Result; CMD_Refresh(sDataGrid_ID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #15 - Regen Screen */
	/** The screen is reloaded like the user was going to it for the first time.  The OnUnload and OnLoad commands are fired. */

	virtual void CMD_Regen_Screen() { string sCMD_Result; CMD_Regen_Screen(sCMD_Result,NULL);};
	virtual void CMD_Regen_Screen(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #16 - Requires Special Handling */
	/** When a button needs to do something too sophisticated for a normal command, attach this command.  When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code. */

	virtual void CMD_Requires_Special_Handling() { string sCMD_Result; CMD_Requires_Special_Handling(sCMD_Result,NULL);};
	virtual void CMD_Requires_Special_Handling(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #17 - Seek Data Grid */
	/** Causes a datagrid to seek to a particular position. */
		/** @param #9 Text */
			/** If specified, the orbiter will jump to the first row which has a cell that starts with this text.  Specify Position X to use a column other than the first one. */
		/** @param #11 Position X */
			/** The column to jump to.  If Text is not blank, the column to search. */
		/** @param #12 Position Y */
			/** The row to jump to.  Ignored if Text is not blank */
		/** @param #15 DataGrid ID */
			/** The datagrid to scroll.  If not specified, the first visible one will be used */

	virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { string sCMD_Result; CMD_Seek_Data_Grid(sText.c_str(),iPosition_X,iPosition_Y,sDataGrid_ID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #18 - Set Graphic To Display */
	/** All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes".  These are like "views".  A Selected mode may appear depressed, for example.  All children of this object will also be set. */
		/** @param #3 PK_DesignObj */
			/** The object to set */
		/** @param #10 ID */
			/** 0=standard mode, -1=selected -2=highlight a positive number is one of the alternates */

	virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID) { string sCMD_Result; CMD_Set_Graphic_To_Display(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #20 - Set Object Parameter */
	/** changes one of the object's DesignObjParameters */
		/** @param #3 PK_DesignObj */
			/** The object to change */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #27 PK_DesignObjParameter */
			/** The parameter */

	virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { string sCMD_Result; CMD_Set_Object_Parameter(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),iPK_DesignObjParameter,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #21 - Set Object Position */
	/** Change an objects's position on the screen */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Position(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #22 - Set Object Size */
	/** Change an object's size */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Size(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #23 - Set Pos Rel To Parent */
	/** Like Set Object Position, but the X and Y coordinates are assumed to be relative to the parent rather than absolute */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Pos_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #24 - Set Size Rel To Parent */
	/** Change an object's size, relative to it's parent object */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/** The percentage of the parent object's width.  100=the parent's full width. */
		/** @param #12 Position Y */
			/** The percentage of the parent object's height.  100=the parent's full height. */

	virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Size_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #25 - Set Text */
	/** Change the text within a text object on the fly */
		/** @param #3 PK_DesignObj */
			/** The Design Object which contains the text object.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #9 Text */
			/** The text to assign */
		/** @param #25 PK_Text */
			/** The text object in which to store the current input */

	virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text) { string sCMD_Result; CMD_Set_Text(sPK_DesignObj.c_str(),sText.c_str(),iPK_Text,sCMD_Result,NULL);};
	virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #26 - Set Bound Icon */
	/** Sets an icon that is bound to status.  "Bind Icon" is put in the object's on load commands, and then this command sets the status at runtime. */
		/** @param #5 Value To Assign */
			/** The value corresponding to an alt graphic. */
		/** @param #14 Type */
			/** The type of bound icon. */

	virtual void CMD_Set_Bound_Icon(string sValue_To_Assign,string sType) { string sCMD_Result; CMD_Set_Bound_Icon(sValue_To_Assign.c_str(),sType.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Bound_Icon(string sValue_To_Assign,string sType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #27 - Set Variable */
	/** Change the value of a variable */
		/** @param #4 PK_Variable */
			/** The variable to change */
		/** @param #5 Value To Assign */
			/** The value to assign */

	virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign) { string sCMD_Result; CMD_Set_Variable(iPK_Variable,sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Simulates that a key has been touched.  Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse.  Also works with the "Capture Keyboard to Variable" command. */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simulates a mouse click or touch on the indicated x & y coordinates */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #30 - Stop Sound */
	/** If a sound file is being played, it will be stopped. */

	virtual void CMD_Stop_Sound() { string sCMD_Result; CMD_Stop_Sound(sCMD_Result,NULL);};
	virtual void CMD_Stop_Sound(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #31 - Store Variables */
	/** The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value. */

	virtual void CMD_Store_Variables() { string sCMD_Result; CMD_Store_Variables(sCMD_Result,NULL);};
	virtual void CMD_Store_Variables(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #32 - Update Object Image */
	/** Changes the background image within an object */
		/** @param #3 PK_DesignObj */
			/** The object in which to put the bitmap */
		/** @param #14 Type */
			/** 1=bmp, 2=jpg, 3=png */
		/** @param #19 Data */
			/** The contents of the bitmap, like reading from the file into a memory buffer */
		/** @param #23 Disable Aspect Lock */
			/** If 1, the image will be stretched to fit the object */

	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { string sCMD_Result; CMD_Update_Object_Image(sPK_DesignObj.c_str(),sType.c_str(),pData,iData_Size,sDisable_Aspect_Lock.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #58 - Set Current User */
	/** Sets what user is currently using the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

	virtual void CMD_Set_Current_User(int iPK_Users) { string sCMD_Result; CMD_Set_Current_User(iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** If you don't know the location, you can also set just the entertainment area */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea) { string sCMD_Result; CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #66 - Select Object */
	/** The same as clicking on an object. */
		/** @param #3 PK_DesignObj */
			/** The object to select. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** Will only happen if this is the current screen. */
		/** @param #102 Time */
			/** If specified, rather than happening immediately it will happen in x seconds. */

	virtual void CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime) { string sCMD_Result; CMD_Select_Object(sPK_DesignObj.c_str(),sPK_DesignObj_CurrentScreen.c_str(),sTime.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #72 - Surrender to OS */
	/** Let the O/S take over.  This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone.  The orbiter will then let the other application ta */
		/** @param #8 On/Off */
			/** 1=Hide and let the OS take over.  0=The orbiter comes up again. */
		/** @param #54 Fully release keyboard */
			/** Only applies if on/off is 1.  If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch.  If true, it will pass all keys. */

	virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard) { string sCMD_Result; CMD_Surrender_to_OS(sOnOff.c_str(),bFully_release_keyboard,sCMD_Result,NULL);};
	virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #77 - Set Current Room */
	/** If you don't know the location, you can also set just the room */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Current_Room(int iPK_Room) { string sCMD_Result; CMD_Set_Current_Room(iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #85 - Reset Highlight */
	/** Resets the currently highlighted object.  Do this when you hide or unhide blocks that have tab stops. */

	virtual void CMD_Reset_Highlight() { string sCMD_Result; CMD_Reset_Highlight(sCMD_Result,NULL);};
	virtual void CMD_Reset_Highlight(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #88 - Set Current Location */
	/** Sets the location the orbiter is in.  The location is a combination of room and entertainment area. */
		/** @param #65 LocationID */
			/** The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area. */

	virtual void CMD_Set_Current_Location(int iLocationID) { string sCMD_Result; CMD_Set_Current_Location(iLocationID,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #192 - On */
	/** Turn the device on */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Turn the device off */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #238 - Continuous Refresh */
	/** Continuously refresh the current page.  Used when the page contains constantly changing data. */
		/** @param #102 Time */
			/** The interval time in seconds */

	virtual void CMD_Continuous_Refresh(string sTime) { string sCMD_Result; CMD_Continuous_Refresh(sTime.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Continuous_Refresh(string sTime,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #242 - Set Now Playing */
	/** Used by the media engine to set the "now playing" text on an orbiter.  If the orbiter is bound to the remote for an entertainment area it will get more updates than just media,  like cover art, but this is the basic information that is visible on screens */
		/** @param #2 PK_Device */
			/** The currently active media device */
		/** @param #3 PK_DesignObj */
			/** 4 comma delimited objects: normal remote, popup remote, file list remote, popup file list remote */
		/** @param #5 Value To Assign */
			/** The description of the media */
		/** @param #48 Value */
			/** The track number or position in the playlist */

	virtual void CMD_Set_Now_Playing(int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,int iValue) { string sCMD_Result; CMD_Set_Now_Playing(iPK_Device,sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),iValue,sCMD_Result,NULL);};
	virtual void CMD_Set_Now_Playing(int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #254 - Bind Icon */
	/** Used to make a button have an icon that reflects a current state, such as the user's status, the house mode, etc.  This is accomplished by creating an object with multiple alternate versions, and then executing a "Set  Bound Icon" to select the right one. */
		/** @param #3 PK_DesignObj */
			/** The object which contains the icon, or whose child objects contain the icon. */
		/** @param #14 Type */
			/** The type of binding, like "housemode", "userstatus_39288", etc. */
		/** @param #104 Child */
			/** If true, it will set the property for the child object(s), rather than the designated object. */

	virtual void CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild) { string sCMD_Result; CMD_Bind_Icon(sPK_DesignObj.c_str(),sType.c_str(),bChild,sCMD_Result,NULL);};
	virtual void CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #258 - Clear Selected Devices */
	/** Floorplans, in particular, rely on a vector of selected devices, allowing the user to select more than one.  This command clears that list, removing any selected devices.  It can optionally cause the Object passed in as a parameter to be refreshed. */
		/** @param #3 PK_DesignObj */
			/** If specified, the object referenced here will be invalidated and redrawn. */

	virtual void CMD_Clear_Selected_Devices(string sPK_DesignObj) { string sCMD_Result; CMD_Clear_Selected_Devices(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Clear_Selected_Devices(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #260 - Set Main Menu */
	/** Sets the Main Menu to 'Normal', 'Sleeping' or 'Screen Saver', optionally assigning new screens to those values. */
		/** @param #9 Text */
			/** Can be N, S, or V to set to the Normal, Sleeping, or Screen Saver, optionally followed by an = to assign a new screen to that menu.  e.g. N or N=1872.0.0 */

	virtual void CMD_Set_Main_Menu(string sText) { string sCMD_Result; CMD_Set_Main_Menu(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Main_Menu(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #265 - Quit */
	/** Exits the orbiter application */

	virtual void CMD_Quit() { string sCMD_Result; CMD_Quit(sCMD_Result,NULL);};
	virtual void CMD_Quit(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #324 - Set Timeout */
	/** Specifies when a given screen will timeout, executing the timeout actions.  This will also reset a pending timeout */
		/** @param #3 PK_DesignObj */
			/** The screen to set the timeout on.  If blank the current screen. */
		/** @param #102 Time */
			/** The timeout in seconds.  0 or blank means no timeout. */

	virtual void CMD_Set_Timeout(string sPK_DesignObj,string sTime) { string sCMD_Result; CMD_Set_Timeout(sPK_DesignObj.c_str(),sTime.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Timeout(string sPK_DesignObj,string sTime,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #325 - Keep Screen On */
	/** Allow or don't allow the screen to blank with the screen saver. */
		/** @param #8 On/Off */
			/** If other than "0", the screen saver will be disabled. */

	virtual void CMD_Keep_Screen_On(string sOnOff) { string sCMD_Result; CMD_Keep_Screen_On(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Keep_Screen_On(string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #330 - Set Mouse Pointer Over Object */
	/** Positions the on-screen mouse pointer centered over a certain object */
		/** @param #3 PK_DesignObj */
			/** The object to center the mouse over. */

	virtual void CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj) { string sCMD_Result; CMD_Set_Mouse_Pointer_Over_Object(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #354 - Show Mouse Pointer */
	/** Shows or hides the mouse pointer. */
		/** @param #8 On/Off */
			/** 1=show it, 0=hide it. */

	virtual void CMD_Show_Mouse_Pointer(string sOnOff) { string sCMD_Result; CMD_Show_Mouse_Pointer(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #366 - Activate Window */
	/** ActivateApplication - Used by Linux On Screeen Orbiters only */
		/** @param #50 Name */
			/** Name as known by ratpoison. */

	virtual void CMD_Activate_Window(string sName) { string sCMD_Result; CMD_Activate_Window(sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #389 - Send Message */
	/** Sends a message stored in a parameter as a text object. */
		/** @param #9 Text */
			/** The message in command line-style format */

	virtual void CMD_Send_Message(string sText) { string sCMD_Result; CMD_Send_Message(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Message(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #397 - Show Popup */
	/** Shows a screen as a popup, at position x, y */
		/** @param #3 PK_DesignObj */
			/** The ID of the screen */
		/** @param #11 Position X */
			/** X position */
		/** @param #12 Position Y */
			/** Y position */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If specified the popup will be local to this screen, otherwise it will be global.  Global and local popups are treated separately */
		/** @param #50 Name */
			/** The popup name */
		/** @param #126 Exclusive */
			/** Hide any other popups that are also visible, unless don't hide is set. */
		/** @param #127 Dont Auto Hide */
			/** If true, this popup will not be automatically hidden when the screen changes or another exclusive is shown.  It must be explicitly hidden. */

	virtual void CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide) { string sCMD_Result; CMD_Show_Popup(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sName.c_str(),bExclusive,bDont_Auto_Hide,sCMD_Result,NULL);};
	virtual void CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #398 - Remove Popup */
	/** Hides a popup. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** (optional).  The screen on which it's a local popup */
		/** @param #50 Name */
			/** The name of the popup.  If not specified all popups will be removed */

	virtual void CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName) { string sCMD_Result; CMD_Remove_Popup(sPK_DesignObj_CurrentScreen.c_str(),sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #399 - Show Shortcuts */
	/** Shows keyboard shortcuts for 10 seconds or until the screen changes. */

	virtual void CMD_Show_Shortcuts() { string sCMD_Result; CMD_Show_Shortcuts(sCMD_Result,NULL);};
	virtual void CMD_Show_Shortcuts(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #401 - Show File List */
	/**  */
		/** @param #3 PK_DesignObj */
			/** The screen with the file listing */
		/** @param #13 Filename */
			/** The directory to start with */
		/** @param #29 PK_MediaType */
			/** The type of media the user wants to browse. */
		/** @param #128 PK_DesignObj_Popup */
			/** The popup to use */

	virtual void CMD_Show_File_List(string sPK_DesignObj,string sFilename,int iPK_MediaType,string sPK_DesignObj_Popup) { string sCMD_Result; CMD_Show_File_List(sPK_DesignObj.c_str(),sFilename.c_str(),iPK_MediaType,sPK_DesignObj_Popup.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_File_List(string sPK_DesignObj,string sFilename,int iPK_MediaType,string sPK_DesignObj_Popup,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #402 - Use Popup Remote Controls */
	/** If this command is executed the remote controls will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen on which to put the popup */

	virtual void CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { string sCMD_Result; CMD_Use_Popup_Remote_Controls(iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #403 - Use Popup File List */
	/** If this command is executed the file lists will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen to put the popup on */

	virtual void CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { string sCMD_Result; CMD_Use_Popup_File_List(iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->

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

};

extern int g_iDontRender; /** < @todo ask */
extern clock_t g_cLastTime; /** < time of he last input from the user @todo ask */

class NeedToRender
{

	class Orbiter *m_pOrbiter; /** <  */
	const char *m_pWhere; /** <  */
	static ScreenHistory *m_pScreenHistory;
	static bool m_bAddToHistory;
public:

	/**
	 * @brief constructor
	 */
	NeedToRender( class Orbiter *pOrbiter, const char *pWhere );

	/**
	 * @brief destructor
	 */
	~NeedToRender(  )
	{
		g_iDontRender--;
		if ( g_iDontRender == 0 )
		{
			if( m_pScreenHistory )
			{
				ScreenHistory *pScreenHistory = m_pScreenHistory;
				m_pScreenHistory=NULL;
				m_pOrbiter->NeedToChangeScreens( pScreenHistory, m_bAddToHistory );
			}
			g_pPlutoLogger->Write( LV_STATUS, "NeedToRender::~NeedToRender() calling redraw for: %s", m_pWhere);
			m_pOrbiter->RedrawObjects();
		}

	}

	static void NeedToChangeScreens( class Orbiter *pOrbiter, ScreenHistory *pScreenHistory, bool bAddToHistory = true );
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
