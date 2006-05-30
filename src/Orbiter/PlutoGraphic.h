#ifndef PLUTOGRAPHIC_H
#define PLUTOGRAPHIC_H

#define GRAPHIC_NORMAL		 0
#define GRAPHIC_SELECTED	-1
#define GRAPHIC_HIGHLIGHTED	-2

#include "PlutoUtils/GraphicFormat.h"
#include <algorithm>
//-------------------------------------------------------------------------------------------------------
// Graphic memory management.  GR_DYNAMIC means that this is a dynamic image
// and there's no reason to ever keep it.  Discardonchange may keep the image
// if there's enough system memory. 
//-------------------------------------------------------------------------------------------------------
enum eGraphicManagement { GR_KEEPUNCOMPRESSED, GR_KEEPCOMPRESSED, GR_DISCARDONCHANGE, GR_DYNAMIC, GR_STORAGE };

// Windows-style RGB management
//-------------------------------------------------------------------------------------------------------
#ifndef RGB
	#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
	#define GetRValue(rgb)      ((BYTE)(rgb))
	#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
	#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
#endif
//-------------------------------------------------------------------------------------------------------
enum GraphicType
{
	gtSDLGraphic,
	gtPocketFrogGraphic,
	gtOpenGLGraphic
};
//-------------------------------------------------------------------------------------------------------
namespace DCE
{
	class Orbiter;
};

using namespace DCE;
//-------------------------------------------------------------------------------------------------------
class PlutoGraphic
{
public:
	PlutoGraphic();
	PlutoGraphic(Orbiter *pOrbiter);
	PlutoGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pOrbiter);

	virtual ~PlutoGraphic();
	virtual PlutoGraphic* Clone() = 0; //Must be implemented

	Orbiter *m_pOrbiter;

	virtual void Initialize();
	virtual GraphicType GraphicType_get()=0;  // Must be implemented
	virtual bool IsEmpty()=0; //Must be implemented
	virtual bool LoadGraphic(char *pData, size_t iSize,int iRotation=0) = 0; //Must be implemented
	virtual void Clear() = 0; //Must be implemented
	virtual PlutoGraphic *GetHighlightedVersion() { return NULL; } // Return the same object in a highlighted state
	virtual bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize) = 0;

	enum eGraphicManagement m_GraphicManagement;
	enum eGraphicFormat m_GraphicFormat;

	int Width, Height;
	string m_Filename;
};
//-------------------------------------------------------------------------------------------------------
typedef list<PlutoGraphic*> PlutoGraphicList;
typedef vector<PlutoGraphic *> VectorPlutoGraphic;
//-------------------------------------------------------------------------------------------------------
#endif
