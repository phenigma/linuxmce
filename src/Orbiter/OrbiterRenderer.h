#ifndef __ORBITER_RENDERER_H__
#define __ORBITER_RENDERER_H__

#include <string>
//-----------------------------------------------------------------------------------------------------
#include "DesignObj_Orbiter.h"
#include "PlutoGraphic.h"
#include "SerializeClass/ShapesColors.h"

namespace DCE
{
	class Orbiter;
}

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
class OrbiterRenderer
{
private:

	friend class OrbiterRendererFactory;
	Orbiter *m_pOrbiter;

protected:

	OrbiterRenderer(Orbiter *pOrbiter);

	void ConfigureDefaultTextStyle();
	auto_ptr<TextStyle> m_spTextStyle;

	virtual void UpdateScreen();

public:

	virtual ~OrbiterRenderer();

	Orbiter *OrbiterLogic();

	virtual void EventLoop() = 0; 
	virtual void Configure() = 0;

	/**
	* @brief renders text with the specified style
	*/
	virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text, class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0)) = 0;
	virtual void BatchedTextRendering(bool bValue) {}

	/**
	* @brief draws an rectangle
	*/
	virtual void SolidRectangle(int iX, int iY, int iWidth, int iHeight, PlutoColor color) = 0;

	/**
	* @brief draws an x-or'd rectangle outline.  Used to highlight something on screen
	*/
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color) = 0;

	/**
	* @brief draws an line
	*/
	virtual void DrawLine(int iX, int iY, int iWidth, int iHeight, PlutoColor color) = 0;

	/**
	* @brief replaces the specified color from within the specified rectangle with another one
	*/
	virtual void ReplaceColorInRectangle( int iX, int iY, int iWidth, int iHeight, PlutoColor ColorToReplace, PlutoColor ReplacementColor) = 0;

	/**
	* @brief floods the region that containes the specified point, replacing one color
	* A graphic is no longer on screen. Maybe remove it from cache.
	*/
	virtual void FloodFill(int iX, int iY, PlutoColor ColorToReplace, PlutoColor ReplacementColor) = 0;

	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio = false, PlutoPoint point = PlutoPoint(0, 0)) = 0;

	virtual PlutoGraphic *CreateGraphic() = 0;

	/**
	* @brief We're going to be redrawing something on top of this object. Save it's state, so that during the next redraw this will be used
	*/
	virtual void SaveBackgroundForDeselect( DesignObj_Orbiter *pObj, PlutoPoint point ) = 0;
	virtual PlutoGraphic *GetBackground( PlutoRectangle &rect ) = 0;

	virtual void ShowProgress(int nPercent);
	virtual bool DisplayProgress(string sMessage, int nProgress) { return false; }
	virtual bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
	virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts = NULL);
	virtual int PromptFor(string sToken);

	/**
	* @brief Render the screen
	*/
	virtual void RenderScreen(bool bRenderGraphicsOnly);

	virtual void BeginPaint() = 0;
	virtual void EndPaint() = 0;
	virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0)) = 0;

	virtual void OnQuit() = 0;
	virtual void OnReload();

	//helpers
	virtual void ClipRectangle(int& x, int& y, int& width, int& height);
	virtual void ClipRectangle(PlutoRectangle &rect);
	virtual void RenderFrame(void *) {}

	//highlighting and selecting logic

	/**
	* @brief Do the Highlighting of the currently highlighted object, or remove the highlighting
	*/
    virtual void DoHighlightObject();
	virtual void UnHighlightObject(bool bDeleteOnly = false);
};

#endif //__ORBITER_RENDERER_H__

