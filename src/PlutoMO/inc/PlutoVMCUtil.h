#ifndef PLUTO_VMC_UTIL_H
#define PLUTO_VMC_UTIL_H

#include <coecntrl.h>
#include <coemain.h>
#include <coeccntx.h>
#include <coemain.h>

#include "VIPShared/RenderMenu.h"
#include <aknview.h>

// CONSTANTS
// UID of view
const TUid KCPlutoVMCUtilId = {10};

//------------------------------------------------------------------------------------------------------------------
class CPlutoVMCUtil: public RenderMenu, public CCoeStatic

{

public:

		CPlutoVMCUtil(TUid aUid, TScope=EThread);
		virtual ~CPlutoVMCUtil();

		void Setup(CCoeControl* pContainer, CAknView* pView);

		void BeginPaint(CWindowGc *pGC);
		void EndPaint();

		void Parse(TFileName iFileName);
		void SetAndSwitchToMenu(VIPMenuCollection *pVMC);
		void SetImage(unsigned char Type, unsigned long Size, const char *ImageBuf);
		void SetList(
			unsigned long x, 
			unsigned long y, 
			unsigned long Width, 
			unsigned long Height,
			RPointerArray<string> DatagridStringList
		);
		void SetCaptureKeyboardCommand(
			bool bOnOff, 
			bool bDataGrid, 
			bool bReset, 
			bool bTypePin, 
			int  iVariable, 
			string sText
		);

		void LocalKeyPressed(int KeyCode);
		void LocalDoRender();

		virtual void Redraw();

		bool GridExists()		{ return m_bGridExists; }

		virtual bool ScrollListUp();
		virtual bool ScrollListDown();
		virtual bool ScrollListPartialMatch();
		virtual bool ClearEdit();

		virtual bool SetVariable();
		virtual bool SelectCurrentItem();

		bool m_bRedrawOnlyGrid;
		bool m_bRedrawOnlyEdit;

		TRect m_ImageRect;

		bool m_bSimulation;

protected:
		// From RenderMenu
		virtual void MessageBox(const char *Message,const char *Title);
		virtual bool DrawText(const char *Text, MyRect &r);
		virtual void SetTextProperties(int Size,const char *Font,int R, int G, int B);
		virtual void SwitchToMenu(VIPMenu *pMenu);
		virtual bool DrawImage(const char *Filename,int X,int Y,int Width,int Height);
		virtual bool DrawImage(unsigned char ImageType,void *pGraphic,int GraphicSize,int X,int Y,int Width,int Height);

		virtual void DrawDataGrid(unsigned long X, unsigned long Y, unsigned long Width, unsigned long Height,
			RPointerArray<string> GridList);	
		virtual bool DrawEdit();

		virtual void OpenProgram(string ProgramName);
		virtual bool Draw3dRect(MyRect &r);
		

private:
		CWindowGc* m_pGC;

		CCoeControl* m_pContainer;
		CAknView* m_pView;

		CFont* m_CurrentFont;

		unsigned long m_uGridSelectedItem;
		unsigned long m_uGridTopItem;

		bool m_bGridExists;

		unsigned long m_uGridX;
		unsigned long m_uGridY;
		unsigned long m_uGridWidth;
		unsigned long m_uGridHeight;

		RPointerArray<string> m_GridList;
};
//------------------------------------------------------------------------------------------------------------------
#endif