/***************************************************************************
                         menu.c  -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#define _IN_MENU

#include "externals.h"
#include "draw.h"
#include "menu.h"
#include "gpu.h"

#ifdef _WINDOWS

#include "record.h"

HFONT hGFont=NULL;
BOOL  bTransparent=FALSE;

#endif

unsigned long dwCoreFlags = 0;

// create lists/stuff for fonts (actually there are no more lists, but I am too lazy to change the func names ;)
void InitMenu(void)
{
#ifdef _WINDOWS
 hGFont=CreateFont(//-8,
                   13,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                   ANSI_CHARSET,OUT_DEFAULT_PRECIS,
                   CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                   DEFAULT_PITCH,
                   //"Courier New");
                   //"MS Sans Serif");
                   "Arial");
#endif
}

// kill existing lists/fonts
void CloseMenu(void)
{
#ifdef _WINDOWS
 if(hGFont) DeleteObject(hGFont);
 hGFont=NULL;
#else
 DestroyPic();
#endif
}

// DISPLAY FPS/MENU TEXT

#include <time.h>
extern time_t tStart;

int iMPos=0;                                           // menu arrow pos

void DisplayText(void)                                 // DISPLAY TEXT
{
#ifdef _WINDOWS
 HDC hdc;HFONT hFO;

 IDirectDrawSurface_GetDC(DX.DDSRender,&hdc);
 hFO=(HFONT)SelectObject(hdc,hGFont);

 SetTextColor(hdc,RGB(0,255,0));
 if(bTransparent) 
      SetBkMode(hdc,TRANSPARENT);
 else SetBkColor(hdc,RGB(0,0,0));

 if(szDebugText[0] && ((time(NULL) - tStart) < 2))     // special debug text? show it
  {
   RECT r={0,0,1024,1024};
   DrawText(hdc,szDebugText,lstrlen(szDebugText),&r,DT_LEFT|DT_NOCLIP);
  }
 else                                                  // else standard gpu menu
  {
   szDebugText[0]=0;
   lstrcat(szDispBuf,szMenuBuf);
   ExtTextOut(hdc,0,0,0,NULL,szDispBuf,lstrlen(szDispBuf),NULL);
  }

 SelectObject(hdc,hFO);
 IDirectDrawSurface_ReleaseDC(DX.DDSRender,hdc);
#endif
}

// Build Menu buffer (== Dispbuffer without FPS)...
void BuildDispMenu(int iInc)
{
 if(!(ulKeybits&KEY_SHOWFPS)) return;                  // mmm, cheater ;)

 iMPos+=iInc;                                          // up or down
 if(iMPos<0) iMPos=3;                                  // wrap around
 if(iMPos>3) iMPos=0;

 strcpy(szMenuBuf,"   FL   FS   DI   GF        ");     // main menu items

 if(UseFrameLimit)                                     // set marks
  {
   if(iFrameLimit==1) szMenuBuf[2]  = '+';
   else               szMenuBuf[2]  = '*';
  }
 if(iFastFwd)       szMenuBuf[7]  = '~';
 else
 if(UseFrameSkip)   szMenuBuf[7]  = '*';

 if(iUseDither)                                        // set marks
  {
   if(iUseDither==1) szMenuBuf[12]  = '+';
   else              szMenuBuf[12]  = '*';
  }

 if(dwActFixes)     szMenuBuf[17] = '*';

 if(dwCoreFlags&1)  szMenuBuf[23]  = 'A';
 if(dwCoreFlags&2)  szMenuBuf[23]  = 'M';

 if(dwCoreFlags&0xff00)                                //A/M/G/D   
  {
   if((dwCoreFlags&0x0f00)==0x0000)                    // D
    szMenuBuf[23]  = 'D';
   else
   if((dwCoreFlags&0x0f00)==0x0100)                    // A
    szMenuBuf[23]  = 'A';
   else
   if((dwCoreFlags&0x0f00)==0x0200)                    // M
    szMenuBuf[23]  = 'M';
   else
   if((dwCoreFlags&0x0f00)==0x0300)                    // G
    szMenuBuf[23]  = 'G';

   szMenuBuf[24]='0'+(char)((dwCoreFlags&0xf000)>>12);                         // number
  }

#ifdef _WINDOWS
 if(bVsync_Key)     szMenuBuf[25]  = 'V';
#endif

 if(lSelectedSlot)  szMenuBuf[26]  = '0'+(char)lSelectedSlot;   

 szMenuBuf[(iMPos+1)*5]='<';                           // set arrow

#ifdef _WINDOWS
 if(RECORD_RECORDING)
  {
   szMenuBuf[27]  = ' ';
   szMenuBuf[28]  = ' ';
   szMenuBuf[29]  = ' ';
   szMenuBuf[30]  = 'R';
   szMenuBuf[31]  = 'e';
   szMenuBuf[32]  = 'c';
   szMenuBuf[33]  = 0;
  }

 if(DX.DDSScreenPic) ShowTextGpuPic();
#endif
}

// Some menu action...
void SwitchDispMenu(int iStep)                         // SWITCH DISP MENU
{
 if(!(ulKeybits&KEY_SHOWFPS)) return;                  // tststs

 switch(iMPos)
  {
   case 0:                                             // frame limit
    {
     int iType=0;
     bInitCap = TRUE;

#ifdef _WINDOWS
     if(iFrameLimit==1 && UseFrameLimit &&
        GetAsyncKeyState(VK_SHIFT)&32768)
      {
       fFrameRate+=iStep;
       if(fFrameRate<3.0f) fFrameRate=3.0f;
       SetAutoFrameCap();
       break;
      }
#endif

     if(UseFrameLimit) iType=iFrameLimit;
     iType+=iStep;
     if(iType<0) iType=2;
     if(iType>2) iType=0;
     if(iType==0) UseFrameLimit=0;
     else
      {
       UseFrameLimit=1;
       iFrameLimit=iType;
       SetAutoFrameCap();
      }
    } break;

   case 1:                                             // frame skip
    bInitCap = TRUE;
    if(iStep>0)
     {
      if(!UseFrameSkip) {UseFrameSkip=1;iFastFwd = 0;}
      else
       {
        if(!iFastFwd) iFastFwd=1;
        else {UseFrameSkip=0;iFastFwd = 0;}
       }
     }
    else
     {
      if(!UseFrameSkip) {UseFrameSkip=1;iFastFwd = 1;}
      else
       {
        if(iFastFwd) iFastFwd=0;
        else {UseFrameSkip=0;iFastFwd = 0;}
       }
     }
    bSkipNextFrame=FALSE;
    break;

   case 2:                                             // dithering
    iUseDither+=iStep;
    if(iUseDither<0) iUseDither=2;
    if(iUseDither>2) iUseDither=0;
    break;

   case 3:                                             // special fixes
    if(iUseFixes) {iUseFixes=0;dwActFixes=0;}
    else          {iUseFixes=1;dwActFixes=dwCfgFixes;}
    SetFixes();
    if(iFrameLimit==2) SetAutoFrameCap();
    break;
  }

 BuildDispMenu(0);                                     // update info
}
