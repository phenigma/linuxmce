#ifndef __ORBITER_POCKETFROG_H__
#define __ORBITER_POCKETFROG_H__
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter.h"
#include "../DesignObj_Orbiter.h"
#include "../PocketFrog/PocketFrogGraphic.h"
//-----------------------------------------------------------------------------------------------------
#include <PocketFrog.h>
using namespace Frog;
//-----------------------------------------------------------------------------------------------------
namespace DCE
{

class Orbiter_PocketFrog : public Orbiter, public Game
{
protected:
	static Orbiter_PocketFrog* m_pInstance; //the one and only instance of OrbiterSDL_Win32

protected: // (mtoader) I want access to them in the OrbiterLinuxDesktop
	int m_nImageWidth, m_nImageHeight;
	int m_bFullScreen;

public:

	DisplayDevice* m_display;
	DisplayDevice* GetOrbiterDisplay() { return GetDisplay(); }

	Orbiter_PocketFrog(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);
	virtual ~Orbiter_PocketFrog();

	// Public virtual methods (from Game)
    virtual bool GameInit();
    virtual void GameEnd();
    virtual void GameLoop();
    virtual void StylusDown( Point stylus );
    virtual void StylusUp( Point stylus );
    virtual void StylusMove( Point stylus );

	// Drawing routines
	virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity = 100);
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color);
	virtual void DrawLine(int x, int y, int width, int height, PlutoColor color, int Opacity = 100) {};
	virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor);
	virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	virtual void RenderText(class DesignObjText *Text,class TextStyle *pTextStyle);
	virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj);
	virtual PlutoGraphic *CreateGraphic();

	// Rendering
	virtual void RenderScreen();
	virtual void RedrawObjects();

	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio);

	virtual void BeginPaint();
	virtual void EndPaint();
	virtual void UpdateRect(PlutoRectangle rect);

	virtual void OnQuit();

	// Other
	virtual void Initialize(GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0);
	virtual void SetTime(char *ServerTimeString) {};

	static void Cleanup();
	static void BuildOrbiter(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);
	static Orbiter_PocketFrog *GetInstance();

	void WriteStatusOutput(const char* pMessage);

	bool m_bConnectionLost;


protected:

	// Protected abstract methods

	// Protected virtual methods

	Surface *Screen;
	bool m_bNeedToUpdate;
};

}


#endif