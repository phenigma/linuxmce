#ifndef __ORBITER_RENDERER_H__
#define __ORBITER_RENDERER_H__

#include <string>
//-----------------------------------------------------------------------------------------------------
#include "DesignObj_Orbiter.h"
#include "PlutoGraphic.h"
#include "SerializeClass/ShapesColors.h"
#include "DCE/Logger.h"

namespace DCE
{
	class Orbiter;
	//class DesignObj_Orbiter;
	//class DesignObjText;
}

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
class OrbiterRenderer
{
private:

	friend class OrbiterRendererFactory;
	Orbiter *m_pOrbiter;

	/**
	* @brief stores objects that need to be redrawned
	* When it's time to redraw some objects without redrawing the whole screen, store them here
	* then call RedrawObjects(), rather than updating the screen over and over if several objects change at once
	*/
	vector<DesignObj_Orbiter *> m_vectObjs_NeedRedraw;
	vector<DesignObjText *> m_vectTexts_NeedRedraw;

protected:

	OrbiterRenderer(Orbiter *pOrbiter);

	void ConfigureDefaultTextStyle();
	virtual void UpdateScreen();

	auto_ptr<TextStyle> m_spTextStyle;
	pluto_pthread_mutex_t m_NeedRedrawVarMutex; //this will protect needredraw vectors

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

	/**
	* @brief Do the Highlighting of the currently highlighted object, or remove the highlighting
	*/
    virtual void DoHighlightObject();
	virtual void UnHighlightObject(bool bDeleteOnly = false);

	/**
	* @brief Find the first 'tab stop' object on screen and highlight it
	*/
	virtual void HighlightFirstObject();
	/**
	* @brief Find an object from a given direction (ie up = start at the top and move down, down=start at the bottom and move up)
	* finding any datagrid if there is one active if bPreferGrid is true, and that is only a child of pObj_Parent if that is not null,
	* and use pObj_RelativeTo as a starting point if it's specified
	*/
	DesignObj_Orbiter *FindFirstObjectByDirection(char cDirection /* u,d,l,r,1 (ul),2 (ur),3(dl),4(dr) */,bool bPreferGrid,DesignObj_Orbiter *pObj_Parent,DesignObj_Orbiter *pObj_RelativeTo);

	/**
	* @brief This is used by the following function
	*/
	DesignObj_Orbiter *FindObjectToHighlight( DesignObj_Orbiter *pObjCurrent, int PK_Direction );

	/**
	* @brief Given a direction ( UDLR ) find the object.  Return false if we couldn't find anything and left the highlighted object unchanged
	*/
	virtual bool HighlightNextObject( int dwPK_Direction );

	/**
	* @brief Render a shortcut for an object
	*/
	virtual void RenderShortcut(DesignObj_Orbiter *pObj);

	/**
	* @brief Hide a popup
	*/
	virtual void HidePopups(DesignObj_Orbiter *pObj);

	/**
	* @brief Render a popup
	*/
	virtual void RenderPopup(PlutoPopup *pPopup, PlutoPoint point);

	/**
	* @brief These will redraw any objects in m_vectObjsToRedraw. Use this to queue objects to redraw, such as those tht
	*/
	virtual void RedrawObjects();

	/*
	 *	Refreshes the screen as a task of maintenance thread
	 */
	virtual void RefreshScreen(void *data);

	/**
	* @brief renders an object on the screen async; it will add the object to m_vectObjs_NeedRedraw vector to be re-rendered
	*/
	virtual void RenderObjectAsync(DesignObj_Orbiter *pObj);

	/**
	* @brief renders a text object on the screen async; it will add the object to m_vectTexts_NeedRedraw vector to be re-rendered
	*/
	virtual void RenderTextAsync(DesignObjText *pObj);

	/*
	 *	Wrapper for ObjectOnScreen
	 */
	virtual void ObjectOnScreenWrapper();

	/**
	* @brief puts the object on screen
	* @param pObj pointer to the object to put on screen
	*/
	virtual void ObjectOnScreen( VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, PlutoPoint *ptPopup=NULL );

	/**
	* @brief A graphic is no longer on screen. Maybe remove it from cache
	*/
	virtual void GraphicOffScreen(vector<class PlutoGraphic*> *pvectGraphic);

	/**
	* @brief takes out the specified object from screen
	* @param pObj the object to remove from screen
	*/
	virtual void ObjectOffScreen( DesignObj_Orbiter *pObj );
};

#endif //__ORBITER_RENDERER_H__

