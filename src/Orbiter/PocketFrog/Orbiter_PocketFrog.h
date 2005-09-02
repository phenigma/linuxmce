#ifndef __ORBITER_POCKETFROG_H__
#define __ORBITER_POCKETFROG_H__
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter.h"
#include "../DesignObj_Orbiter.h"
#include "../PocketFrog/PocketFrogGraphic.h"
#include "PocketFrogWrapper.h"
//-----------------------------------------------------------------------------------------------------
#include <PocketFrog.h>
using namespace Frog;
//-----------------------------------------------------------------------------------------------------
namespace DCE
{

class TextToRenderInfo
{
public:
	string TextToDisplay;
	PlutoRectangle rPosition;
	int iPK_HorizAlignment,iPK_VertAlignment;
	PlutoPoint point;

	string sFont;
	PlutoColor ForeColor;
	int iPixelHeight;
	bool bBold, bItalic, bUnderline;
};

class Orbiter_PocketFrog : public Orbiter, public PlutoGame
{
protected:
	static Orbiter_PocketFrog* m_pInstance; //the one and only instance of OrbiterSDL_Win32
	bool m_bPoolRendering;
	vector<TextToRenderInfo *> m_vectPooledTextToRender;

	bool m_bUpdating;
	bool m_bFullScreen;

public:
	DisplayDevice* GetOrbiterDisplay() { return GetDisplay(); }

	Orbiter_PocketFrog(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);
	virtual ~Orbiter_PocketFrog();

	//from socket class
	virtual void PingFailed();

	// Public virtual methods (from Game)
	virtual void GameSuspend(); 
	virtual void GameResume(); 

	virtual bool IsFullScreen();
	virtual int GetWindowWidth();
	virtual int GetWindowHeight();

    virtual bool GameInit();
    virtual void GameEnd();
    virtual void GameLoop();
    virtual void StylusDown( Point stylus );
    virtual void StylusUp( Point stylus );
    virtual void StylusMove( Point stylus );
	virtual bool PocketFrogButtonDown(int button);
	virtual bool PocketFrogButtonUp(int button); 
	virtual void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void TryToUpdate();

	// Drawing routines
	virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity = 100);
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color);
	virtual void DrawLine(int x, int y, int width, int height, PlutoColor color, int Opacity = 100) {};
	virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor);
	virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0));
	virtual void RenderText(HDC hdc,string &sTextToDisplay,PlutoRectangle &rPosition,int iPK_HorizAlignment,int iPK_VertAlignment,
		string &sFont,PlutoColor &ForeColor,int iPixelHeight,bool bBold,bool bItalic,bool bUnderline,PlutoPoint point = PlutoPoint(0, 0));
	virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point);
	virtual PlutoGraphic *GetBackground( PlutoRectangle &rect ) ;
	virtual PlutoGraphic *CreateGraphic();

	// Rendering
	virtual void RenderScreen();
	virtual void RedrawObjects();

	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));

	virtual void BeginPaint();
	virtual void EndPaint();
	virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0));

	virtual void ShowProgress();
	virtual void OnQuit();
    virtual void DisplayMessage(string sMessage);

    virtual bool OnReplaceHandler(string sIP);
    virtual int  PromptUser(string sPrompt,map<int,string> *p_mapPrompts);

	// Other
	virtual void Initialize(GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0);
	virtual void SetTime(char *ServerTimeString) {};

	static void Cleanup();
	static void BuildOrbiter(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);
	static Orbiter_PocketFrog *GetInstance();

	static inline Pixel GetColor16(PlutoColor color);
	static inline BYTE GetRedColor(Pixel pixel);
	static inline BYTE GetGreenColor(Pixel pixel);
	static inline BYTE GetBlueColor(Pixel pixel);

	void WriteStatusOutput(const char* pMessage);

	bool m_bConnectionLost;
    virtual void OnUnexpectedDisconnect();

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);

    virtual void AdjustWindowSize(int iWidth, int iHeight);
	void CalcTextRectangle(RECT &rectLocation,PlutoRectangle &rPosition,int iRotation,int iHeight,int iVertAlignment);

	bool SelfUpdate();
};

}


#endif