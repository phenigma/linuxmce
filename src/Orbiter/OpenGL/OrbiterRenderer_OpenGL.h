/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __ORBITERRENDERER_OPENGL_H__
#define __ORBITERRENDERER_OPENGL_H__
//-----------------------------------------------------------------------------------------------------
#include "../OrbiterRenderer.h"
#include "../DesignObj_Orbiter.h"

#include "PendingGLEffects.h"

#include "PopupCollection.h"

class OpenGL3DEngine;
//-----------------------------------------------------------------------------------------------------

namespace DCE
{
	class OrbiterRenderer_OpenGL : public OrbiterRenderer
	{
	protected:
		pthread_t GLThread;
		bool NeedToUpdateScreen_;
		std::auto_ptr<PendingGLEffects> m_spPendingGLEffects;
		bool m_bWindowCreated;
		PopupCollection* Popups;

		DesignObj_Orbiter *m_pLastHighlightedObject;

	public:

		OrbiterRenderer_OpenGL(Orbiter *pOrbiter);
		virtual ~OrbiterRenderer_OpenGL();

		virtual void Configure();
		virtual void EventLoop(); 
		virtual void Destroy();

		virtual bool NeedToUpdateScreen();
		virtual void ScreenUpdated();

		virtual void GetWindowPosition(PlutoPoint& point);
		virtual void UpdateScreen();

		virtual void RedrawObjects();

		// Drawing routines
		virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color, string ParentObjectID = "", string ObjectID = "");
		virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color, string ParentObjectID = "", string ObjectID = "");
		virtual void UnSelectObject(string ParentObjectID = "", string ObjectID = "");
		virtual void DrawLine(int x1, int y1, int x2, int y2, PlutoColor color) {};
		virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor, DesignObj_Orbiter *pObj=NULL);
		virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
		virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0), string ObjectID = "");
		virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point);
		virtual PlutoGraphic *GetBackground( PlutoRectangle &rect ) ;
		virtual PlutoGraphic *CreateGraphic();

		// Rendering
		virtual void RenderScreen(bool bRenderGraphicsOnly);

		virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, 
			bool bDisableAspectRatio = false, PlutoPoint point = PlutoPoint(0, 0), int nAlphaChannel = 255,
			string ParentObjectID = "", string ObjectID = "", string ObjectHash = "");

		void RemoveGraphic(string ObjectID);

		virtual void BeginPaint();
		virtual void EndPaint();
		virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0));

		virtual void AdjustWindowSize(int iWidth, int iHeight);
		virtual void DoHighlightObject();
		virtual void UnHighlightObject(bool bDeleteOnly = false);
		virtual void SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point );

		virtual void RenderObjectAsync(DesignObj_Orbiter *pObj);
		virtual void ShowProgress(int nPercent);

		virtual void ObjectOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj,					PlutoPoint *ptPopup = NULL);
		virtual void ObjectOffScreen(DesignObj_Orbiter *pObj);

		void WakeupFromCondWait();
		void OnIdle();

		/**
		* @brief A graphic is no longer on screen. Maybe remove it from cache
		*/
		virtual void GraphicOffScreen(vector<class PlutoGraphic*> *pvectGraphic);

		/**
		 *	Notify Orbiter Renderer that OpenGL windows was created	
		 */
		void WindowCreated() { m_bWindowCreated = true; }

		/**
		 *  Window creation status
		 */
		bool IsWindowCreated() { return m_bWindowCreated; }

		/**
		* Any post install actions here	
		*/
		virtual void PostInitializeActions();

		/**
		* @brief Render a popup
		*/
		virtual void RenderPopup(PlutoPopup *pPopup, PlutoPoint point, int EffectID);

		virtual bool HandleHidePopup(PlutoPopup* Popup);
		virtual bool HandleShowPopup(PlutoPopup* Popup, PlutoPoint Position, int EffectID);

		virtual void HandleRefreshCommand(string sDataGrid_ID);
		
		virtual void UpdateObjectImage(string sPK_DesignObj, string sType, char *pData, int iData_Size, 
			string sDisable_Aspect_Lock);
			
		/**
		* @brief initialize other classes, after the video mode was changed
		*/
		virtual void InitializeAfterSetVideoMode() {}

		/**
		* @brief initialize other classes, after WaitForRelativesIfOSD
		*/
		virtual void InitializeAfterRelatives() {};

		virtual void OnQuit();

		virtual void ProcessingInputEvent();

		virtual void DestroyGraphic(string sObjectID);

		virtual void ObjectRendered(DesignObj_Orbiter *pObj, PlutoPoint point);

		OpenGL3DEngine* Engine;	
		pthread_cond_t Condition;
		pluto_pthread_mutex_t Mutex; 

		
	};
}

#endif //__ORBITERRENDERER_OPENGL_H__
