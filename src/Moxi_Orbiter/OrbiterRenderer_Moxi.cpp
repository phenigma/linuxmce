/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "OrbiterRenderer_Moxi.h"
#include "Moxi_Orbiter.h"
#include "../Gen_Devices/AllCommandsRequests.h"

#include "../Orbiter/SDL/JpegWrapper.h"
#include "../Orbiter/Orbiter.h"
#include "../Orbiter/ScreenHistory.h"
using namespace DCE;

#include "SDL_rotozoom.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_video.h>
#include <SDL_rwops.h>

#include "sge.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "png.h"
//-----------------------------------------------------------------------------------------------------
void SaveImageToFile(struct SDL_Surface *pScreenImage, string FileName);
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_Moxi::OrbiterRenderer_Moxi(Orbiter *pOrbiter) : OrbiterRenderer_SDL(pOrbiter)
{

}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_Moxi::~OrbiterRenderer_Moxi()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_Moxi::DisplayImageOnScreen(SDL_Surface *pScreenImage)
{
	PLUTO_SAFETY_LOCK(sm,OrbiterLogic()->m_ScreenMutex);

	if(NULL == OrbiterLogic()->m_pScreenHistory_Current)
		return;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DisplayImageOnScreen Current screen: %s",  
		OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());

	Moxi_Orbiter *pMoxi_Orbiter = dynamic_cast<Moxi_Orbiter *>(OrbiterLogic());

	double ZoomX = 512 / double(pScreenImage->w);
	double ZoomY = 512 / double(pScreenImage->h);

	//resize here
	SDL_Surface *pResizedSurface = zoomSurface(pScreenImage, ZoomX, ZoomY, SMOOTHING_ON);

	int nSize = pResizedSurface->h * pResizedSurface->pitch;
	char *pData = new char[nSize];
	memcpy(pData, pResizedSurface->pixels, nSize);

	SDL_FreeSurface(pResizedSurface);

	CMD_Update_Object_Image_DT cmd_Update_Object_Image_DT(OrbiterLogic()->m_dwPK_Device, 
		DEVICETEMPLATE_Moxi_PVR_CONST, BL_SameHouse,
		OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(),
		"jpg", pData, nSize, ""
	);
	OrbiterLogic()->SendCommand(cmd_Update_Object_Image_DT);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_Moxi::BeginPaint()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_Moxi::EndPaint()
{
	DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
void SaveImageToFile(struct SDL_Surface *pScreenImage, string FileName)
{
	SDL_Surface * Drawing = pScreenImage;

	png_bytepp image_rows;
	FILE * File;
	int BitsPerColor;

	File = fopen(FileName.c_str(), "wb");

	image_rows = new png_bytep[Drawing->h];
	for (int n = 0; n < Drawing->h; n++)
		image_rows[n] = (unsigned char *) Drawing->pixels + n * Drawing->pitch;

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop png_info = png_create_info_struct(png_ptr);

	png_init_io(png_ptr, File);
	png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	BitsPerColor = Drawing->format->BitsPerPixel / Drawing->format->BytesPerPixel;
	png_set_IHDR(png_ptr, png_info, Drawing->w, Drawing->h, BitsPerColor, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, png_info);
	png_write_image(png_ptr, image_rows);
	png_write_end(png_ptr, png_info);

	delete [] image_rows;

	png_destroy_write_struct(&png_ptr, &png_info);
	fclose(File);
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_Moxi::DisplayProgress(string, int)
{
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_Moxi::DisplayProgress(string, const map<string, bool> &, int)
{
	return false;
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer_Moxi::PromptUser(string, int, map<int,string> *)
{
	return 0;
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer_Moxi::PromptFor(string)
{
	return 0;
}
//-----------------------------------------------------------------------------------------------------
