#include "GLFontRenderer.h"

#include "../../Orbiter.h"
#include "../../TextStyle.h"

#include "GLFontManager.h"

#include "../../../pluto_main/Define_VertAlignment.h" 
#include "../../../pluto_main/Define_HorizAlignment.h" 


GLFontRenderer::GLFontRenderer(int ScreenHeight,
							   std::string FontName,
							   int Height, 
							   int Style,
							   unsigned char R, 
							   unsigned char G,
							   unsigned char B)						   
{
	GLFontManager* FontManager = GLFontManager::GetInstance();
	Font = FontManager->GetFont(FontName, Height);
	this->R_ = R;
	this->G_ = G;
	this->B_ = B;
	this->Height_ = Height;
	this->Style_ = Style;
}

GLFontRenderer::~GLFontRenderer()
{
}

Orbiter* GLFontRenderer::OrbiterLogic()
{
	return NULL;
}

MeshFrame* GLFontRenderer::RenderText(string &TextToDisplay, PlutoRectangle &rPosition, int iPK_HorizAlignment,
									  int iPK_VertAlignment, string &sFont, PlutoColor &ForeColor, 
									  int iPixelHeight, bool bBold, bool bItalic, bool bUnderline,  
									  PlutoPoint point, PlutoRectangle &rectCalcLocation)
{
	GLFontTextureList * LetterWriter = Font->GetFontStyle(Style_, R_, G_, B_);
	std::string StrMessage;
	MeshContainer* Container = NULL;
	MeshFrame* Frame = new MeshFrame();

	if (TextToDisplay.find('\n') != TextToDisplay.npos)
	{
		StrMessage = TextToDisplay.substr(0, TextToDisplay.find('\n')-1);
		TextToDisplay = TextToDisplay.substr(TextToDisplay.find('\n')+1, TextToDisplay.length());
	}
	else 
	{
		StrMessage = TextToDisplay;
		TextToDisplay = "";
	}

	int Length = LetterWriter->TextOut(point.X, point.Y, StrMessage.c_str(), Container);
	MeshTransform Transform;
	switch (iPK_HorizAlignment)
	{
	case HORIZALIGNMENT_Left_CONST: 
		Transform.Translate(float(rectCalcLocation.X) , float(rectCalcLocation.Y), 0);
		break;

	case HORIZALIGNMENT_Center_CONST: 
		Transform.Translate(float(rectCalcLocation.X + (rectCalcLocation.Width - Length)/2),
			float(rectCalcLocation.Y), 0);
		break;

	default: 
		Transform.Translate(float(rectCalcLocation.X + (rectCalcLocation.Width - Length)), 
			float(rectCalcLocation.Y), 0);
		break;			
	}
	Frame->ApplyTransform(Transform);
	
	Frame->SetMeshContainer(Container);

	return Frame;
}
					  
MeshFrame* GLFontRenderer::TextOut(string &TextToDisplay,class DesignObjText *Text,
	TextStyle *pTextStyle, PlutoPoint point)
{
	
	MeshFrame * Result = new MeshFrame();
	Font->GetFontStyle(R_, G_, B_, Style_);
	PlutoRectangle rectLocation;
	rectLocation.X = point.X + Text->m_rPosition.X;
	rectLocation.Y = point.Y + Text->m_rPosition.Y;
	rectLocation.Width = Text->m_rPosition.Width;
	rectLocation.Height = Text->m_rPosition.Height;
	bool bMultiLine = TextToDisplay.find("\n") != string::npos;

	//handle escape sequences
	if(TextToDisplay.find("~S") != string::npos)
	{
		string sText = TextToDisplay;
		size_t nPos = 0;
		while((nPos = sText.find("~S")) != string::npos)
		{
			size_t nNextPos = nPos;

			nNextPos = sText.find("~", nNextPos + 1);
			if(nNextPos == string::npos)
				nNextPos = sText.length();
				

			string sPrevText = sText.substr(0, nPos);
			string sNextText = sText.substr(nNextPos+1);
			sText = sPrevText + sNextText;
		}
		TextToDisplay = sText;
		//TODO: fix crusesh, right now use a simplest code that remove styles
		// escape sequences
		/*
		int nPos;
		string sText = TextToDisplay;
		vector<string> vectTextPieces;
		size_t nPos = 0;
		while((nPos = sText.find("~S")) != string::npos)
		{
			size_t nNextPos = nPos;
			bMultiLine = TextToDisplay.find("\n") != string::npos;
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
			TextStyle *pPieceTextStyle = pTextStyle;
			
			if(nPos == 0)
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

			while(sTextToRender.length() != 0)
			{
				MeshFrame* Frame = RenderText(sTextToRender, Text->m_rPosition, Text->m_iPK_HorizAlignment, 
					Text->m_iPK_VertAlignment, pPieceTextStyle->m_sFont,pPieceTextStyle->m_ForeColor,
					pPieceTextStyle->m_iPixelHeight,pPieceTextStyle->m_bBold,pPieceTextStyle->m_bItalic,
					pPieceTextStyle->m_bUnderline, point, rectLocation);
				Result->AddChild(Frame);
			}
			
			if(bMultiLine)
				Text->m_rPosition.Y += rectLocation.Bottom() - rectLocation.Top();
			else
				Text->m_rPosition.X += rectLocation.Right() - rectLocation.Left(); 

		}

		Text->m_rPosition = plutoRect;
		*/
	}
//	else //normal rendering
	{
		int NoLines = 0;
		//Text->m_iPK_VertAlignment = VERTALIGNMENT_Middle_CONST;

		MeshTransform Transform;
		while(TextToDisplay.length() != 0)
		{
			MeshFrame* Frame = RenderText(
			TextToDisplay,Text->m_rPosition,Text->m_iPK_HorizAlignment,
			Text->m_iPK_VertAlignment,pTextStyle->m_sFont,pTextStyle->m_ForeColor,
			pTextStyle->m_iPixelHeight,pTextStyle->m_bBold, pTextStyle->m_bItalic, 
			pTextStyle->m_bUnderline, point, rectLocation);
			if (NoLines) 
				Transform.ApplyTranslate(0, float(Height_), 0);
			Frame->ApplyTransform(Transform);
			
			Result->AddChild(Frame);
			NoLines++;
		}
		switch(Text->m_iPK_VertAlignment) {
			case VERTALIGNMENT_Bottom_CONST:
				Transform.SetIdentity();
				Transform.Translate(0, float(rectLocation.Height - Height_ * NoLines), 0);
				Result->ApplyTransform(Transform);
				break;
			case VERTALIGNMENT_Middle_CONST:
				Transform.SetIdentity();
				Transform.Translate(0, float(rectLocation.Height - Height_ * NoLines)/2, 0);
				Result->ApplyTransform(Transform);
				break;
			default:;
		};

	}
	
	return Result;
}
