/*
 PlutoVMCUtil
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "plutomo.hrh"
#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>  // for Fonts. 
#include <e32def.h>

#include <aknviewappui.h>
#include <avkon.hrh>

#include "PlutoMOAppUi.h"
#include "BD_PC_SelectedFromList.h"
#include "BD_PC_SetVariable.h"
#include "BD/BD_WhatDoYouHave.h"

#include "PlutoUtils/PlutoDefs.h"

#include "PlutoVMCContainer.h"
#include "PlutoVMCUtil.h"
#include "PlutoVMCView.h"
#include "ImageLoader.h"
#include "Logger.h"

#ifdef __WINS__
//#define TEST_DATAGRID
#endif

//------------------------------------------------------------------------------------------------------------------
CPlutoVMCUtil::CPlutoVMCUtil(TUid aUid, TScope scop/*=EThread*/) : CCoeStatic(aUid, scop)
{
	m_bSimulation = false;
	m_pGC = NULL;

	m_pContainer = NULL;
	m_pView = NULL;

	m_uGridSelectedItem = 0;
	m_bRedrawOnlyGrid = false;
	m_bRedrawOnlyEdit = false;
	m_bGridExists = false;

	m_uGridX = 0;
	m_uGridY = 0;
	m_uGridWidth = 0;
	m_uGridHeight = 0;
	m_uGridTopItem = 0;

	m_GridList.Reset();

	//TEST! (testing showlist command)

#ifdef TEST_DATAGRID
	m_bGridExists = true;

	m_uGridX = 0;
	m_uGridY = 50;
	m_uGridWidth = 176;
	m_uGridHeight = 120;

	m_GridList.Append(new string("17:30 ProTV\nStirile ProTV"));
	m_GridList.Append(new string("17:35 ProTV\nStirile ProTV"));
	m_GridList.Append(new string("20:00 HBO - Braveheart"));
	m_GridList.Append(new string("17:40 ProTV\nStirile ProTV"));
	m_GridList.Append(new string("17:45 ProTV\nStirile ProTV"));
	m_GridList.Append(new string("17:50 ProTV\nStirile ProTV"));
	m_GridList.Append(new string("17:55 ProTV\nStirile ProTV"));
	m_GridList.Append(new string("18:30 ProTV - film"));
	m_GridList.Append(new string("19:30 ProTV\nStirile ProTV"));
	m_GridList.Append(new string("19:50 ProTV\nStirile ProTV"));

	m_bGridSendSelectedOnMove = 0;;
	m_uGridSelectedItem = 2;

	//TEST!! (testing capture keyboard command)
	m_CaptureKeyboardParam.bOnOff = true;
	m_CaptureKeyboardParam.bDataGrid = false;
	m_CaptureKeyboardParam.bReset = true;
	m_CaptureKeyboardParam.bTypePin = true;
	m_CaptureKeyboardParam.bTextBox = true;
	m_CaptureKeyboardParam.iVariable = 17;
	m_CaptureKeyboardParam.sVariableValue = "";
	m_CaptureKeyboardParam.TextX = 0;
	m_CaptureKeyboardParam.TextY = 0;
	m_CaptureKeyboardParam.TextWidth = 176;
	m_CaptureKeyboardParam.TextHeight = 25;
	m_CaptureKeyboardParam.Reset();	
#endif

	m_bEdit_BackspacePressed = false;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ CPlutoVMCUtil::~CPlutoVMCUtil()
{
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::Setup(CCoeControl* pContainer, CAknView* pView)
{
	m_pContainer = pContainer;
	m_pView = pView;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::BeginPaint(CWindowGc* pGC)
{
	m_pGC = pGC;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::EndPaint()
{
	m_pGC = NULL;

	m_bRedrawOnlyGrid = false;
	m_bRedrawOnlyEdit = false;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::SetAndSwitchToMenu(VIPMenuCollection *pVMC)
{
	m_pImageStatic_Type=0;
	m_pImageStatic_Size=0;
	if(NULL != m_pImageStatic_Data)
	{
		delete (char *)m_pImageStatic_Data;
		m_pImageStatic_Data = NULL;
	}

	if(NULL != m_pMenuCollection)
	{
		delete m_pMenuCollection;
		m_pMenuCollection = NULL;
	}

	m_pMenuCollection = pVMC;

	SwitchToMenu(m_pMenuCollection->m_pMenu_Starting);
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::SetList(
	unsigned long x, 
	unsigned long y, 
	unsigned long Width, 
	unsigned long Height,
	RPointerArray<string> List,
	bool bSendSelectedOnMove,
	bool bTurnOn,
	unsigned long iSelectedItem
)
{
	m_bGridExists = bTurnOn;
	m_bGridSendSelectedOnMove = bSendSelectedOnMove;

	if(bTurnOn)
	{
		m_uGridX		= x;
		m_uGridY		= y;
		m_uGridWidth	= Width;
		m_uGridHeight	= Height;

		m_GridList.Reset();

		for(int i = 0; i < List.Count(); i++)
		{
			m_GridList.Append(List[i]);
		}

		if(iSelectedItem >= 0 && iSelectedItem < List.Count())
			m_uGridSelectedItem = iSelectedItem;
		else
			m_uGridSelectedItem = 0;
	}

}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::SetCaptureKeyboardCommand(
	bool bOnOff, 
	bool bDataGrid, 
	bool bReset, 
	bool bTypePin, 
	int  iVariable, 
	string sText
)
{
	m_CaptureKeyboardParam.bOnOff = bOnOff;

	if(bOnOff)
	{
		m_CaptureKeyboardParam.bDataGrid = bDataGrid;
		m_CaptureKeyboardParam.bReset = bReset;
		m_CaptureKeyboardParam.bTypePin = bTypePin; 

		m_CaptureKeyboardParam.iVariable = iVariable;

		unsigned int msgpos = 0;
		string token;

		m_CaptureKeyboardParam.bTextBox = 0 != sText.length();

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextX = atoi(token.c_str());

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextY = atoi(token.c_str());

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextWidth = atoi(token.c_str());

		if((token = StringUtils::Tokenize(sText, ",", msgpos)) != "") 	
			m_CaptureKeyboardParam.TextHeight = atoi(token.c_str());
	}
	else
	{
		m_CaptureKeyboardParam.bDataGrid = false;
		m_CaptureKeyboardParam.bReset = false;
		m_CaptureKeyboardParam.bTypePin = false;
		m_CaptureKeyboardParam.bTextBox = false;

		m_CaptureKeyboardParam.iVariable = 0;
	}

	m_CaptureKeyboardParam.Reset();
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void CPlutoVMCUtil::MessageBox(const char *Message,const char *Title)
{
    CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
    informationNote->ExecuteLD(string(Message).Des());

	delete informationNote;
	informationNote = NULL;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::DrawImage(const char *Filename,int X,int Y,int Width,int Height)
{
	bool bResult = false;

	if(!Width)
		Width = SCREEN_WIDTH;

	if(!Height)
		Height = SCREEN_HEIGHT;

	TRect rect(X, Y, Width, Height);

	CFbsBitmap *pBitmap = CImageLoader::LoadBitmap(string(Filename).Des());
	m_pGC->BitBlt(rect.iTl, pBitmap);
	delete pBitmap;

	return bResult;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::DrawImage(unsigned char ImageType,void *pGraphic,int GraphicSize,int X,int Y,int Width,int Height)
{
	LOG("Drawing image, size : ");
	LOGN(GraphicSize);
	LOGN("\n");

	TFileName aName;

	char *Buff = (char *)pGraphic;

	HBufC8 *base_str = HBufC8::NewL(GraphicSize);
	TPtr8 pStr = base_str->Des();

	for (int i = 0; i < GraphicSize; i++)
		pStr.Append(TChar(Buff[i]));

	HBufC16 *pPath = HBufC16::NewL(256);
	TPtr16 aPath = pPath->Des();

	RFile file;
	RFs aFs;
	aFs.Connect();

	file.Temp(aFs, aPath, aName, EFileStream | EFileWrite);
	file.Write(pStr, GraphicSize);
	file.Close();

	LOG("Image saved on the disk : ");

	char* sFilename = new char[256];
	for(i = 0; i < aName.Length(); i++)
		sFilename[i] = aName[i];
	sFilename[i] = '\0';

	DrawImage(sFilename, X, Y, Width, Height);

	LOG("Image rendered : ");

    // Create file management object
    CFileMan* fileMan = CFileMan::NewL(aFs);
    CleanupStack::PushL(fileMan); 

	string FilesPath(aPath);
	FilesPath += "*.$$$";
	fileMan->Delete(FilesPath.Des());

    // Clean up
    CleanupStack::PopAndDestroy();

	aFs.Close();

	delete pPath;
	pPath = NULL;

	delete sFilename;
	sFilename = NULL;

	delete base_str;
	base_str = NULL;

	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::DrawText(const char *Text, MyRect &r)
{
	//TODO: Use Radu's word wrap class

	TPoint point(r.left, r.top);
    m_pGC->DrawText(string(Text).Des(), point);

	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void CPlutoVMCUtil::SetTextProperties(int Size,const char *Font,int R, int G, int B)
{
	CGraphicsDevice* screenDevice = CCoeEnv::Static()->ScreenDevice();
	
	if(NULL != m_CurrentFont)
	{
		//Discard and destroy the font
		m_pGC->DiscardFont();
		CCoeEnv::Static()->ReleaseScreenFont(m_CurrentFont);
	}

	TFontSpec myFontSpec(string(Font).Des(), Size); 
	m_CurrentFont = CCoeEnv::Static()->CreateScreenFontL(myFontSpec); 
	//screenDevice->GetNearestFontInTwips(m_CurrentFont, myFontSpec);

	TRgb color(R, G, B);
	m_pGC->SetPenColor(color);

	m_pGC->UseFont(m_CurrentFont);
}

//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void CPlutoVMCUtil::DrawDataGrid(unsigned long X, unsigned long Y, unsigned long Width, 
	unsigned long Height, RPointerArray<string> GridList)
{
	TRgb black(0,0,0);
	TRgb darkGray(85,85,85);
	TRgb liteGray(140,140,200);//light gray + blue
	TRgb white(255,255,255); // appears as blank screen gray-green color
	//TRgb blue(50,50,255); // appears as blank screen gray-green color
	//TRgb red(255, 0, 0); //darkRed

	TRgb blue(101,123,179); // appears as blank screen gray-green color
	TRgb blue_lite(205,214,237); // appears as blank screen gray-green color

	int RowHeight = 20;
	const int SmallOffsetX = 5;
	const int SmallOffsetY = 14;
	const int ShadowLen = 3;

	int VisibleItems = Height / RowHeight;
	RowHeight = Height / VisibleItems;

	m_pGC->SetBrushStyle(CGraphicsContext::ESolidBrush);
	m_pGC->SetPenStyle(CGraphicsContext::ENullPen);

	//background border - darkGray
	m_pGC->SetBrushColor(darkGray);
	TRect ShadowRect(X, Y, X + Width, Y + RowHeight * VisibleItems);
	m_pGC->DrawRect(ShadowRect);

	//let's see how many item are visible right now
	for(int i = m_uGridTopItem; i < m_uGridTopItem + VisibleItems; i++)
	{
		if(i >= GridList.Count()) 
			break;

		string *s = GridList[i];
		const char *pStr = s->c_str();
		int iBNpos = s->find('\n');

		if(iBNpos != -1)
			VisibleItems--;
	}

	//scroll the grid if need it
	while(m_uGridTopItem + VisibleItems - 1 < m_uGridSelectedItem)
		m_uGridTopItem++;
	while(m_uGridTopItem > m_uGridSelectedItem)
		m_uGridTopItem--;

	//let's see now how many items are visible
	VisibleItems = Height / RowHeight;
	for(i = m_uGridTopItem; i < m_uGridTopItem + VisibleItems; i++)
	{
		if(i >= GridList.Count()) 
			break;

		string *s = GridList[i];
		const char *pStr = s->c_str();
		int iBNpos = s->find('\n');

		if(iBNpos != -1)
			VisibleItems--;
	}

	unsigned long uGridBottomItem = m_uGridTopItem + VisibleItems;

	if(uGridBottomItem > GridList.Count())
		uGridBottomItem = GridList.Count();

	int iExpandOffset = 0;

	for(i = m_uGridTopItem; i < uGridBottomItem; i++)
	{
		string *s = GridList[i];
		const char *pStr = s->c_str();

		int iBNpos = s->find('\n');

		int strLen =  s->length();
		
		string s1("");
		string s2("");

		for(int j = 0; j < iBNpos; j++)
			s1.Append(s->operator [](j));

		for(j = iBNpos + 1; j < strLen; j++)
			s2.Append(s->operator [](j));

		int iItemSizeOffset = (iBNpos != -1) * RowHeight;

		if(m_uGridSelectedItem == i)
		{
			//selected item shadow : darkGray
			m_pGC->SetBrushColor(darkGray);
			TRect ShadowItemRect(
				X + ShadowLen, 
				Y + (i - m_uGridTopItem) * RowHeight + iExpandOffset + ShadowLen, 
				X + Width, 
				Y + (i - m_uGridTopItem + 1) * RowHeight + iExpandOffset + iItemSizeOffset
			);
			m_pGC->DrawRect(ShadowItemRect);

			//selected background color : bluelite
			m_pGC->SetBrushColor(blue_lite);
			TRect ItemRect(
				X, 
				Y + (i - m_uGridTopItem) * RowHeight + iExpandOffset, 
				X + Width - ShadowLen, 
				Y + (i - m_uGridTopItem + 1) * RowHeight + iExpandOffset - ShadowLen + iItemSizeOffset
			);
			m_pGC->DrawRect(ItemRect);

			//selected text color : white
			SetTextProperties(1, "Arial", 0, 0, 0);
		}
		else
		{
			//item shadow : darkGray
			m_pGC->SetBrushColor(darkGray);
			TRect ShadowItemRect(
				X + ShadowLen, 
				Y + (i - m_uGridTopItem) * RowHeight + iExpandOffset + 1, 
				X + Width, 
				Y + (i - m_uGridTopItem + 1) * RowHeight + iExpandOffset + iItemSizeOffset
			);
			m_pGC->DrawRect(ShadowItemRect);

			//background color : liteGray
			m_pGC->SetBrushColor(blue);
			TRect ItemRect(
				X, 
				Y + (i - m_uGridTopItem) * RowHeight + iExpandOffset, 
				X + Width - 1, 
				Y + (i - m_uGridTopItem + 1) * RowHeight + iExpandOffset - 1 + iItemSizeOffset
			);
			m_pGC->DrawRect(ItemRect);

			//unselected text color : while
			SetTextProperties(1, "Arial", 0, 0, 0);
		}

		if(iBNpos != -1)
		{
			const char *pStr1 = s1.c_str();
			const char *pStr2 = s2.c_str();

			//MyRect( top !!!, left, ...) << offf...
			MyRect RowRect1(Y + SmallOffsetY +  (i - m_uGridTopItem) * RowHeight + iExpandOffset, X + SmallOffsetX, Width, RowHeight);
			DrawText(pStr1, RowRect1);
			MyRect RowRect2(Y + SmallOffsetY +  (i - m_uGridTopItem) * RowHeight + iExpandOffset + iItemSizeOffset, X + SmallOffsetX, Width, RowHeight);
			DrawText(pStr2, RowRect2);			

			iExpandOffset += RowHeight;

//			if(!bLastItem)
//				uGridBottomItem --; //one item less
		}
		else
		{
			MyRect RowRect(Y + SmallOffsetY +  (i - m_uGridTopItem) * RowHeight + iExpandOffset, X + SmallOffsetX, Width, RowHeight);
			DrawText(pStr, RowRect);
		}

	}
}
//------------------------------------------------------------------------------------------------------------------


void CPlutoVMCUtil::Parse(TFileName iFileName)
{
	m_pImageStatic_Type=0;
	m_pImageStatic_Size=0;
	if(NULL != m_pImageStatic_Data)
	{
		delete (char *)m_pImageStatic_Data;
		m_pImageStatic_Data = NULL;
	}

	RFile file;
	RFs   aFs;
	aFs.Connect();

	file.Open(aFs, iFileName, EFileShareReadersOnly | EFileStream | EFileRead);

	TInt Status = KErrNone;
	TInt FileSize;

	file.Size(FileSize);

	HBufC8* Buffer = HBufC8::NewL(FileSize);
	TPtr8 pStr = Buffer->Des();
	
	HBufC8* TempBuff = HBufC8::NewL(1024);
	TPtr8 pTempBuff = TempBuff->Des();

	while(Status == KErrNone)
	{
		TInt Status = file.Read(pTempBuff, 1024);

		switch(Status)
		{
		case KErrOverflow:
			MessageBox("KErrOverflow: You tried to read more then descriptor's maximum length", "Error");
			break;

		case KErrGeneral:
			MessageBox("KErrGeneral: General error", "Error");
			break;
		}

		if(pTempBuff.Length() > 0)
			pStr.Append(pTempBuff);
		else
			break; //the eof
	}

	delete TempBuff;
	TempBuff = NULL;
	
	file.Close();

	char *ch_buff = new char[pStr.Length()];

	for (int i = 0; i < pStr.Length(); i++)
	{
		char ch = (char)pStr[i];
		ch_buff[i] = ch;
	}

	if(NULL != m_pMenuCollection)
	{
		delete m_pMenuCollection;
		m_pMenuCollection = NULL;
	}

	m_pMenuCollection = new VIPMenuCollection(FileSize, ch_buff);

	delete [] ch_buff;
	ch_buff = NULL;

	delete Buffer;
	Buffer = NULL;
	
	SwitchToMenu(m_pMenuCollection->m_pMenu_Starting);
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void CPlutoVMCUtil::Redraw()
{
	((CPlutoVMCView *)m_pView)->Refresh();
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::LocalDoRender()
{
	if(NULL != m_pMenu || (m_pImageStatic_Size && m_pImageStatic_Data))
	{
		m_pGC->SetPenStyle(CGraphicsContext::ENullPen);
		m_pGC->SetBrushColor(KRgbGray);
		m_pGC->SetBrushStyle(CGraphicsContext::ESolidBrush);
		
		if(!m_bRedrawOnlyGrid && !m_bRedrawOnlyEdit)
		{
			TRect aRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			m_pGC->DrawRect(aRect);

			DoRender();
		}

		if(m_bGridExists)
		{
			DrawDataGrid(m_uGridX, m_uGridY, m_uGridWidth, m_uGridHeight, m_GridList);
		}			

		if(m_CaptureKeyboardParam.bTextBox)
			DrawEdit();
	}
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ void CPlutoVMCUtil::SwitchToMenu(VIPMenu *pMenu)
{
	if( !pMenu )
		return;

	m_pMenu = pMenu;
	// Just get the first one

	MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuInputs,VIPMenuInput,m_pMenuInput,itm);

	// Go through all of them
	MYSTL_ITERATE_LONG_MAP(m_pMenu->m_mapMenuInputs,VIPMenuInput,pInput,itm2)
	{
		if( pInput->m_iClearVariable )
		{
			VIPVariable *pVariable = m_pMenuCollection->m_mapVariables_Find(pInput->m_iVariableID);
			if( pVariable )
				pVariable->m_sCurrentValue="";
		}
	}
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::SetImage(unsigned char Type, unsigned long Size, const char *ImageBuf)
{	
	m_pImageStatic_Type=0;
	m_pImageStatic_Size=0;
	if(NULL != m_pImageStatic_Data)
	{
		delete (char *)m_pImageStatic_Data;
		m_pImageStatic_Data = NULL;
	}

	if(NULL != m_pMenuCollection)
	{
		delete m_pMenuCollection;
		m_pMenuCollection = NULL;
	}

	m_pImageStatic_Type = Type; 
	m_pImageStatic_Size = Size;
	m_pImageStatic_Data = ImageBuf;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::ScrollListUp()
{
	//LOG("SCROLL_LIST_UP\n");

	m_bRedrawOnlyGrid = true;
	
	if(m_uGridSelectedItem > 0)
	{
		m_uGridSelectedItem--;

		if(m_bGridSendSelectedOnMove)
			SelectCurrentItem();

		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::ScrollListDown()
{
	//LOG("SCROLL_LIST_DOWN\n");

	m_bRedrawOnlyGrid = true;

	if(m_uGridSelectedItem < m_GridList.Count() - 1)
	{
		m_uGridSelectedItem++;

		if(m_bGridSendSelectedOnMove)
			SelectCurrentItem(); 
		
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::ScrollListPartialMatch()
{
	bool bResult = false;
	string s = m_CaptureKeyboardParam.sVariableValue;	

	const char* pStr = s.c_str(); //debug

	for(int i = 0; i < m_GridList.Count(); i++)
	{
		string item = *(m_GridList[i]);

		const char *pItem = item.c_str(); //debug

		if(s.length() > item.length())
			continue;

		bool bMatch = true;

		for(int j = 0; j < s.length(); j++)
		{
			char c = item[j];

			if('A' <= c && c <= 'Z') 
				c = c - 'A' + 'a';

			if(s[j] != c)
			{
				bMatch = false;
				break;
			}
		}

		if(bMatch)
		{
			if(m_uGridSelectedItem != i)
				bResult = true;

			m_uGridSelectedItem = i;
			return bResult;
		}
	}	

	if(m_uGridSelectedItem != 0)
		bResult = true;

	m_uGridSelectedItem = 0;
	return bResult;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::SelectCurrentItem()
{
	//LOG("SELECT_CURRENT_ITEM\n");

	if(
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor
	)
	{
		BDCommand *pCommand = new BD_PC_SelectedFromList(m_uGridSelectedItem); 
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor->AddCommand(pCommand);
	}

	return false; //don't redraw
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::DrawEdit()
{
	TRgb black(0,0,0);
	TRgb darkGray(85,85,85);
	TRgb liteGray(170,170,170);
	TRgb white(255,255,255); // appears as blank screen gray-green color
	TRgb blue(101,123,179); // appears as blank screen gray-green color
	TRgb blue_lite(205,214,237); // appears as blank screen gray-green color

	int EditX = m_CaptureKeyboardParam.TextX;
	int EditY = m_CaptureKeyboardParam.TextY;
	int EditWidth = m_CaptureKeyboardParam.TextWidth;
	int EditHeight = m_CaptureKeyboardParam.TextHeight;

	m_pGC->SetBrushStyle(CGraphicsContext::ESolidBrush);
	m_pGC->SetPenStyle(CGraphicsContext::ENullPen);

	m_pGC->SetBrushColor(black); //(liteGray);
	TRect ShadowRect(EditX, EditY, EditX + EditWidth, EditY + EditHeight);
	m_pGC->DrawRect(ShadowRect);

	m_pGC->SetBrushColor(blue_lite);
	TRect rect(EditX + 1, EditY + 1, EditX + EditWidth - 1, EditY + EditHeight - 1);
	m_pGC->DrawRect(rect);

	SetTextProperties(1, "Swiss", 0, 0, 0);
	MyRect RowRect(EditY + 15, EditX + 15, EditWidth, EditHeight);

	const char *pStr = m_CaptureKeyboardParam.sVariableValue.c_str();

	if(m_CaptureKeyboardParam.bTypePin)
	{
		string Temp = "";

		int len = m_CaptureKeyboardParam.sVariableValue.length();
		for(int i = 0; i < len - 1; i++)
			Temp.Append('*');

		if(len > 0)
		{
			//uncomment this to have Dan's behavior
			/*
			if(!m_bEdit_BackspacePressed)
				Temp.Append(m_CaptureKeyboardParam.sVariableValue[len - 1]);
			else
			*/
				Temp.Append('*');
		}
		


		Temp.Append('\0');
		
		const char *pStrTemp = Temp.c_str();
		DrawText(pStrTemp, RowRect);
	}
	else		
		DrawText(pStr, RowRect);	

	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::ClearEdit()
{
	if(m_CaptureKeyboardParam.sVariableValue.length() == 0)
		return false;

	string s = m_CaptureKeyboardParam.sVariableValue;
	m_CaptureKeyboardParam.sVariableValue = "";

	for(int i = 0; i < s.length() - 1; i++)
		m_CaptureKeyboardParam.sVariableValue += s[i];

	ScrollListPartialMatch();

	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::ClearAllEdit()
{
	if(m_CaptureKeyboardParam.sVariableValue.length() == 0)
		return false;

	m_CaptureKeyboardParam.sVariableValue = "";
	ScrollListPartialMatch();

	return true;
}
//------------------------------------------------------------------------------------------------------------------
/*virtual*/ bool CPlutoVMCUtil::SetVariable()
{
	if(m_CaptureKeyboardParam.iVariable != 0)
	{
		if(((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor)
		{
			BDCommand *pCommand = new BD_PC_SetVariable(
				m_CaptureKeyboardParam.iVariable, 
				m_CaptureKeyboardParam.sVariableValue
			);

			((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor->AddCommand(pCommand);
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::LocalKeyPressed(int KeyCode) 
{ 
	RenderMenu::KeyPressed(KeyCode);

	BDCommandProcessor_Symbian_Base* pBDCommandProcessor = 
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor;

	if(pBDCommandProcessor) //any bt connection ?
	{
		BD_WhatDoYouHave *pBD_WhatDoYouHave = new BD_WhatDoYouHave();
		pBDCommandProcessor->AddCommand(pBD_WhatDoYouHave);
		pBDCommandProcessor->ProcessCommands(!m_bSimulation);
	}
}
//------------------------------------------------------------------------------------------------------------------
void CPlutoVMCUtil::OpenProgram(string ProgramName)
{
	//TODO: implement this
	CPlutoMOAppUi* pApp = (CPlutoMOAppUi*)(CCoeEnv::Static()->AppUi());

	pApp->iURL.Copy(pApp->iURLClone);
	pApp->iURL.Append(ProgramName.Des());
	pApp->LaunchBrowser();
}
//------------------------------------------------------------------------------------------------------------------
bool CPlutoVMCUtil::Draw3dRect(MyRect &rect)
{
	TRgb black(0,0,0);
	TRgb darkGray(85,85,85);
	TRgb liteGray(140,140,200);//light gray + blue
	TRgb white(255,255,255); // appears as blank screen gray-green color
	TRgb blue(50,50,255); // appears as blank screen gray-green color
	TRgb red(255, 0, 0); //darkRed

	
	//m_pGC->SetBrushStyle(CGraphicsContext::ESolidBrush);
	//m_pGC->SetPenStyle(CGraphicsContext::ENullPen);

	//background border - darkGray
	//m_pGC->SetBrushColor(darkGray);

	//TRect ItemRect(rect.left, rect.top, rect.right, rect.bottom);
	//m_pGC->DrawRect(ItemRect);

	return true;
}
//------------------------------------------------------------------------------------------------------------------
