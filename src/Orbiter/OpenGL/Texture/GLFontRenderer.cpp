#include "GLFontRenderer.h"

#include "../../Orbiter.h"
#include "../../TextStyle.h"


GLFontRenderer::GLFontRenderer(int ScreenHeight,
							   std::string FontName,
							   int Height, 
							   int Style,
							   unsigned char R, 
							   unsigned char G,
							   unsigned char B)
							   
{
	Font.MapFont(FontName, Height, Style, R, G, B);
}

GLFontRenderer::~GLFontRenderer()
{
}

Orbiter* GLFontRenderer::OrbiterLogic()
{
	return NULL;
}

MeshFrame* GLFontRenderer::RenderText(string &TextToDisplay,PlutoRectangle &rPosition,int iPK_HorizAlignment,int iPK_VertAlignment,string &sFont,PlutoColor &ForeColor,int iPixelHeight,bool bBold,bool bItalic,bool bUnderline, PlutoPoint point, RECT &rectCalcLocation)
{
	return NULL;
}
					  
MeshFrame* GLFontRenderer::TextOut(std::string TextToDisplay, DesignObjText *Text, 
								   TextStyle* pTextStyle, PlutoPoint point, RECT &rectLocation)
{

/*	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);
	*/

	//handle escape sequences
	if(TextToDisplay.find("~S") != string::npos)
	{   
		string sText = TextToDisplay;
		vector<string> vectTextPieces;
		size_t nPos = 0;
		bool bMultiLine = TextToDisplay.find("\n") != string::npos;
		while((nPos = sText.find("~S")) != string::npos)
		{
			size_t nNextPos = nPos;
			if(nPos == 0)
			{
				nNextPos = sText.find("~S", nPos + 1);

				if(nNextPos == string::npos)
					nNextPos = sText.length();

				if(sText.find("~", nNextPos + 1) == string::npos)
					nNextPos = sText.length();
			}

			string sTextPiece = sText.substr(0, nNextPos);
			vectTextPieces.push_back(sTextPiece);
			sText = sText.substr(nNextPos);
		}

		PlutoRectangle plutoRect = Text->m_rPosition;

		vector<string>::iterator it;
		for(it = vectTextPieces.begin(); it != vectTextPieces.end(); it++)
		{
			string sTextPiece = *it;
			string sTextToRender = sTextPiece;
			size_t nPos = sTextPiece.find("~S");
			TextStyle *pPieceTextStyle = pTextStyle;			if(nPos == 0)
			{
				int nNextPos = int(sTextPiece.find("~", nPos + 1));
				string sTextStyleNumber = sTextPiece.substr(nPos + 2, nNextPos - 2);
				int nTextStyle = atoi(sTextStyleNumber.c_str());
				pPieceTextStyle = OrbiterLogic()->m_mapTextStyle_Find(nTextStyle);

				sTextToRender = sTextPiece.substr(nNextPos + 1);

				if(!pPieceTextStyle)
				{
					g_pPlutoLogger->Write(LV_CRITICAL, "Text style not found %d. Using default!", nTextStyle);
					pPieceTextStyle = pTextStyle;
				}
			}

			Font = new GLFontTextureList(pPieceTextStyle->m_iPixelHeight);
			Font->MapFont(
				pPieceTextStyle->m_sFont, 
				pPieceTextStyle->m_iPixelHeight,
				Style_,
				pPieceTextStyle->m_ForeColor.R(),
				pPieceTextStyle->m_ForeColor.G(),
				pPieceTextStyle->m_ForeColor.B()
				);

			RenderText(sTextToRender, Text->m_rPosition, Text->m_iPK_HorizAlignment, 
				Text->m_iPK_VertAlignment, pPieceTextStyle->m_sFont,pPieceTextStyle->m_ForeColor,
				pPieceTextStyle->m_iPixelHeight,pPieceTextStyle->m_bBold,pPieceTextStyle->m_bItalic,
				pPieceTextStyle->m_bUnderline, point, rectLocation);

			/*if(bMultiLine)
				Text->m_rPosition.Y += rectLocation.bottom - rectLocation.top;
			else
				Text->m_rPosition.X += rectLocation.right - rectLocation.left; */
		}

		//Text->m_rPosition = plutoRect;
	}
	else //normal rendering
	{
		/*
		RenderText(hdc,TextToDisplay,Text->m_rPosition,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment,
			pTextStyle->m_sFont,pTextStyle->m_ForeColor,pTextStyle->m_iPixelHeight,pTextStyle->m_bBold,pTextStyle->m_bItalic,pTextStyle->m_bUnderline,
			point, rectLocation);
			*/
	}

	return NULL;
}