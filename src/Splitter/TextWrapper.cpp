#include "TextWrapper.h"
#include "Orbiter/TextStyle.h"

#if ( defined( PROFILING ) )
	#ifdef WINCE
		#include "DCE/Logger.h"
		using namespace DCE;
	#endif
#endif

// Radu: I hope I'll get around to optimizing all this some time.
// Radu: From my point of view, this looks like its going to eat a lot of memory.

// WORKAROUND: here, RendererImage is typedef'ed to void
// RI should only be stored and passed when needed, not used in this code until the SDL problem is fixed
pair<int, int> GetWordWidth(string Word, string FontPath, TextStyle *pTextStyle, RendererImage * & RI, bool NewSurface = true);
int DoRenderToScreen(list<RendererImage *> &RI, int posX, int posY);
int DoRenderToSurface(SDL_Surface * Surface, list<void *> &RI, int posX, int posY);
void extDeleteRendererImage(RendererImage * & RI);

TextLineWrap::~TextLineWrap()
{
	Clear();
}

void TextLineWrap::Clear()
{
	T.Clear();
	lines.clear();

	// clearing pointers before clearing list
	for (list<ImageRow>::iterator i = ImageLines.begin(); i != ImageLines.end(); i++)
	{
		for (ImageRow::iterator j = i->begin(); j != i->end(); j++)
			extDeleteRendererImage(* j);
	}
	ImageLines.clear();
}

pair<int, int> TextLineWrap::WordWidth(string word, RendererImage * & RI,
							TextStyle * pTextStyle, bool NewSurface)
{
	return GetWordWidth(word, m_FontPath, pTextStyle, RI, NewSurface);
}

void TextLineWrap::AddRow(Row line, ImageRow ImageLine, LineAttr LA)
{
	if (line.size() > 0)
	{
		lines.insert(lines.end(), line);
		ImageLines.insert(ImageLines.end(), ImageLine);
		LineAttrData.insert(LineAttrData.end(), LA);
	}
}

void TextLineWrap::AddImageWord(ImageRow & line, RendererImage * word)
{
	line.insert(line.end(), word);
}

list<Row> & TextLineWrap::Wrap(string text, int atX, int atY, int W, int H,
							   string FontPath, TextStyle * pTextStyle, int PK_HorizAlignment,int PK_VertAlignment)
{
	string flags("");

	switch (PK_HorizAlignment)
	{
		case HORIZALIGNMENT_Center_CONST: flags += "c"; break;
		case HORIZALIGNMENT_Right_CONST: flags += "r"; break;
	}

	switch (PK_VertAlignment)
	{
		case VERTALIGNMENT_Middle_CONST: flags += "m"; break;
		case VERTALIGNMENT_Bottom_CONST: flags += "b"; break;
	}

	// prepend the default alignments if any
	string actualText = (flags != "" ? "~" + flags + "~" : "") + text;

	list<Row> origLines = T.Extract(actualText);
	list<Word> line;
	ImageRow ImageLine;
	RendererImage * RI;
	bool statU = false, statI = false, statB = false;
	LineAttr LAttr;
	pair<int, int> WW; // used to stand for "Word Width", but now it's "Word Dimensions" (first: Width; second: Height)
    int lastY = 0;

	m_FontPath = FontPath;
	m_FontFile = pTextStyle->m_sFont;
	m_FontSize = pTextStyle->m_iPixelHeight;

	Width = W; Height = H;
	origX = atX; origY = atY;
	lastX = 0;
	string Space = "";

	for (list<Row>::iterator i = origLines.begin(); i != origLines.end(); i++)
	{
		for (list<Word>::iterator j = i->begin(); j != i->end(); j++)
		{
			if ((* j)[0] == '~')
			{
				bool Not = false;
				for (Word::iterator k = j->begin(); k != j->end(); k++)
				{
					switch (* k)
					{
						case '~': break;
						case 'x':
						case 'X': Not = true; continue; break;
						case 'B': statB = Not ^ true; break;
						case 'I': statI = Not ^ true; break;
						case 'U': statU = Not ^ true; break;
						case 'l': LAttr.HAlign = HORIZALIGNMENT_Left_CONST; break;
						case 'c': LAttr.HAlign = HORIZALIGNMENT_Center_CONST; break;
						case 'r': LAttr.HAlign = HORIZALIGNMENT_Right_CONST; break;
						case 't': LAttr.VAlign = VERTALIGNMENT_Top_CONST; break;
						case 'm': LAttr.VAlign = VERTALIGNMENT_Middle_CONST; break;
						case 'b': LAttr.VAlign = VERTALIGNMENT_Bottom_CONST; break;
					}
					Not = false;
				}
				continue; // don't wrap formatting
			}
			bool statB_bk = pTextStyle->m_bBold;
			bool statU_bk = pTextStyle->m_bUnderline;
			bool statI_bk = pTextStyle->m_bItalic;

			pTextStyle->m_bBold = statB;
			pTextStyle->m_bUnderline = statU;
			pTextStyle->m_bItalic = statI;

			WW = WordWidth(Space + * j, RI, pTextStyle);

			if (lastX + WW.first >= Width && lastY + WW.second < Height)
			{
				LAttr.Width = lastX;
				LAttr.Height = WW.second;
				AddRow(line, ImageLine, LAttr);
                lastY += WW.second;
				line.clear();
				ImageLine.clear();
				LAttr.Width = lastX = 0;

				if( (lines.size()+1) * LAttr.Height > H )
					return lines;
				
				// re-render last word because it is going to start a new line
				WW = WordWidth(* j, RI, pTextStyle, false);
			}

			pTextStyle->m_bBold = statB_bk;
			pTextStyle->m_bUnderline = statU_bk;
			pTextStyle->m_bItalic = statI_bk;

			T.AddWord(line, * j);
			AddImageWord(ImageLine, RI);
			lastX += WW.first;
			Space = " ";
		}
		LAttr.Width = lastX;
		LAttr.Height = WW.second;

        if (lastY + WW.second < Height)
		    AddRow(line, ImageLine, LAttr);

        lastY += WW.second;
		line.clear();
		ImageLine.clear();
		lastX = 0;
		Space = "";
	}

	return lines;
}

