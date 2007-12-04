#include "UIWindowMasked.h"
//-----------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
#define OPAQUE_MASK 0xFF
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

	m_bFiltersChanged = true;
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ApplyFilters(HWND hWnd)
{
	if(NULL != hWnd && m_bFiltersChanged)
	{
		HRGN hRgnNew = CreateRectRgn(0, 0, 0, 0);
		
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Region: Create empty region!");

		int nTitleHeigth = ::GetSystemMetrics(SM_CYSIZE);
		int nBorderWidth = ::GetSystemMetrics(SM_CXBORDER);

		for(list<PlutoRectangle>::iterator it = m_listRectangularRegions.begin(); it != m_listRectangularRegions.end(); ++it)
		{
			PlutoRectangle rect = *it;

			PlutoRectangle rectOpaque(rect.X + nBorderWidth, rect.Y + nBorderWidth + nTitleHeigth, 
				rect.Width, rect.Height);

			ApplyOpaqueRegion(hRgnNew, rectOpaque);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Region: Added region %d,%d,%d,%d!", rect.X, rect.Y, rect.Width, rect.Height);
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
					while (iX < rect.Width && *(pFilter + iY * rect.Width + iX) == OPAQUE_MASK)
						iX++;

					//remember this pixel
					int iLeftX = iX;
					//now find first non transparent pixel
					while (iX < rect.Width && *(pFilter + iY * rect.Width + iX) != OPAQUE_MASK)
						++iX;

					PlutoRectangle rectOpaque(
						rect.X + nBorderWidth + iLeftX, 
						rect.Y + nBorderWidth + nTitleHeigth + iY, 
						iX, 
						iY + 1);

					ApplyOpaqueRegion(hRgnNew, rectOpaque);
				} 
				while(iX < rect.Width);

				iX = 0;
			}
		}

		SetWindowRgn(hWnd, hRgnNew, TRUE);

		if(NULL != m_hRgnCurrent)
			DeleteObject(m_hRgnCurrent);

		m_hRgnCurrent = hRgnNew;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Region: Applied region!");

		m_bFiltersChanged = false;
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
void UIWindowMasked::DumpMask(int nWidth, int nHeight)
{
	SDL_Surface *pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, nWidth, nHeight, 32, rmask, gmask, bmask, amask);
	SDL_FillRect(pSurface, NULL, SDL_MapRGBA(pSurface->format, 128, 128, 128, 0));

	int nTitleHeigth = ::GetSystemMetrics(SM_CYSIZE);
	int nBorderWidth = ::GetSystemMetrics(SM_CXBORDER);

	for(list<PlutoRectangle>::iterator it = m_listRectangularRegions.begin(); it != m_listRectangularRegions.end(); ++it)
	{
		PlutoRectangle rect = *it;
		PlutoRectangle rectOpaque(rect.X + nBorderWidth, rect.Y + nBorderWidth + nTitleHeigth, 
			rect.Width, rect.Height);

		ApplyOpaqueMask(pSurface, rectOpaque);
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
				while (iX < rect.Width && *(pFilter + iY * rect.Width + iX) == OPAQUE_MASK)
					iX++;

				//remember this pixel
				int iLeftX = iX;
				//now find first non transparent pixel
				while (iX < rect.Width && *(pFilter + iY * rect.Width + iX) != OPAQUE_MASK)
					++iX;

				if(iLeftX < nWidth)
				{
					PlutoRectangle rectOpaque(
						rect.X + nBorderWidth + iLeftX, 
						rect.Y + nBorderWidth + nTitleHeigth + iY, 
						iX - iLeftX, 
						1);

					ApplyOpaqueMask(pSurface, rectOpaque);
				}
			} 
			while(iX < rect.Width);

			iX = 0;
		}
	}

	IMG_SavePNG(pSurface, "/home/mask-dump.png");
	SDL_FreeSurface(pSurface); 	
}
//-----------------------------------------------------------------------------------------------------
void UIWindowMasked::ApplyOpaqueMask(SDL_Surface *pSurface, const PlutoRectangle& rect)
{
	for(int j = 0; j < rect.Height; j++)
		for(int i = 0; i < rect.Width; i++)
			putpixel(pSurface, rect.X + i, rect.Y + j, SDL_MapRGBA(pSurface->format, 128, 128, 128, OPAQUE_MASK)); 
}
//-----------------------------------------------------------------------------------------------------

