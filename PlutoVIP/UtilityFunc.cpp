#include "stdafx.h"
#include "UtilityFunc.h"


typedef DWORD (WINAPI *PSLWA)(HWND, DWORD, BYTE, DWORD);

static PSLWA pSetLayeredWindowAttributes = NULL;
static BOOL initialized = FALSE;


BOOL MakeWindowTransparent(HWND hWnd, unsigned char factor)
{
   /* First, see if we can get the API call we need. If we've tried
    * once, we don't need to try again. */
   if (!initialized)
   {
      HMODULE hDLL = LoadLibrary ("user32");

      pSetLayeredWindowAttributes = 
         (PSLWA) GetProcAddress(hDLL, "SetLayeredWindowAttributes");

      initialized = TRUE;
   }

   if (pSetLayeredWindowAttributes == NULL) 
      return FALSE;

   /* Windows need to be layered to be made transparent. This is done
    * by modifying the extended style bits to contain WS_EX_LAYARED. */
   SetLastError(0);

   SetWindowLong(hWnd, 
                 GWL_EXSTYLE , 
                 GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

   if (GetLastError())
      return FALSE;

   /* Now, we need to set the 'layered window attributes'. This
    * is where the alpha values get set. */
   return pSetLayeredWindowAttributes (hWnd, 
                                       RGB(255,255,255), 
                                       factor,
                                       LWA_COLORKEY|LWA_ALPHA);
}