void TextLineWrap::RenderToScreen()
{
	RenderToSurface(NULL);
}

void TextLineWrap::RenderToSurface(SDL_Surface * Surface)
{
	list<ImageRow>::iterator i1; list<LineAttr>::iterator i2;
	list<ImageRow>::reverse_iterator j1; list<LineAttr>::reverse_iterator j2;
	int X = 0, Y = 0;

	list<ImageRow> TopLines, MiddleLines, BottomLines;
	list<LineAttr> TopAttr, MiddleAttr, BottomAttr;

	for (i1 = ImageLines.begin(), i2 = LineAttrData.begin(); i1 != ImageLines.end(); i1++, i2++)
	{
		switch (i2->VAlign)
		{
			case VERTALIGNMENT_Top_CONST:
				TopLines.insert(TopLines.end(), * i1);
				TopAttr.insert(TopAttr.end(), * i2);
				break;

			case VERTALIGNMENT_Middle_CONST:
				MiddleLines.insert(MiddleLines.end(), * i1);
				MiddleAttr.insert(MiddleAttr.end(), * i2);
				break;

			case VERTALIGNMENT_Bottom_CONST:
				BottomLines.insert(BottomLines.end(), * i1);
				BottomAttr.insert(BottomAttr.end(), * i2);
				break;
		}
	}

	int TopBottomY, BottomTopY;

	for (i1 = TopLines.begin(), i2 = TopAttr.begin(); i1 != TopLines.end(); i1++, i2++)
	{
		switch (i2->HAlign)
		{
			case HORIZALIGNMENT_Left_CONST: X = 0; break;
			case HORIZALIGNMENT_Center_CONST: X = int(Width / 2.0 - i2->Width / 2.0); break;
			case HORIZALIGNMENT_Right_CONST: X = Width - i2->Width; break;
		}
		Surface == NULL ? DoRenderToScreen(* i1, origX + X, origY + Y)
			: DoRenderToSurface(Surface, * i1, origX + X, origY + Y);
		Y += i2->Height;
	}
	TopBottomY = Y;

	Y = Height;

	for (j1 = BottomLines.rbegin(), j2 = BottomAttr.rbegin(); j1 != BottomLines.rend(); j1++, j2++)
	{
		Y -= j2->Height;
		switch (j2->HAlign)
		{
			case HORIZALIGNMENT_Left_CONST: X = 0; break;
			case HORIZALIGNMENT_Center_CONST: X = int(Width / 2.0 - (* j2).Width / 2.0); break;
			case HORIZALIGNMENT_Right_CONST: X = Width - (* j2).Width; break;
		}
		Surface == NULL ? DoRenderToScreen(* j1, origX + X, origY + Y)
			: DoRenderToSurface(Surface, * j1, origX + X, origY + Y);
	}
	BottomTopY = Y;

	int MiddleHeight = 0;
	for (i2 = MiddleAttr.begin(); i2 != MiddleAttr.end(); i2++)
	{
		MiddleHeight += (* i2).Height;
	}

	Y = TopBottomY + int((BottomTopY - TopBottomY) / 2.0 - MiddleHeight / 2.0);
	for (i1 = MiddleLines.begin(), i2 = MiddleAttr.begin(); i1 != MiddleLines.end(); i1++, i2++)
	{
		switch (i2->HAlign)
		{
			case HORIZALIGNMENT_Left_CONST: X = 0; break;
			case HORIZALIGNMENT_Center_CONST: X = int(Width / 2.0 - i2->Width / 2.0); break;
			case HORIZALIGNMENT_Right_CONST: X = Width - i2->Width; break;
		}
		Surface == NULL ? DoRenderToScreen(* i1, origX + X, origY + Y)
			: DoRenderToSurface(Surface, * i1, origX + X, origY + Y);
		Y += i2->Height;
	}
}

// WORKAROUND: void * = SDL_Surface *
void WrapAndRenderText(SDL_Surface * Surface, string text, int X, int Y, int W, int H,
					   string FontPath, TextStyle *pTextStyle,int PK_HorizAlignment,int PK_VertAlignment)
{
H+=(H/10);
	TextLineWrap T;
	T.Wrap(text, X, Y, W, H, FontPath, pTextStyle,PK_HorizAlignment,PK_VertAlignment);
	T.RenderToSurface(Surface);
}
