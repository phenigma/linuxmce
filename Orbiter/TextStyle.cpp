#include "PlutoUtils/CommonIncludes.h"	
#include "pluto_main/Table_Style.h"
#include "pluto_main/Table_StyleVariation.h"
#include "TextStyle.h"

TextStyle::TextStyle(class Row_StyleVariation *pRow_StyleVariation)
{
	m_iPK_Style = pRow_StyleVariation->FK_Style_get();
	m_iPK_StyleVariation = pRow_StyleVariation->PK_StyleVariation_get();
	m_iPK_Style_Selected = pRow_StyleVariation->FK_Style_getrow()->FK_Style_Selected_get();
	m_iPK_Style_Highlighed = pRow_StyleVariation->FK_Style_getrow()->FK_Style_Highlighted_get();
	m_iPK_Style_Alt = pRow_StyleVariation->FK_Style_getrow()->FK_Style_Alt_get();
	m_sFont = pRow_StyleVariation->Font_get();
	m_ForeColor = pRow_StyleVariation->ForeColor_get();
	m_BackColor = pRow_StyleVariation->BackColor_get();
	m_ShadowColor = pRow_StyleVariation->ShadowColor_get();
	m_iPixelHeight = pRow_StyleVariation->PixelHeight_get();
	m_bBold = pRow_StyleVariation->Bold_get()==1;
	m_bItalic = pRow_StyleVariation->Italic_get()==1;
	m_bUnderline = pRow_StyleVariation->Underline_get()==1;
	m_iShadowX = pRow_StyleVariation->ShadowX_get();
	m_iShadowY = pRow_StyleVariation->ShadowY_get();
	m_iBorderStyle = pRow_StyleVariation->BorderStyle_get();
	m_iPK_HorizAlignment = pRow_StyleVariation->FK_HorizAlignment_get();
	m_iPK_VertAlignment = pRow_StyleVariation->FK_VertAlignment_get();
	m_pTTF_Font=NULL;
 //	m_iRotate = pRow_StyleVariation->r;
	/*
	m_SelectedBGColor = pRow_StyleVariation->;
	m_HighlightedBGColor = pRow_StyleVariation->;
	m_sAltBGColor = pRow_StyleVariation->;
*/
}
