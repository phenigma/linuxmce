#ifndef TextStyle_h
#define TextStyle_h

#include "SerializeClass/SerializeClass.h"
#include "SerializeClass/ShapesColors.h"

// Encapsulate a text style
class TextStyle : public SerializeClass
{
public:
	int m_iPK_Style,m_iPK_StyleVariation,m_iPK_Style_Selected,m_iPK_Style_Highlighed,m_iPK_Style_Alt;
	int m_iVersion; // This is for the selected, alt, or highlighted versions
	string m_sFont;
	PlutoColor m_ForeColor,m_BackColor,m_ShadowColor;
	int m_iPixelHeight;
	bool m_bBold, m_bItalic, m_bUnderline;
	int m_iShadowX, m_iShadowY;  // Pixels to offset a drop shadow
	int m_iBorderStyle;
	int m_iPK_HorizAlignment,m_iPK_VertAlignment;
	int m_iRotate;
	void *m_pTTF_Font; // Make this a void * for now, since trying to include the headers creates a nightmare.  We'll just cast

	TextStyle(class Row_StyleVariation *pRow_StyleVariation);
	TextStyle() { m_pTTF_Font=NULL; } // Needed for de-serializing

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + 
			m_iPK_Style + m_iPK_StyleVariation + m_iPK_Style_Selected + m_iPK_Style_Highlighed + m_iPK_Style_Alt + m_iVersion +
			m_sFont + 
			m_ForeColor + m_BackColor + m_ShadowColor + 
			m_iPixelHeight + 
			m_bBold + m_bItalic + m_bUnderline + 
			m_iShadowX + m_iShadowY +
			m_iBorderStyle + 
			m_iPK_HorizAlignment + m_iPK_VertAlignment + m_iRotate;
	}

	~TextStyle()
	{
//#ifdef ORBITER  -- hack, find a solution.  see void * above
//		if( m_pTTF_Font )  
//			TTF_CloseFont(m_pTTF_Font);
//#endif
	}
};

typedef map<int,TextStyle *> MapTextStyle;

#endif
