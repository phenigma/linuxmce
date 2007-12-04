#include "UIWindowMasked.h"
//-----------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
#define OPAQUE_MASK			0xFF
#define TRANSPARENT_MASK	0x00
//-----------------------------------------------------------------------------------------------------
UIWindowMasked::UIWindowMasked() : m_hRgnCurrent(NULL), m_bFiltersChanged(false)
{
}
//-----------------------------------------------------------------------------------------------------
UIWindowMasked::~UIWindowMasked()
{
	if(NULL != m_hRgnCurrent)
	{
		DeleteObject(m_hRgnCurrent);
		m_hRgnCurrent = NULL;
	}
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::AddFilter(const PlutoRectangle& rect, const unsigned char *pFilter/* = NULL*/) 
{
	if(NULL == pFilter)
	{
		m_listRectangularRegions.push_back(rect);
	}
	else
	{
		m_listNonuniformRegions.push_back(make_pair(pFilter, rect));
	}

	m_bFiltersChanged = true;
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::RemoveFilter(const PlutoRectangle& rect)
{
	m_listRectangularRegions.remove(rect);

	m_bFiltersChanged = true;
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ClearFilters() 
{
	m_listRectangularRegions.clear();
	m_listNonuniformRegions.clear();
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ApplyFilters(HWND hWnd)
{
	if(NULL != hWnd && m_bFiltersChanged)
	{
		HRGN hRgnNew = CreateRectRgn(0, 0, 0, 0);
		
		ApplyDiscretFilters(hRgnNew, NULL);

		SetWindowRgn(hWnd, hRgnNew, TRUE);

		if(NULL != m_hRgnCurrent)
			DeleteObject(m_hRgnCurrent);

		m_hRgnCurrent = hRgnNew;

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Region: Applied filters!");

		m_bFiltersChanged = false;
	}
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ApplyDiscretFilters(HRGN hRgnNew, SDL_Surface *pSurface)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Region: Create empty region!");

	int nTitleHeigth = ::GetSystemMetrics(SM_CYSIZE);
	int nBorderDim = ::GetSystemMetrics(SM_CXBORDER);

	for(list<PlutoRectangle>::iterator it = m_listRectangularRegions.begin(); it != m_listRectangularRegions.end(); ++it)
	{
		PlutoRectangle rect = *it;

		PlutoRectangle rectOpaque(rect.X + nBorderDim, rect.Y + nBorderDim + nTitleHeigth, 
			rect.Width, rect.Height);

		ApplyOpaqueRegion(hRgnNew, rectOpaque);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Region: Applied region %d,%d,%d,%d!", rect.X, rect.Y, rect.Width, rect.Height);
	}

	for(list<pair<const unsigned char *, PlutoRectangle> >::iterator itn = m_listNonuniformRegions.begin();
		itn != m_listNonuniformRegions.end(); ++itn)
	{
		PlutoRectangle rect = itn->second;
		const unsigned char *pFilter = itn->first;

		int iY = 0;
		int iX = 0;				

		for (iY = 0; iY < rect.Height; iY++)
		{
			do
			{
				//skip over transparent pixels at start of lines.
				while (iX < rect.Width && *(pFilter + iY * rect.Width + iX) != TRANSPARENT_MASK)
					iX++;

				//remember this pixel
				int iLeftX = iX;
				//now find first non transparent pixel
				while (iX < rect.Width && *(pFilter + iY * rect.Width + iX) == TRANSPARENT_MASK)
					++iX;

				if(iX <= rect.Width)
				{
					PlutoRectangle rectOpaque(iLeftX, iY, iX - iLeftX, 1);

					rectOpaque.X += rect.X + nBorderDim;
					rectOpaque.Y += rect.Y + nBorderDim + nTitleHeigth;

					ApplyOpaqueRegion(hRgnNew, rectOpaque);
				}
			} 
			while(iX < rect.Width);

			iX = 0;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Region: Applied nonuniform region %d,%d,%d,%d!", rect.X, rect.Y, rect.Width, rect.Height);
	}
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ApplyOpaqueRegion(HRGN hRgnMaster, const PlutoRectangle& rect)
{
	//create temporary rectangular opaque region
	HRGN hRgnTemp = CreateRectRgn(rect.X, rect.Y, rect.X + rect.Width, rect.Y + rect.Height);

	//combine into main region.
	CombineRgn(hRgnMaster, hRgnMaster, hRgnTemp, RGN_OR);

	//delete it
	DeleteObject(hRgnTemp);
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ApplyOpaqueMask(SDL_Surface *pSurface, const PlutoRectangle& rect)
{
	for(int j = 0; j < rect.Height; j++)
		for(int i = 0; i < rect.Width; i++)
			putpixel(pSurface, rect.X + i, rect.Y + j, SDL_MapRGBA(pSurface->format, 255, 0, 0, 255)); 
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ApplyDiscretFilter(HRGN hRgnMaster, SDL_Surface *pSurface, const PlutoRectangle& rect)
{
	if(NULL != hRgnMaster)
		ApplyOpaqueRegion(hRgnMaster, rect);
	else
		ApplyOpaqueMask(pSurface, rect);
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::DumpMask(int nWidth, int nHeight)
{
	SDL_Surface *pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, nWidth, nHeight, 32, rmask, gmask, bmask, amask);
	SDL_FillRect(pSurface, NULL, SDL_MapRGBA(pSurface->format, 0, 255, 0, 255));

	ApplyDiscretFilters(NULL, pSurface);

	IMG_SavePNG(pSurface, "/home/mask-dump.png");
	SDL_FreeSurface(pSurface); 	
}
//-----------------------------------------------------------------------------------------------------

