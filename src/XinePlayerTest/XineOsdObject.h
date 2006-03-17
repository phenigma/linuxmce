#ifndef XineOsdObject_h
#define XineOsdObject_h

#include <xine.h>

class XineStream;

class XineOsdObject
{
	friend class XineStream;

private:
	xine_osd_t *m_OsdObject;
	XineStream *m_OwningStream;
	
	int m_X, m_Y, m_Width, m_Height;
	bool m_Valid;
	
	operator xine_osd_t*()
	{
		return m_OsdObject;
	}

public:
	XineOsdObject();
	
	bool Initialize(XineStream *pStream, int x, int y, int width, int height);
	bool IsValid();
	
	enum PaletteType {paletteRGB, paletteYUV};
	bool SetPalette(uint32_t *colorsComponents, uint8_t *transparencyLevel, int colorsCount, PaletteType paletteType);
	
	void DrawRect(int x, int y, int width, int height, int color, bool filled);
};

#endif
