/*
 OrbiterSDLBluetooth
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "OrbiterSDLBluetooth.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCE/Logger.h"
#include "OrbiterSDLBluetooth.h"
#include "SerializeClass/ShapesColors.h"
#include "BD/BDCommandProcessor.h"
#include "VIPShared/BD_CP_ShowImage.h"
#include "VIPShared/BD_CP_ShowList.h"
#include "VIPShared/BD_CP_CaptureKeyboard.h"
#include "VIPShared/BD_CP_SimulateEvent.h"

#include "VIPShared/PlutoPhoneCommands.h"

#include "DataGrid.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_video.h>
#include <SDL_rwops.h>

#include "sge.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "png.h"

using namespace DCE;

//-----------------------------------------------------------------------------------------------------
OrbiterSDLBluetooth::OrbiterSDLBluetooth(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, int nImageWidth, int nImageHeight)
    : OrbiterSDL(DeviceID, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, nImageHeight),
    m_pBDCommandProcessor(pBDCommandProcessor)
{
    m_bShowListSent = false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterSDLBluetooth::~OrbiterSDLBluetooth()
{
}
//-----------------------------------------------------------------------------------------------------
int sdl_rwops_close(SDL_RWops *context)
{
    //if ( context ) {
    //  free(context);
    //}
    return(0);
}
//-----------------------------------------------------------------------------------------------------
static int sdl_rwops_seek(SDL_RWops *context, int offset, int whence)
{
    Uint8 *newpos;

    switch (whence) {
        case SEEK_SET:
            newpos = context->hidden.mem.base+offset;
            break;
        case SEEK_CUR:
            newpos = context->hidden.mem.here+offset;
            break;
        case SEEK_END:
            newpos = context->hidden.mem.stop+offset;
            break;
        default:
            SDL_SetError("Unknown value for 'whence'");
            return(-1);
    }
    if ( newpos < context->hidden.mem.base ) {
        newpos = context->hidden.mem.base;
    }
    if ( newpos > context->hidden.mem.stop ) {
        newpos = context->hidden.mem.stop;
    }
    context->hidden.mem.here = newpos;
    return (int) (context->hidden.mem.here-context->hidden.mem.base);
}
//-----------------------------------------------------------------------------------------------------
static int sdl_rwops_write(SDL_RWops *context, const void *ptr, int size, int num)
{
    if ( (context->hidden.mem.here + (num*size)) > context->hidden.mem.stop ) {
        num = (int) (context->hidden.mem.stop-context->hidden.mem.here)/size;
    }
    memcpy(context->hidden.mem.here, ptr, num*size);
    context->hidden.mem.here += num*size;
    return(num);
}
//-----------------------------------------------------------------------------------------------------
static int sdl_rwops_read(SDL_RWops *context, void * ptr, int size, int maxnum)
{
    int num;

    num = maxnum;
    if ( (context->hidden.mem.here + (num*size)) > context->hidden.mem.stop ) {
        num = (int) (context->hidden.mem.stop-context->hidden.mem.here)/size;
    }
    memcpy(ptr, context->hidden.mem.here, num*size);
    context->hidden.mem.here += num*size;
    return(num);
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
/*virtual*/ void OrbiterSDLBluetooth::DisplayImageOnScreen(struct SDL_Surface *pScreenImage)
{
    unsigned char ImageType = 0;
    unsigned long ImageSize;
    char *pImage;
/*
    SDL_RWops *dst = new SDL_RWops;

    dst->hidden.mem.base = NULL;

    dst->close = &sdl_rwops_close;
    dst->seek  = &sdl_rwops_seek;
    dst->write = &sdl_rwops_write;
    dst->read  = &sdl_rwops_read;

    int iSize = pScreenImage->h * pScreenImage->pitch;
    dst->hidden.mem.base = new Uint8[iSize];
    dst->hidden.mem.here = dst->hidden.mem.base;
    dst->hidden.mem.stop = dst->hidden.mem.base + iSize;

    SDL_SaveBMP_RW(pScreenImage, dst, 1);

    pImage = (char *)(dst->hidden.mem.base);
    unsigned int *pImageSize = (unsigned int *)(dst->hidden.mem.base + 2);
    ImageSize = (unsigned int)*pImageSize;
*/

	const string csTempFileName = "TmpScreen.png";
    SaveImageToFile(pScreenImage, csTempFileName);

    FILE* file;
    file = fopen(csTempFileName.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    ImageSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    pImage = new char[ImageSize];

    fread(pImage, 1, ImageSize, file);
    fclose(file);

    BD_CP_ShowImage *pBD_CP_ShowImage = new BD_CP_ShowImage(ImageType, ImageSize, pImage);

    delete pImage;
    pImage = NULL;


//  delete dst->hidden.mem.base;
//  dst->hidden.mem.base = NULL;

//  delete dst;
//  dst = NULL;

    if( m_bShowListSent && !m_vectObjs_GridsOnScreen.size() )
    {
        list<string> listGrid;
        listGrid.clear();

        //TODO: add an ON/OFF param! [HACK :D]
        BD_CP_ShowList *pBD_CP_ShowList = new BD_CP_ShowList(0, 0, 0, 0, listGrid, false, false);
        if( m_pBDCommandProcessor )
        {
            m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList);
            m_bShowListSent = false;
        }
    }

    if( m_pBDCommandProcessor )
        m_pBDCommandProcessor->AddCommand(pBD_CP_ShowImage);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDLBluetooth::RenderDataGrid(DesignObj_DataGrid *pObj)
{
    PLUTO_SAFETY_LOCK(dg,m_DatagridMutex);
    string delSelections;
    if (!pObj->sSelVariable.empty())
    {
        PLUTO_SAFETY_LOCK(vm,m_VariableMutex)
        delSelections = "|"+m_mapVariable[atoi(pObj->sSelVariable.c_str())]+"|";
        vm.Release();
    }

#if (defined(PROFILING))
    clock_t clkStart = clock();
#endif

    string Unused;
    Orbiter::PrepareRenderDataGrid(pObj, Unused);

    if(pObj->m_pDataGridTable)
    {
        if(pObj->m_pDataGridTable->m_ColumnCount != 1)
        {
            g_pPlutoLogger->Write(LV_WARNING,
                "OrbiterSDLBluetooth: I don't know how to render grids with more then one columns, "
				"I'll let the base to render as an image"
            );

			OrbiterSDL::RenderDataGrid(pObj);
            return;
        }

		//now I have a grid with one column.. I know how to render it on the phone

        int x       = pObj->m_rPosition.X;
        int y       = pObj->m_rPosition.Y;
        int Width   = pObj->m_rPosition.Width;
        int Height  = pObj->m_rPosition.Height;

        list<string> listGrid;

		int iSelectedColumn = 0;
		if(pObj->m_sExtraInfo.find( 'c' ) != string::npos)
		{
			g_pPlutoLogger->Write(LV_STATUS, "Extraoptions in grid: %s", pObj->m_sExtraInfo.c_str());
			
			int iPos = pObj->m_sExtraInfo.find( 'c' ); 

			if(iPos + 1 < pObj->m_sExtraInfo.size())
				iSelectedColumn = pObj->m_sExtraInfo[iPos + 1] - '0' - 1;
		}

		bool bSendSelectedOnMove = false; //when use press up/down buttons, PlutoMO will send a SelectedItem command
		if(pObj->m_sExtraInfo.find( 'T' ) != string::npos)
		{
			bSendSelectedOnMove = true;
		}

		bool bTurnOn = true;
		g_pPlutoLogger->Write(LV_WARNING, "About to send BD_CP_ShowList command, column %d, turnon %d, items count %d",
				iSelectedColumn, bTurnOn, pObj->m_pDataGridTable->getTotalRowCount());
		
        for(int i = 0; i < pObj->m_pDataGridTable->getTotalRowCount(); i++)
        {
            DataGridCell * pCell = pObj->m_pDataGridTable->GetData(iSelectedColumn, i);

			string sItem = pCell != NULL ? pCell->GetText() : "<empty>";
			g_pPlutoLogger->Write(LV_STATUS, "Item %d : %s", i, sItem.c_str());
			
            listGrid.push_back(sItem);
        }

        BD_CP_ShowList *pBD_CP_ShowList = new BD_CP_ShowList(x, y, Width, Height, listGrid, 
			bSendSelectedOnMove, bTurnOn);

        if( m_pBDCommandProcessor )
        {
            m_pBDCommandProcessor->AddCommand(pBD_CP_ShowList);
            m_bShowListSent = true;
        }

        listGrid.clear();
    }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDLBluetooth::CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage)
{
    if(
        Orbiter::BuildCaptureKeyboardParams(
            sPK_DesignObj,
            iPK_Variable,
            sOnOff,
            sType,
            sReset,
            iPK_Text,
            bDataGrid
        )
    )
    {
		BD_CP_CaptureKeyboard *pBD_CP_CaptureKeyboard = new BD_CP_CaptureKeyboard(
				m_bCaptureKeyboard_OnOff,
				m_bCaptureKeyboard_Reset,
				m_bCaptureKeyboard_TypePin,
				m_bCaptureKeyboard_DataGrid,
				m_iCaptureKeyboard_PK_Variable,
				m_sCaptureKeyboard_Text
		);

		if( m_pBDCommandProcessor )
			m_pBDCommandProcessor->AddCommand(pBD_CP_CaptureKeyboard);
    }
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLBluetooth::SimulateMouseClick(int x, int y)
{
	//do nothing
#ifndef WIN32
	printf("OrbiterSDLBluetooth::SimulateMouseClick. The event will be ignored (we don't have mouse on the phone :P");
#endif	
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLBluetooth::SimulateKeyPress(long key)
{
	BD_CP_SimulateEvent *pBD_CP_SimulateEvent = new BD_CP_SimulateEvent(0, key);

	if( m_pBDCommandProcessor )
		m_pBDCommandProcessor->AddCommand(pBD_CP_SimulateEvent);

#ifndef WIN32
	printf("OrbiterSDLBluetooth::SimulateKeyPress with key code: %d", key);
#endif
}
//-----------------------------------------------------------------------------------------------------
